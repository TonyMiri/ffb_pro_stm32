
/**
 ******************************************************************************
 * @file           : usbd_desc.c
 * @version        : v1.0_Cube
 * @brief          : This file implements the USB device descriptors.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
 * @{
 */

/** @addtogroup USBD_DESC
 * @{
 */

/** @defgroup USBD_DESC_Private_TypesDefinitions USBD_DESC_Private_TypesDefinitions
 * @brief Private types.
 * @{
 */

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_Defines USBD_DESC_Private_Defines
 * @brief Private defines.
 * @{
 */

// Use a VID/PID combination that won't conflict with commercial devices
#define USBD_VID                    0x0483  // STMicroelectronics VID 
#define USBD_PID_FS                0x5740  // Unique PID for this project
#define USBD_LANGID_STRING         0x0409  // English (US)
#define USBD_MANUFACTURER_STRING   "DIY FFB Project"  
#define USBD_PRODUCT_STRING_FS     "GamePort FFB Joystick"
#define USBD_CONFIGURATION_STRING_FS "HID Config"
#define USBD_INTERFACE_STRING_FS "HID Interface"

#define USB_SIZ_BOS_DESC 0x0C

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_Macros USBD_DESC_Private_Macros
 * @brief Private macros.
 * @{
 */

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_FunctionPrototypes USBD_DESC_Private_FunctionPrototypes
 * @brief Private functions declaration.
 * @{
 */

static void Get_SerialNum(void);
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len);

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_FunctionPrototypes USBD_DESC_Private_FunctionPrototypes
 * @brief Private functions declaration for FS.
 * @{
 */

uint8_t *USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#if (USBD_LPM_ENABLED == 1)
uint8_t *USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#endif

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_Variables USBD_DESC_Private_Variables
 * @brief Private variables.
 * @{
 */

USBD_DescriptorsTypeDef FS_Desc =
    {
        USBD_FS_DeviceDescriptor, USBD_FS_LangIDStrDescriptor, USBD_FS_ManufacturerStrDescriptor, USBD_FS_ProductStrDescriptor, USBD_FS_SerialStrDescriptor, USBD_FS_ConfigStrDescriptor, USBD_FS_InterfaceStrDescriptor
#if (USBD_LPM_ENABLED == 1)
        ,
        USBD_FS_USR_BOSDescriptor
#endif
};

#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif
/** USB standard device descriptor. */
__ALIGN_BEGIN uint8_t USBD_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
    {
        0x12,
        USB_DESC_TYPE_DEVICE,
#if (USBD_LPM_ENABLED == 1)
        0x01,
#else
        0x00,
#endif
        0x02,
        0x00,
        0x00,
        0x00,
        USB_MAX_EP0_SIZE,
        LOBYTE(USBD_VID),
        HIBYTE(USBD_VID),
        LOBYTE(USBD_PID_FS),
        HIBYTE(USBD_PID_FS),
        0x00,
        0x02,
        USBD_IDX_MFC_STR,
        USBD_IDX_PRODUCT_STR,
        USBD_IDX_SERIAL_STR,
        USBD_MAX_NUM_CONFIGURATION};

/** BOS descriptor. */
#if (USBD_LPM_ENABLED == 1)
#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN uint8_t USBD_FS_BOSDesc[USB_SIZ_BOS_DESC] __ALIGN_END =
    {
        0x5,
        USB_DESC_TYPE_BOS,
        0xC,
        0x0,
        0x1,

        0x7,
        USB_DEVICE_CAPABITY_TYPE,
        0x2,
        0x2,
        0x0,
        0x0,
        0x0};
#endif

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_Variables USBD_DESC_Private_Variables
 * @brief Private variables.
 * @{
 */

#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif

/** USB lang identifier descriptor. */
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
    {
        USB_LEN_LANGID_STR_DESC,
        USB_DESC_TYPE_STRING,
        LOBYTE(USBD_LANGID_STRING),
        HIBYTE(USBD_LANGID_STRING)};

#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif

__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
    USB_SIZ_STRING_SERIAL,
    USB_DESC_TYPE_STRING,
};

/**
 * @}
 */

/** @defgroup USBD_DESC_Private_Functions USBD_DESC_Private_Functions
 * @brief Private functions.
 * @{
 */

/**
 * @brief  Return the device descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_FS_DeviceDesc);
  return USBD_FS_DeviceDesc;
}

/**
 * @brief  Return the LangID string descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_LangIDDesc);
  return USBD_LangIDDesc;
}

/**
 * @brief  Return the product string descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if (speed == 0)
  {
    USBD_GetString((uint8_t *)USBD_PRODUCT_STRING_FS, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_PRODUCT_STRING_FS, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

/**
 * @brief  Return the manufacturer string descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/**
 * @brief  Return the serial number string descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = USB_SIZ_STRING_SERIAL;

  Get_SerialNum();

  return (uint8_t *)USBD_StringSerial;
}

/**
 * @brief  Return the configuration string descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if (speed == USBD_SPEED_HIGH)
  {
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING_FS, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING_FS, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

/**
 * @brief  Return the interface string descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if (speed == 0)
  {
    USBD_GetString((uint8_t *)USBD_INTERFACE_STRING_FS, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_INTERFACE_STRING_FS, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

#if (USBD_LPM_ENABLED == 1)
/**
 * @brief  Return the BOS descriptor
 * @param  speed : Current device speed
 * @param  length : Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t *USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_FS_BOSDesc);
  return (uint8_t *)USBD_FS_BOSDesc;
}
#endif

/**
 * @brief  Create the serial number string descriptor
 * @param  None
 * @retval None
 */
static void Get_SerialNum(void)
{
  uint32_t deviceserial0;
  uint32_t deviceserial1;
  uint32_t deviceserial2;

  deviceserial0 = *(uint32_t *)DEVICE_ID1;
  deviceserial1 = *(uint32_t *)DEVICE_ID2;
  deviceserial2 = *(uint32_t *)DEVICE_ID3;

  deviceserial0 += deviceserial2;

  if (deviceserial0 != 0)
  {
    IntToUnicode(deviceserial0, &USBD_StringSerial[2], 8);
    IntToUnicode(deviceserial1, &USBD_StringSerial[18], 4);
  }
}

/**
 * @brief  Convert Hex 32Bits value into char
 * @param  value: value to convert
 * @param  pbuf: pointer to the buffer
 * @param  len: buffer length
 * @retval None
 */
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
  uint8_t idx = 0;

  for (idx = 0; idx < len; idx++)
  {
    if (((value >> 28)) < 0xA)
    {
      pbuf[2 * idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[2 * idx + 1] = 0;
  }
}
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
