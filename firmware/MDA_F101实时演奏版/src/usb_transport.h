#pragma once
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void mda_usb_init(void);
bool mda_usb_request(uint8_t *request);
bool mda_usb_send(const void *data,uint32_t length);
#ifdef __cplusplus
}
#endif
