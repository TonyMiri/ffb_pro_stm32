#ifndef __USB_HID_FFB_H__
#define __USB_HID_FFB_H__

#include <stdint.h>

// Original standard definitions
#define GET_STATUS           0
#define CLEAR_FEATURE       1
#define SET_FEATURE         3
#define SET_ADDRESS         5
#define GET_DESCRIPTOR      6
#define GET_CONFIGURATION   8
#define SET_CONFIGURATION   9
#define GET_INTERFACE       10
#define SET_INTERFACE       11

// HID requests
#define HID_GET_REPORT      1
#define HID_GET_IDLE        2
#define HID_GET_PROTOCOL    3
#define HID_SET_REPORT      9
#define HID_SET_IDLE        10
#define HID_SET_PROTOCOL    11

// Force Feedback Report IDs
#define USB_EFFECT_BLOCK        0x01
#define USB_ENVELOPE_REPORT     0x02
#define USB_CONDITION_REPORT    0x03
#define USB_PERIODIC_REPORT     0x04
#define USB_CONSTANT_FORCE      0x05
#define USB_RAMP_FORCE         0x06
#define USB_EFFECT_OPERATION    0x0A
#define USB_BLOCK_FREE         0x0B
#define USB_DEVICE_CONTROL     0x0C
#define USB_DEVICE_GAIN        0x0D
#define USB_CUSTOM_FORCE       0x0E

// Effect Types
#define USB_EFFECT_CONSTANT    0x01
#define USB_EFFECT_RAMP        0x02
#define USB_EFFECT_SQUARE      0x03
#define USB_EFFECT_SINE        0x04
#define USB_EFFECT_TRIANGLE    0x05
#define USB_EFFECT_SAWUP       0x06
#define USB_EFFECT_SAWDOWN     0x07
#define USB_EFFECT_SPRING      0x08
#define USB_EFFECT_DAMPER      0x09
#define USB_EFFECT_INERTIA     0x0A
#define USB_EFFECT_FRICTION    0x0B

// Report Structures
typedef struct {
    uint8_t reportId;      // =USB_EFFECT_BLOCK
    uint8_t effectBlockIndex;  // 1-40
    uint8_t effectType;    // Effect type
    uint16_t duration;     // 0-32767 ms
    uint16_t triggerButton;  // Button that triggers effect
    uint16_t samplePeriod;   // Sample period
    uint8_t gain;         // 0-255
    uint8_t trigerRepeatInterval; // 0-255
} USB_FFBReport_SetEffect_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t magnitude;      // 0-255
    int8_t offset;
    uint8_t phase;         // 0-255
    uint16_t period;       // 0-32767 ms
} USB_FFBReport_SetPeriodic_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    int16_t magnitude;     // -255 to 255
} USB_FFBReport_SetConstantForce_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t attackLevel;   // 0-255
    uint8_t fadeLevel;     // 0-255
    uint16_t attackTime;   // ms
    uint16_t fadeTime;     // ms
} USB_FFBReport_SetEnvelope_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t parameterBlockOffset;
    int8_t cpOffset;
    uint8_t positiveCoefficient;
    uint8_t negativeCoefficient;
} USB_FFBReport_SetCondition_Output_Data_t;

// Helper macros from original
#define LSB(n) ((uint16_t)(n) & 0xFF)
#define MSB(n) (((uint16_t)(n) >> 8) & 0xFF)
#define LVAL(n) LSB(n), MSB(n)

#endif // __USB_HID_FFB_H__
