/**
 ******************************************************************************
 * @file    system_stm32f4xx.c
 * @author  MCD Application Team
 * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
 *
 *   This file provides two functions and one global variable to be called from
 *   user application:
 *      - SystemInit(): This function is called at startup just after reset and
 *                      before branch to main program. This call is made inside
 *                      the "startup_stm32f4xx.s" file.
 *
 *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
 *                                  by the user application to setup the SysTick
 *                                  timer or configure other parameters.
 *
 *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
 *                                 be called whenever the core clock is changed
 *                                 during program execution.
 *
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/** @addtogroup CMSIS
 * @{
 */

/** @addtogroup stm32f4xx_system
 * @{
 */

/** @addtogroup STM32F4xx_System_Private_Includes
 * @{
 */

#include "stm32f4xx.h"

#if !defined(HSE_VALUE)
#define HSE_VALUE ((uint32_t)25000000)
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE ((uint32_t)16000000)
#endif

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_Defines
 * @{
 */

/************************* Miscellaneous Configuration ************************/

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx)

#endif

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)

#endif

#if defined(USER_VECT_TAB_ADDRESS)

#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS SRAM_BASE
#define VECT_TAB_OFFSET 0x00000000U
#else
#define VECT_TAB_BASE_ADDRESS FLASH_BASE
#define VECT_TAB_OFFSET 0x00000000U
#endif
#endif
/******************************************************************************/

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_Macros
 * @{
 */

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_Variables
 * @{
 */

uint32_t SystemCoreClock = 16000000;
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};
/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_FunctionPrototypes
 * @{
 */

#if defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
static void SystemInit_ExtMemCtl(void);
#endif

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_Functions
 * @{
 */

/**
 * @brief  Setup the microcontroller system
 *         Initialize the FPU setting, vector table location and External memory
 *         configuration.
 * @param  None
 * @retval None
 */
void SystemInit(void)
{

#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
#endif

#if defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
  SystemInit_ExtMemCtl();
#endif

#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 *
 * @note   Each time the core clock (HCLK) changes, this function must be called
 *         to update SystemCoreClock variable value. Otherwise, any configuration
 *         based on this variable will be incorrect.
 *
 * @note   - The system frequency computed by this function is not the real
 *           frequency in the chip. It is calculated based on the predefined
 *           constant and the selected clock source:
 *
 *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *
 *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *
 *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
 *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
 *
 *         (*) HSI_VALUE is a constant defined in stm32f4xx_hal_conf.h file (default value
 *             16 MHz) but the real value may vary depending on the variations
 *             in voltage and temperature.
 *
 *         (**) HSE_VALUE is a constant defined in stm32f4xx_hal_conf.h file (its value
 *              depends on the application requirements), user has to ensure that HSE_VALUE
 *              is same as the real frequency of the crystal used. Otherwise, this function
 *              may have wrong result.
 *
 *         - The result of this function could be not correct when using fractional
 *           value for HSE crystal.
 *
 * @param  None
 * @retval None
 */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
  case 0x00:
    SystemCoreClock = HSI_VALUE;
    break;
  case 0x04:
    SystemCoreClock = HSE_VALUE;
    break;
  case 0x08:

    pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
    pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

    if (pllsource != 0)
    {

      pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
    }
    else
    {

      pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
    }

    pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> 16) + 1) * 2;
    SystemCoreClock = pllvco / pllp;
    break;
  default:
    SystemCoreClock = HSI_VALUE;
    break;
  }

  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];

  SystemCoreClock >>= tmp;
}

#if defined(DATA_IN_ExtSRAM) && defined(DATA_IN_ExtSDRAM)
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F469xx) || defined(STM32F479xx)
/**
 * @brief  Setup the external memory controller.
 *         Called in startup_stm32f4xx.s before jump to main.
 *         This function configures the external memories (SRAM/SDRAM)
 *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
 * @param  None
 * @retval None
 */
void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmp = 0x00;

  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t index;

  RCC->AHB1ENR |= 0x000001F8;

  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

  GPIOD->AFR[0] = 0x00CCC0CC;
  GPIOD->AFR[1] = 0xCCCCCCCC;

  GPIOD->MODER = 0xAAAA0A8A;

  GPIOD->OSPEEDR = 0xFFFF0FCF;

  GPIOD->OTYPER = 0x00000000;

  GPIOD->PUPDR = 0x00000000;

  GPIOE->AFR[0] = 0xC00CC0CC;
  GPIOE->AFR[1] = 0xCCCCCCCC;

  GPIOE->MODER = 0xAAAA828A;

  GPIOE->OSPEEDR = 0xFFFFC3CF;

  GPIOE->OTYPER = 0x00000000;

  GPIOE->PUPDR = 0x00000000;

  GPIOF->AFR[0] = 0xCCCCCCCC;
  GPIOF->AFR[1] = 0xCCCCCCCC;

  GPIOF->MODER = 0xAA800AAA;

  GPIOF->OSPEEDR = 0xAA800AAA;

  GPIOF->OTYPER = 0x00000000;

  GPIOF->PUPDR = 0x00000000;

  GPIOG->AFR[0] = 0xCCCCCCCC;
  GPIOG->AFR[1] = 0xCCCCCCCC;

  GPIOG->MODER = 0xAAAAAAAA;

  GPIOG->OSPEEDR = 0xAAAAAAAA;

  GPIOG->OTYPER = 0x00000000;

  GPIOG->PUPDR = 0x00000000;

  GPIOH->AFR[0] = 0x00C0CC00;
  GPIOH->AFR[1] = 0xCCCCCCCC;

  GPIOH->MODER = 0xAAAA08A0;

  GPIOH->OSPEEDR = 0xAAAA08A0;

  GPIOH->OTYPER = 0x00000000;

  GPIOH->PUPDR = 0x00000000;

  GPIOI->AFR[0] = 0xCCCCCCCC;
  GPIOI->AFR[1] = 0x00000CC0;

  GPIOI->MODER = 0x0028AAAA;

  GPIOI->OSPEEDR = 0x0028AAAA;

  GPIOI->OTYPER = 0x00000000;

  GPIOI->PUPDR = 0x00000000;

  RCC->AHB3ENR |= 0x00000001;

  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  FMC_Bank5_6->SDCR[0] = 0x000019E4;
  FMC_Bank5_6->SDTR[0] = 0x01115351;

  FMC_Bank5_6->SDCMR = 0x00000011;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  for (index = 0; index < 1000; index++)
    ;

  FMC_Bank5_6->SDCMR = 0x00000012;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  FMC_Bank5_6->SDCMR = 0x00000073;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  FMC_Bank5_6->SDCMR = 0x00046014;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  tmpreg = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000027C << 1));

  tmpreg = FMC_Bank5_6->SDCR[0];
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

  FMC_Bank1->BTCR[2] = 0x00001011;
  FMC_Bank1->BTCR[3] = 0x00000201;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif
#if defined(STM32F469xx) || defined(STM32F479xx)

  FMC_Bank1->BTCR[2] = 0x00001091;
  FMC_Bank1->BTCR[3] = 0x00110212;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif

  (void)(tmp);
}
#endif
#elif defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
/**
 * @brief  Setup the external memory controller.
 *         Called in startup_stm32f4xx.s before jump to main.
 *         This function configures the external memories (SRAM/SDRAM)
 *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
 * @param  None
 * @retval None
 */
void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmp = 0x00;
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
#if defined(DATA_IN_ExtSDRAM)
  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t index;

#if defined(STM32F446xx)

  RCC->AHB1ENR |= 0x0000007D;
#else

  RCC->AHB1ENR |= 0x000001F8;
#endif

  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

#if defined(STM32F446xx)

  GPIOA->AFR[0] |= 0xC0000000;
  GPIOA->AFR[1] |= 0x00000000;

  GPIOA->MODER |= 0x00008000;

  GPIOA->OSPEEDR |= 0x00008000;

  GPIOA->OTYPER |= 0x00000000;

  GPIOA->PUPDR |= 0x00000000;

  GPIOC->AFR[0] |= 0x00CC0000;
  GPIOC->AFR[1] |= 0x00000000;

  GPIOC->MODER |= 0x00000A00;

  GPIOC->OSPEEDR |= 0x00000A00;

  GPIOC->OTYPER |= 0x00000000;

  GPIOC->PUPDR |= 0x00000000;
#endif

  GPIOD->AFR[0] = 0x000000CC;
  GPIOD->AFR[1] = 0xCC000CCC;

  GPIOD->MODER = 0xA02A000A;

  GPIOD->OSPEEDR = 0xA02A000A;

  GPIOD->OTYPER = 0x00000000;

  GPIOD->PUPDR = 0x00000000;

  GPIOE->AFR[0] = 0xC00000CC;
  GPIOE->AFR[1] = 0xCCCCCCCC;

  GPIOE->MODER = 0xAAAA800A;

  GPIOE->OSPEEDR = 0xAAAA800A;

  GPIOE->OTYPER = 0x00000000;

  GPIOE->PUPDR = 0x00000000;

  GPIOF->AFR[0] = 0xCCCCCCCC;
  GPIOF->AFR[1] = 0xCCCCCCCC;

  GPIOF->MODER = 0xAA800AAA;

  GPIOF->OSPEEDR = 0xAA800AAA;

  GPIOF->OTYPER = 0x00000000;

  GPIOF->PUPDR = 0x00000000;

  GPIOG->AFR[0] = 0xCCCCCCCC;
  GPIOG->AFR[1] = 0xCCCCCCCC;

  GPIOG->MODER = 0xAAAAAAAA;

  GPIOG->OSPEEDR = 0xAAAAAAAA;

  GPIOG->OTYPER = 0x00000000;

  GPIOG->PUPDR = 0x00000000;

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F469xx) || defined(STM32F479xx)

  GPIOH->AFR[0] = 0x00C0CC00;
  GPIOH->AFR[1] = 0xCCCCCCCC;

  GPIOH->MODER = 0xAAAA08A0;

  GPIOH->OSPEEDR = 0xAAAA08A0;

  GPIOH->OTYPER = 0x00000000;

  GPIOH->PUPDR = 0x00000000;

  GPIOI->AFR[0] = 0xCCCCCCCC;
  GPIOI->AFR[1] = 0x00000CC0;

  GPIOI->MODER = 0x0028AAAA;

  GPIOI->OSPEEDR = 0x0028AAAA;

  GPIOI->OTYPER = 0x00000000;

  GPIOI->PUPDR = 0x00000000;
#endif

  RCC->AHB3ENR |= 0x00000001;

  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

#if defined(STM32F446xx)
  FMC_Bank5_6->SDCR[0] = 0x00001954;
#else
  FMC_Bank5_6->SDCR[0] = 0x000019E4;
#endif
  FMC_Bank5_6->SDTR[0] = 0x01115351;

  FMC_Bank5_6->SDCMR = 0x00000011;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  for (index = 0; index < 1000; index++)
    ;

  FMC_Bank5_6->SDCMR = 0x00000012;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

#if defined(STM32F446xx)
  FMC_Bank5_6->SDCMR = 0x000000F3;
#else
  FMC_Bank5_6->SDCMR = 0x00000073;
#endif
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

#if defined(STM32F446xx)
  FMC_Bank5_6->SDCMR = 0x00044014;
#else
  FMC_Bank5_6->SDCMR = 0x00046014;
#endif
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  tmpreg = FMC_Bank5_6->SDRTR;
#if defined(STM32F446xx)
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000050C << 1));
#else
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000027C << 1));
#endif

  tmpreg = FMC_Bank5_6->SDCR[0];
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);
#endif
#endif

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx)

#if defined(DATA_IN_ExtSRAM)

  RCC->AHB1ENR |= 0x00000078;

  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);

  GPIOD->AFR[0] = 0x00CCC0CC;
  GPIOD->AFR[1] = 0xCCCCCCCC;

  GPIOD->MODER = 0xAAAA0A8A;

  GPIOD->OSPEEDR = 0xFFFF0FCF;

  GPIOD->OTYPER = 0x00000000;

  GPIOD->PUPDR = 0x00000000;

  GPIOE->AFR[0] = 0xC00CC0CC;
  GPIOE->AFR[1] = 0xCCCCCCCC;

  GPIOE->MODER = 0xAAAA828A;

  GPIOE->OSPEEDR = 0xFFFFC3CF;

  GPIOE->OTYPER = 0x00000000;

  GPIOE->PUPDR = 0x00000000;

  GPIOF->AFR[0] = 0x00CCCCCC;
  GPIOF->AFR[1] = 0xCCCC0000;

  GPIOF->MODER = 0xAA000AAA;

  GPIOF->OSPEEDR = 0xFF000FFF;

  GPIOF->OTYPER = 0x00000000;

  GPIOF->PUPDR = 0x00000000;

  GPIOG->AFR[0] = 0x00CCCCCC;
  GPIOG->AFR[1] = 0x000000C0;

  GPIOG->MODER = 0x00085AAA;

  GPIOG->OSPEEDR = 0x000CAFFF;

  GPIOG->OTYPER = 0x00000000;

  GPIOG->PUPDR = 0x00000000;

  RCC->AHB3ENR |= 0x00000001;

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  FMC_Bank1->BTCR[2] = 0x00001011;
  FMC_Bank1->BTCR[3] = 0x00000201;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif
#if defined(STM32F469xx) || defined(STM32F479xx)

  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  FMC_Bank1->BTCR[2] = 0x00001091;
  FMC_Bank1->BTCR[3] = 0x00110212;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) || defined(STM32F412Vx)

  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FSMCEN);

  FSMC_Bank1->BTCR[2] = 0x00001011;
  FSMC_Bank1->BTCR[3] = 0x00000201;
  FSMC_Bank1E->BWTR[2] = 0x0FFFFFFF;
#endif

#endif
#endif
  (void)(tmp);
}
#endif

