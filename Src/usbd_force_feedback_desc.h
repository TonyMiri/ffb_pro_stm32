#ifndef __USBD_FFB_DESC_H
#define __USBD_FFB_DESC_H

#include <stdint.h>

// Report IDs
#define FFB_ID_STATE         0x02  // Input
#define FFB_ID_EFFECT       0x01  // Output
#define FFB_ID_ENVELOPE     0x02  // Output
#define FFB_ID_CONDITION    0x03  // Output
#define FFB_ID_PERIODIC     0x04  // Output
#define FFB_ID_CONSTANT     0x05  // Output
#define FFB_ID_RAMP         0x06  // Output
#define FFB_ID_EFFECT_OP    0x0A  // Output
#define FFB_ID_GAIN         0x0D  // Output

// HID descriptor macros
#define HID_RI_USAGE(size, ...)           0x09
#define HID_RI_COLLECTION(size, ...)      0xA1
#define HID_RI_LOGICAL_MINIMUM(size, ...) 0x15
#define HID_RI_LOGICAL_MAXIMUM(size, ...) 0x25
#define HID_RI_PHYSICAL_MINIMUM(size, ...) 0x35
#define HID_RI_PHYSICAL_MAXIMUM(size, ...) 0x45
#define HID_RI_REPORT_COUNT(size, ...)    0x95
#define HID_RI_REPORT_SIZE(size, ...)     0x75
#define HID_RI_INPUT(size, ...)           0x81
#define HID_RI_END_COLLECTION(...)        0xC0
#define HID_RI_USAGE_PAGE(size, ...)      0x05
#define HID_RI_USAGE_MINIMUM(size, ...)   0x19
#define HID_RI_USAGE_MAXIMUM(size, ...)   0x29

#define HID_IOF_DATA                    0x00
#define HID_IOF_VARIABLE                0x02
#define HID_IOF_ABSOLUTE                0x00

extern const uint8_t USBD_FFB_ReportDesc[];
extern const uint16_t USBD_FFB_ReportDesc_Size;

#endif
