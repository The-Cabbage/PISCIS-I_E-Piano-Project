#include <zephyr/kernel.h>
/* Both libraries define this helper; retain Zephyr's version outside the header. */
#pragma push_macro("DIV_ROUND_CLOSEST")
#undef DIV_ROUND_CLOSEST
#include "usbh_core.h"
#pragma pop_macro("DIV_ROUND_CLOSEST")
#include "midi_host.h"

/* Initial scope: one MIDI 1.0 streaming interface, alt 0, first Bulk IN.
 * Static storage and a bounded synchronous URB keep teardown simple: the hub
 * disconnect callback waits for at most one read before clearing its pointer.
 */
K_MUTEX_DEFINE(midi_lock);
static struct usbh_hubport *port;
static struct usb_endpoint_descriptor *input_ep;
static struct usbh_urb read_urb;
static bool stopped;
static uint8_t rx[512] USB_MEM_ALIGNX;
extern uintptr_t usb_sunxi_ehci_base(void);

static void emit(uint8_t type, uint8_t cable, uint8_t channel,
                 uint8_t key, uint8_t value, int detail)
{
    const struct midi_event event = {
        .time_ms = k_uptime_get_32(), .type = type, .cable = cable,
        .channel = channel, .key = key, .value = value, .detail = detail
    };
    midi_event_push(&event);
}

/* Validate the class-specific MS header, not just the generic Audio class. */
static bool is_midi1(struct usbh_hubport *p, uint8_t number)
{
    const uint8_t *d = p->raw_config_desc;
    const size_t total = p->config.config_desc.wTotalLength;
    bool selected = false, found = false;
    if (!d || total < 9 || total >= CONFIG_USBHOST_REQUEST_BUFFER_LEN)
        return false;
    for (size_t i = 0; i < total;) {
        if (total - i < 2 || d[i] < 2 || d[i] > total - i)
            return false;
        const uint8_t len = d[i], type = d[i + 1];
        if (type == 4) {
            if (len < 9) return false;
            selected = d[i + 2] == number && d[i + 3] == 0 &&
                       d[i + 5] == 1 && d[i + 6] == 3 && d[i + 7] == 0;
        } else if (selected && type == 0x24 && len >= 3 && d[i + 2] == 1) {
            if (len < 7 || d[i + 3] != 0 || d[i + 4] != 1)
                return false;
            found = true;
        }
        i += len;
    }
    return found;
}

static int midi_connect(struct usbh_hubport *p, uint8_t intf)
{
    if (intf >= CONFIG_USBHOST_MAX_INTERFACES) return -USB_ERR_INVAL;
    struct usbh_interface *iface = &p->config.intf[intf];
    struct usbh_interface_altsetting *alt = &iface->altsetting[0];
    struct usb_endpoint_descriptor *ep = NULL;
    if (!iface->altsetting_num || alt->intf_desc.bAlternateSetting != 0 ||
        !is_midi1(p, alt->intf_desc.bInterfaceNumber)) {
        printk("MIDI: unsupported descriptor (requires MIDI 1.0 alt 0)\n");
        return -USB_ERR_NOTSUPP;
    }
    unsigned count = alt->intf_desc.bNumEndpoints;
    if (count > CONFIG_USBHOST_MAX_ENDPOINTS) return -USB_ERR_INVAL;
    for (unsigned i = 0; i < count; ++i) {
        struct usb_endpoint_descriptor *candidate = &alt->ep[i].ep_desc;
        if ((candidate->bEndpointAddress & 0x80) &&
            (candidate->bmAttributes & 3) == 2) {
            ep = candidate;
            break;
        }
    }
    if (!ep || ep->wMaxPacketSize < 4 || ep->wMaxPacketSize > sizeof(rx) ||
        ep->wMaxPacketSize % 4) return -USB_ERR_NOTSUPP;

    k_mutex_lock(&midi_lock, K_FOREVER);
    if (port) {
        k_mutex_unlock(&midi_lock);
        printk("MIDI: only one streaming interface is supported\n");
        return -USB_ERR_BUSY;
    }
    memset(&read_urb, 0, sizeof(read_urb));
    port = p;
    input_ep = ep;
    stopped = false;
    iface->priv = &read_urb;
    emit(MIDI_CONNECTED, 0, 0, ep->bEndpointAddress, p->dev_addr,
         ep->wMaxPacketSize);
    k_mutex_unlock(&midi_lock);
    return 0;
}

static int midi_disconnect(struct usbh_hubport *p, uint8_t intf)
{
    k_mutex_lock(&midi_lock, K_FOREVER);
    if (intf < CONFIG_USBHOST_MAX_INTERFACES &&
        port == p && p->config.intf[intf].priv == &read_urb) {
        /* No URB is pending: poll holds this lock through synchronous completion. */
        p->config.intf[intf].priv = NULL;
        port = NULL;
        input_ep = NULL;
        stopped = false;
        emit(MIDI_DISCONNECTED, 0, 0, 0, 0, 0);
    }
    k_mutex_unlock(&midi_lock);
    return 0;
}

static const struct usbh_class_driver midi_driver = {
    .driver_name = "midi1", .connect = midi_connect, .disconnect = midi_disconnect
};

CLASS_INFO_DEFINE const struct usbh_class_info f101_midi1_info = {
    .match_flags = USB_CLASS_MATCH_INTF_CLASS | USB_CLASS_MATCH_INTF_SUBCLASS |
                   USB_CLASS_MATCH_INTF_PROTOCOL,
    .bInterfaceClass = 0x01, .bInterfaceSubClass = 0x03,
    .bInterfaceProtocol = 0x00, .id_table = NULL, .class_driver = &midi_driver
};

static bool is_volume_controller(uint8_t controller)
{
    if (controller == 7) return true;
    // This Keystation Mini 32 uses a stored custom assignment for its knob.
    // Keep CC79 device-specific because other controllers may use it for sound.
    return controller == 79 && port->device_desc.idVendor == 0x0a4d &&
           port->device_desc.idProduct == 0x129d;
}

static bool decode(const uint8_t *b)
{
    const uint8_t cin = b[0] & 15, cable = b[0] >> 4;
    const uint8_t command = b[1] >> 4, channel = b[1] & 15;
    /* Ignore message types outside the standalone instrument's controls. */
    if (cin != 8 && cin != 9 && cin != 11 && cin != 12) return true;
    if (command != cin || (b[2] & 0x80) || (cin != 12 && (b[3] & 0x80))) return false;
    if (cin == 12) {
        emit(MIDI_PROGRAM, cable, channel, 0, b[2], 0);
        return true;
    }
    if (cin == 8 || (cin == 9 && b[3] == 0))
        emit(MIDI_NOTE_OFF, cable, channel, b[2], b[3], 0);
    else if (cin == 9)
        emit(MIDI_NOTE_ON, cable, channel, b[2], b[3], 0);
    else if (is_volume_controller(b[2]))
        emit(MIDI_VOLUME, cable, channel, b[2], b[3], 0);
    else if (b[2] == 64)
        emit(MIDI_SUSTAIN, cable, channel, 64, b[3], 0);
    else if (b[2] == 120 || b[2] == 123)
        emit(MIDI_ALL_NOTES_OFF, cable, channel, b[2], b[3], 0);
    else if (b[2] == 121)
        emit(MIDI_RESET_CONTROLLERS, cable, channel, b[2], b[3], 0);
    return true;
}

void midi_host_poll(void)
{
    k_mutex_lock(&midi_lock, K_FOREVER);
    if (port && port->connected && !stopped) {
        read_urb.actual_length = 0;
        usbh_bulk_urb_fill(&read_urb, port, input_ep, rx,
                           input_ep->wMaxPacketSize, 50, NULL, NULL);
        const int ret = usbh_submit_urb(&read_urb);
        if (ret == 0) {
            const uint32_t length = read_urb.actual_length;
            if (length > sizeof(rx) || length % 4) {
                emit(MIDI_ERROR, 0, 0, 0, 0, -USB_ERR_INVAL);
            } else {
                unsigned bad = 0;
                for (uint32_t i = 0; i < length; i += 4)
                    if (!decode(rx + i)) ++bad;
                if (bad) emit(MIDI_ERROR, 0, 0, 0, 0, -USB_ERR_INVAL);
            }
        } else if (ret != -USB_ERR_TIMEOUT && ret != -USB_ERR_NAK) {
            /* Stop on STALL/transport error instead of busy-looping or retaining
             * notes. First-stage recovery is unplug/replug, explicitly logged. */
            stopped = true;
            emit(MIDI_ERROR, 0, 0, 0, 0, ret);
            emit(MIDI_RESET, 0, 0, 0, 0, ret);
        }
    }
    k_mutex_unlock(&midi_lock);
}

int midi_host_start(void)
{
    return usbh_initialize(0, usb_sunxi_ehci_base(), NULL);
}
