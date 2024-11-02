#ifndef __STM32_FFB_PRO_H
#define __STM32_FFB_PRO_H

#include <stdint.h>
#include "main.h"

// Maximum number of effects
#define MAX_EFFECTS 19  // Effect IDs start at 2

// Effect States
#define MEffectState_Free           0x00
#define MEffectState_Allocated      0x01
#define MEffectState_Playing        0x02
#define MEffectState_SentToJoystick 0x04

// Duration Constants
#define USB_DURATION_INFINITE   0xFFFF
#define MIDI_DURATION_INFINITE  0x0000
#define USB_SAMPLEPERIOD_DEFAULT 0x0000

// USB Effect Types
#define USB_EFFECT_CONSTANT     0x01
#define USB_EFFECT_RAMP         0x02
#define USB_EFFECT_SQUARE       0x03
#define USB_EFFECT_SINE         0x04
#define USB_EFFECT_TRIANGLE     0x05
#define USB_EFFECT_SAWTOOTHDOWN 0x06
#define USB_EFFECT_SAWTOOTHUP   0x07
#define USB_EFFECT_SPRING       0x08
#define USB_EFFECT_DAMPER       0x09
#define USB_EFFECT_INERTIA      0x0A
#define USB_EFFECT_FRICTION     0x0B
#define USB_EFFECT_CUSTOM       0x0C

#define USB_EFFECT_BLOCK        0x01
#define USB_ENVELOPE_REPORT     0x02
#define USB_CONDITION_REPORT    0x03
#define USB_PERIODIC_REPORT     0x04
#define USB_CONSTANT_FORCE      0x05
#define USB_BLOCK_FREE         0x0B
#define USB_DEVICE_CONTROL     0x0C
#define USB_DEVICE_GAIN        0x0D
#define USB_EFFECT_OPERATION   0x0A

// Device Control Commands
#define USB_DCTRL_ACTUATORS_ENABLE  0x01
#define USB_DCTRL_ACTUATORS_DISABLE 0x02
#define USB_DCTRL_STOPALL          0x03
#define USB_DCTRL_RESET            0x04
#define USB_DCTRL_PAUSE            0x05
#define USB_DCTRL_CONTINUE         0x06

// MIDI Protocol Constants
#define FFP_MIDI_MODIFY_DURATION    0x40
#define FFP_MIDI_MODIFY_DIRECTION   0x48
#define FFP_MIDI_MODIFY_GAIN        0x4C
#define FFP_MIDI_MODIFY_SAMPLERATE  0x50
#define FFP_MIDI_MODIFY_ATTACK      0x64
#define FFP_MIDI_MODIFY_ATTACKTIME  0x5C
#define FFP_MIDI_MODIFY_MAGNITUDE   0x68
#define FFP_MIDI_MODIFY_FADETIME    0x60
#define FFP_MIDI_MODIFY_FADE        0x6C
#define FFP_MIDI_MODIFY_FREQUENCY   0x70
#define FFP_MIDI_MODIFY_PARAM1      0x74
#define FFP_MIDI_MODIFY_PARAM2      0x78

// Data Structures
typedef struct {
    uint8_t spring : 1;
    uint8_t constant : 1;
    uint8_t ramp : 1;
    uint8_t square : 1;
    uint8_t sine : 1;
    uint8_t triangle : 1;
    uint8_t sawtoothup : 1;
    uint8_t sawtoothdown : 1;
} TDisabledEffectTypes;

typedef struct {
    uint8_t command;      // 0x23
    uint8_t waveForm;     // Effect type
    uint8_t unknown1;     // Always 0x7F
    uint16_t duration;    // unit=2ms
    uint8_t gain;         // Effect gain
    uint16_t frequency;   // For periodic effects
    uint8_t magnitude;    // Effect strength
    uint16_t direction;   // Effect direction
    uint8_t attackLevel;  // Envelope attack level
    uint8_t fadeLevel;    // Envelope fade level
    uint16_t attackTime;  // Envelope attack time
} FFP_MIDI_Effect_Basic;

typedef struct {
    uint8_t waveForm;
} midi_data_common_t;

typedef struct {
    uint8_t state;
    uint8_t share_data[12];  // Shared between USB reports for calculating MIDI parameters
    volatile uint8_t data[27];  // MIDI message data
} TEffectState;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t effectType;
    uint16_t duration;
    uint16_t triggerRepeatInterval;
    uint16_t samplePeriod;
    uint8_t gain;
    uint8_t triggerButton;
    uint8_t axes;
    uint8_t direction[2];
} FFP_Effect_Params;

// USB Report Structures
typedef struct {
    uint8_t reportId;
    uint8_t status;
    uint8_t effectBlockIndex;
} USB_FFBReport_PIDStatus_Input_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t effectType;
    uint16_t duration;
    uint16_t triggerRepeatInterval;
    uint16_t samplePeriod;
    uint8_t gain;
    uint8_t triggerButton;
    uint8_t enableAxis;
    uint8_t directionX;
    uint8_t directionY;
} USB_FFBReport_SetEffect_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t attackLevel;
    uint8_t fadeLevel;
    uint16_t attackTime;
    uint16_t fadeTime;
} USB_FFBReport_SetEnvelope_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    int8_t cpOffset;
    uint8_t positiveCoefficient;
    uint8_t negativeCoefficient;
    uint8_t parameterBlockOffset;
} USB_FFBReport_SetCondition_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    int16_t magnitude;
} USB_FFBReport_SetConstantForce_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t magnitude;
    int8_t offset;
    uint8_t phase;
    uint16_t period;
} USB_FFBReport_SetPeriodic_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t operation;
    uint8_t loopCount;
} USB_FFBReport_EffectOperation_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
} USB_FFBReport_BlockFree_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t control;
} USB_FFBReport_DeviceControl_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t gain;
} USB_FFBReport_DeviceGain_Output_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectType;
} USB_FFBReport_CreateNewEffect_Feature_Data_t;

typedef struct {
    uint8_t reportId;
    uint8_t effectBlockIndex;
    uint8_t loadStatus;
    uint16_t ramPoolAvailable;
} USB_FFBReport_PIDBlockLoad_Feature_Data_t;

// Function Prototypes
uint8_t FFP_Init(void);
void FFP_SendData(const uint8_t* data, uint16_t len);
void FFP_SetGain(uint8_t gain);
void FFP_StartEffect(uint8_t id);
void FFP_StopEffect(uint8_t id);
void FFP_FreeEffect(uint8_t id);
uint8_t FFP_DeviceControl(uint8_t control);

void FFP_HandleCondition(USB_FFBReport_SetCondition_Output_Data_t* data);
void FFP_HandleSetEffect(USB_FFBReport_SetEffect_Output_Data_t* data);
void FFP_HandlePeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data);
void FFP_HandleConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data);
void FFP_HandleEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data);
void FFP_HandleEffectOperation(USB_FFBReport_EffectOperation_Output_Data_t* data);
void FFP_HandleBlockFree(USB_FFBReport_BlockFree_Output_Data_t* data);
void FFP_HandleDeviceControl(USB_FFBReport_DeviceControl_Output_Data_t* data);
void FFP_HandleDeviceGain(USB_FFBReport_DeviceGain_Output_Data_t* data);

void FFP_CreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData,
                        USB_FFBReport_PIDBlockLoad_Feature_Data_t* outData);

#endif
