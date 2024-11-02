#include "stm32-ffb-pro.h"
#include "main.h"
#include "usart.h"
#include <string.h>

// Effect state management
static volatile uint8_t nextEID = 2;  // FFP effect indexes starts from 2
static volatile TEffectState gEffectStates[MAX_EFFECTS+1];
static volatile TDisabledEffectTypes gDisabledEffects;
static volatile USB_FFBReport_PIDStatus_Input_Data_t pidState;
static uint8_t uart_tx_buffer[32];
static volatile uint8_t uart_tx_complete = 1;

// Local function prototypes
static void FFP_SendByte(uint8_t data);
static uint8_t GetNextFreeEffect(void);
static void StopAllEffects(void);
static void FreeAllEffects(void);
static uint16_t UsbUint16ToMidiUint14_Time(uint16_t inUsbValue);
static uint16_t UsbUint16ToMidiUint14(uint16_t inUsbValue);
static int16_t UsbInt8ToMidiInt14(int8_t inUsbValue);

// Initialize the force feedback system
uint8_t FFP_Init(void)
{
    // Initialize state
    memset((void*)&gDisabledEffects, 0, sizeof(gDisabledEffects));
    memset((void*)gEffectStates, 0, sizeof(gEffectStates));
    memset((void*)&pidState, 0, sizeof(pidState));
    nextEID = 2;

    // Initialization sequence
    const uint8_t init_seq_1[] = {0xc5, 0x01};
    const uint8_t init_seq_2[] = {
        0xf0, 0x00, 0x01, 0x0a, 0x01, 0x10, 0x05, 0x6b, 0xf7
    };

    HAL_Delay(100);
    
    FFP_SendData(init_seq_1, sizeof(init_seq_1));
    HAL_Delay(20);
    
    FFP_SendData(init_seq_2, sizeof(init_seq_2));
    HAL_Delay(57);
    
    FFP_SetGain(255);
    
    pidState.reportId = 2;
    pidState.status |= 1 << 2; // Safety Switch: device usable
    pidState.status |= 1 << 4; // Actuator Power: on
    
    return 0;
}

// Send data to the joystick
void FFP_SendData(const uint8_t* data, uint16_t len)
{
    while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX) {
        // Wait for previous transmission to complete
    }
    memcpy(uart_tx_buffer, data, len);
    HAL_UART_Transmit_DMA(&huart1, uart_tx_buffer, len);
}

// UART transmission complete callback
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1) {
        uart_tx_complete = 1;
    }
}

// Effect state management
static uint8_t GetNextFreeEffect(void)
{
    if (nextEID == MAX_EFFECTS)
        return 0;

    uint8_t id = nextEID++;

    while (gEffectStates[nextEID].state != 0) {
        if (nextEID >= MAX_EFFECTS)
            break;
        nextEID++;
    }

    gEffectStates[id].state = MEffectState_Allocated;
    memset((void*)&gEffectStates[id].data, 0, sizeof(gEffectStates[id].data));
    
    return id;
}

void FFP_StartEffect(uint8_t id)
{
    if (id > MAX_EFFECTS)
        return;

    uint8_t cmd[3] = {0xB5, 0x20, id};
    FFP_SendData(cmd, 3);
    gEffectStates[id].state |= MEffectState_Playing;
}

void FFP_StopEffect(uint8_t id)
{
    if (id > MAX_EFFECTS)
        return;

    uint8_t cmd[3] = {0xB5, 0x30, id};
    FFP_SendData(cmd, 3);
    gEffectStates[id].state &= ~MEffectState_Playing;
}

static void StopAllEffects(void)
{
    for (uint8_t id = 1; id <= MAX_EFFECTS; id++)
        FFP_StopEffect(id);
}

void FFP_FreeEffect(uint8_t id)
{
    if (id > MAX_EFFECTS)
        return;

    gEffectStates[id].state = 0;
    if (id < nextEID)
        nextEID = id;
        
    uint8_t cmd[3] = {0xB5, 0x10, id};
    FFP_SendData(cmd, 3);
}

static void FreeAllEffects(void)
{
    nextEID = 2;
    memset((void*)gEffectStates, 0, sizeof(gEffectStates));
    uint8_t cmd[3] = {0xB5, 0x10, 0x7F}; // Free all
    FFP_SendData(cmd, 3);
}

// USB to MIDI conversion utilities
static uint16_t UsbUint16ToMidiUint14_Time(uint16_t inUsbValue)
{
    if (inUsbValue == USB_DURATION_INFINITE)
        return MIDI_DURATION_INFINITE;
    return (inUsbValue & 0x7F00) + ((inUsbValue & 0x00FF) >> 1);
}

static uint16_t UsbUint16ToMidiUint14(uint16_t inUsbValue)
{
    if (inUsbValue == 0xFFFF)
        return 0x0000;
    return ((inUsbValue << 1) & 0x7F00) + ((inUsbValue & 0x007F));
}

static int16_t UsbInt8ToMidiInt14(int8_t inUsbValue)
{
    int16_t value;
    if (inUsbValue < 0) {
        value = inUsbValue;
        value += 0x7f80;
    } else
        value = inUsbValue;
    return value;
}

// Device control
void FFP_SetGain(uint8_t gain)
{
    uint8_t cmd[3] = {0xB5, 0x7C, gain >> 1};
    FFP_SendData(cmd, 3);
}

uint8_t FFP_DeviceControl(uint8_t control)
{
    uint8_t cmd[2] = {0xc5, 0x00};
    
    switch(control) {
        case USB_DCTRL_ACTUATORS_ENABLE:
            cmd[1] = 0x02;
            break;
        case USB_DCTRL_ACTUATORS_DISABLE:
            cmd[1] = 0x03;
            break;
        case USB_DCTRL_STOPALL:
            cmd[1] = 0x06;
            StopAllEffects();
            break;
        case USB_DCTRL_RESET:
            cmd[1] = 0x01;
            FreeAllEffects();
            break;
        case USB_DCTRL_PAUSE:
            cmd[1] = 0x05;
            break;
        case USB_DCTRL_CONTINUE:
            cmd[1] = 0x04;
            break;
        default:
            return 0;
    }
    
    FFP_SendData(cmd, 2);
    return 1;
}

// USB effect processing
void FFP_ProcessEffectReport(uint8_t *data, uint16_t len)
{
    uint8_t reportID = data[0];
    uint8_t effectId = data[1];
    
    switch(reportID) {
        case USB_EFFECT_BLOCK:
            FFP_HandleSetEffect((USB_FFBReport_SetEffect_Output_Data_t*)data);
            break;
            
        case USB_ENVELOPE_REPORT:
            FFP_HandleEnvelope((USB_FFBReport_SetEnvelope_Output_Data_t*)data);
            break;
            
        case USB_CONDITION_REPORT:
            FFP_HandleCondition((USB_FFBReport_SetCondition_Output_Data_t*)data);
            break;
            
        case USB_PERIODIC_REPORT:
            FFP_HandlePeriodic((USB_FFBReport_SetPeriodic_Output_Data_t*)data);
            break;
            
        case USB_CONSTANT_FORCE:
            FFP_HandleConstantForce((USB_FFBReport_SetConstantForce_Output_Data_t*)data);
            break;
            
        case USB_EFFECT_OPERATION:
            FFP_HandleEffectOperation((USB_FFBReport_EffectOperation_Output_Data_t*)data);
            break;
            
        case USB_BLOCK_FREE:
            FFP_HandleBlockFree((USB_FFBReport_BlockFree_Output_Data_t*)data);
            break;
            
        case USB_DEVICE_CONTROL:
            FFP_HandleDeviceControl((USB_FFBReport_DeviceControl_Output_Data_t*)data);
            break;
            
        case USB_DEVICE_GAIN:
            FFP_HandleDeviceGain((USB_FFBReport_DeviceGain_Output_Data_t*)data);
            break;
    }
}

// USB callbacks
void USBD_FFB_OutEvent_Callback(uint8_t *data, uint32_t len)
{
    FFP_ProcessEffectReport(data, len);
}

void FFP_CreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData,
                        USB_FFBReport_PIDBlockLoad_Feature_Data_t* outData)
{
    outData->reportId = 6;
    outData->effectBlockIndex = GetNextFreeEffect();
    
    if (outData->effectBlockIndex == 0) {
        outData->loadStatus = 2;  // Full
    } else {
        outData->loadStatus = 1;  // Success
        volatile TEffectState* effect = &gEffectStates[outData->effectBlockIndex];
        ((midi_data_common_t*)effect->data)->waveForm = inData->effectType;
    }
    
    outData->ramPoolAvailable = 0xFFFF;
}

void FFP_HandleSetEffect(USB_FFBReport_SetEffect_Output_Data_t* data)
{
    volatile TEffectState* effect = &gEffectStates[data->effectBlockIndex];
    volatile FFP_MIDI_Effect_Basic* midi_data = (FFP_MIDI_Effect_Basic*)effect->data;
    
    uint16_t midi_duration;
    if (data->duration == USB_DURATION_INFINITE) {
        midi_duration = MIDI_DURATION_INFINITE;
    } else {
        midi_duration = UsbUint16ToMidiUint14_Time(data->duration);
    }
    
    // Set duration
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->duration = midi_duration;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_DURATION, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = midi_duration & 0x7F;
        cmd[2] = (midi_duration >> 7) & 0x7F;
        FFP_SendData(cmd, 3);
    }
    
    // Set gain
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->gain = (data->gain >> 1) & 0x7F;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_GAIN, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = (data->gain >> 1) & 0x7F;
        cmd[2] = 0;
        FFP_SendData(cmd, 3);
    }
    
    if (!(effect->state & MEffectState_SentToJoystick)) {
        effect->state |= MEffectState_SentToJoystick;
        FFP_SendData((uint8_t*)midi_data, sizeof(FFP_MIDI_Effect_Basic));
    }
}

void FFP_HandlePeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data)
{
    volatile TEffectState* effect = &gEffectStates[data->effectBlockIndex];
    volatile FFP_MIDI_Effect_Basic* midi_data = (FFP_MIDI_Effect_Basic*)effect->data;
    
    // Calculate frequency (Hz)
    uint16_t frequency = 0x0001; // 1Hz default
    if (data->period <= 13) {
        frequency = 77; // Max safe frequency
    } else if (data->period < 1000) {
        frequency = 1000 / data->period;
    }
    
    // Set frequency
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->frequency = UsbUint16ToMidiUint14(frequency);
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_FREQUENCY, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = frequency & 0x7F;
        cmd[2] = (frequency >> 7) & 0x7F;
        FFP_SendData(cmd, 3);
    }
    
    // Set magnitude
    uint8_t midi_magnitude = (data->magnitude >> 1) & 0x7F;
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->magnitude = midi_magnitude;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_MAGNITUDE, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = midi_magnitude;
        cmd[2] = 0;
        FFP_SendData(cmd, 3);
    }
}

void FFP_HandleConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data)
{
    volatile TEffectState* effect = &gEffectStates[data->effectBlockIndex];
    volatile FFP_MIDI_Effect_Basic* midi_data = (FFP_MIDI_Effect_Basic*)effect->data;
    
    uint8_t midi_magnitude;
    if (data->magnitude >= 0) {
        midi_magnitude = (data->magnitude >> 1) & 0x7F;
    } else {
        midi_magnitude = ((-data->magnitude - 1) >> 1) & 0x7F;
    }
    
    // Set magnitude
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->magnitude = midi_magnitude;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_MAGNITUDE, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = midi_magnitude;
        cmd[2] = 0;
        FFP_SendData(cmd, 3);
    }
    
    // Set direction based on sign
    uint16_t direction = (data->magnitude < 0) ? 180 : 0;
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->direction = UsbUint16ToMidiUint14(direction);
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_DIRECTION, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = direction & 0x7F;
        cmd[2] = (direction >> 7) & 0x7F;
        FFP_SendData(cmd, 3);
    }
}

void FFP_HandleEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data)
{
    volatile TEffectState* effect = &gEffectStates[data->effectBlockIndex];
    volatile FFP_MIDI_Effect_Basic* midi_data = (FFP_MIDI_Effect_Basic*)effect->data;
    
    // Set attack level
    uint8_t attack_level = (data->attackLevel >> 1) & 0x7F;
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->attackLevel = attack_level;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_ATTACK, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = attack_level;
        cmd[2] = 0;
        FFP_SendData(cmd, 3);
    }
    
    // Set fade level
    uint8_t fade_level = (data->fadeLevel >> 1) & 0x7F;
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->fadeLevel = fade_level;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_FADE, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = fade_level;
        cmd[2] = 0;
        FFP_SendData(cmd, 3);
    }
    
    // Set attack time
    uint16_t attack_time = UsbUint16ToMidiUint14_Time(data->attackTime);
    if (!(effect->state & MEffectState_SentToJoystick)) {
        midi_data->attackTime = attack_time;
    } else {
        uint8_t cmd[3] = {0xB5, FFP_MIDI_MODIFY_ATTACKTIME, data->effectBlockIndex};
        FFP_SendData(cmd, 3);
        cmd[0] = 0xA5;
        cmd[1] = attack_time & 0x7F;
        cmd[2] = (attack_time >> 7) & 0x7F;
        FFP_SendData(cmd, 3);
    }
}

void FFP_HandleEffectOperation(USB_FFBReport_EffectOperation_Output_Data_t* data)
{
    uint8_t eid = data->effectBlockIndex;
    
    if (eid == 0xFF)
        eid = 0x7F;  // All effects
        
    switch(data->operation) {
        case 1:  // Start
            FFP_StartEffect(eid);
            break;
            
        case 2:  // StartSolo
            StopAllEffects();
            FFP_StartEffect(eid);
            break;
            
        case 3:  // Stop
            FFP_StopEffect(eid);
            break;
    }
}

void FFP_HandleBlockFree(USB_FFBReport_BlockFree_Output_Data_t* data)
{
    if (data->effectBlockIndex == 0xFF) {
        FreeAllEffects();
    } else {
        FFP_FreeEffect(data->effectBlockIndex);
    }
}

void FFP_HandleDeviceControl(USB_FFBReport_DeviceControl_Output_Data_t* data)
{
    FFP_DeviceControl(data->control);
}

void FFP_HandleDeviceGain(USB_FFBReport_DeviceGain_Output_Data_t* data)
{
    FFP_SetGain(data->gain);
}
