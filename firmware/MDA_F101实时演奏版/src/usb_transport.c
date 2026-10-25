/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include "usb_transport.h"
#include "protocol.h"
#undef DIV_ROUND_CLOSEST
#include "usbd_core.h"
#include "usbd_cdc_acm.h"

/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_HS, 0x02)
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_FS, 0x02)
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_FS, 0x02)
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_HS, 0x02)
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "MDA F101 Live",         /* Product */
    "2022123456",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};


// Local transport adapted from CherryUSB demo (Apache-2.0). Dependencies are unchanged.
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t rx_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t tx_buffer[2048];
K_MSGQ_DEFINE(request_queue,RT_REQUEST,2,4);
static uint8_t assembling[RT_REQUEST];
static uint32_t assembled;
K_SEM_DEFINE(done_sem,0,1);
static volatile bool ready, dtr_enabled;
static void bulk_out(uint8_t busid,uint8_t ep,uint32_t nbytes) {
    for(uint32_t n=0;n<nbytes;++n) {
        assembling[assembled++]=rx_buffer[n];
        if(assembled==RT_REQUEST) {
            k_msgq_put(&request_queue,assembling,K_NO_WAIT);
            assembled=0;
        }
    }
    usbd_ep_start_read(busid,CDC_OUT_EP,rx_buffer,sizeof(rx_buffer));
}
static void bulk_in(uint8_t busid,uint8_t ep,uint32_t nbytes) {
    if(nbytes && nbytes%usbd_get_ep_mps(busid,ep)==0) {
        if(usbd_ep_start_write(busid,ep,NULL,0)<0)k_sem_give(&done_sem);
    } else k_sem_give(&done_sem);
}
static void events(uint8_t busid,uint8_t event) {
    if(event==USBD_EVENT_CONFIGURED) {
        ready=true;
        usbd_ep_start_read(busid,CDC_OUT_EP,rx_buffer,sizeof(rx_buffer));
    } else if(event==USBD_EVENT_RESET || event==USBD_EVENT_DISCONNECTED) {
        ready=false;dtr_enabled=false;assembled=0;k_msgq_purge(&request_queue);
    }
}
static struct usbd_endpoint out_ep={.ep_addr=CDC_OUT_EP,.ep_cb=bulk_out};
static struct usbd_endpoint in_ep={.ep_addr=CDC_IN_EP,.ep_cb=bulk_in};
static struct usbd_interface intf0,intf1;
extern uintptr_t usb_sunxi_otg_base(void);
void mda_usb_init(void) {
    usbd_desc_register(0,&cdc_descriptor);
    usbd_add_interface(0,usbd_cdc_acm_init_intf(0,&intf0));
    usbd_add_interface(0,usbd_cdc_acm_init_intf(0,&intf1));
    usbd_add_endpoint(0,&out_ep);usbd_add_endpoint(0,&in_ep);
    usbd_initialize(0,usb_sunxi_otg_base(),events);
}
void usbd_cdc_acm_set_dtr(uint8_t busid,uint8_t intf,bool dtr) {
    dtr_enabled=dtr;
    if(!dtr){assembled=0;k_msgq_purge(&request_queue);}
}
bool mda_usb_request(uint8_t *request) { return k_msgq_get(&request_queue,request,K_MSEC(100))==0; }
bool mda_usb_send(const void *data,uint32_t length) {
    const uint8_t *p=data;
    while(length) {
        if(!ready || !dtr_enabled)return false;
        uint32_t n=length>sizeof(tx_buffer)?sizeof(tx_buffer):length;
        memcpy(tx_buffer,p,n);k_sem_reset(&done_sem);
        unsigned int key=irq_lock();
        int ret=usbd_ep_start_write(0,CDC_IN_EP,tx_buffer,n);
        irq_unlock(key);
        if(ret<0)return false;
        if(k_sem_take(&done_sem,K_SECONDS(1))!=0)return false;
        p+=n;length-=n;
    }
    return true;
}
