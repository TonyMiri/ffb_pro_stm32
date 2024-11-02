# WeAct STM32F411 Button Pin Issue Documentation

## Problem Statement
The current Sidewinder Force Feedback Pro implementation uses pin PB2 for Button 3, but this presents a critical issue on the WeAct STM32F411 board ("Black Pill"):

1. PB2 is used by BOOT1 on the WeAct board and "cannot be used, without causing problems" (from board documentation)
2. Button 3 from the Sidewinder's DB15 connector is currently mapped to this problematic pin
3. This could interfere with bootloader functionality and cause reliability issues

## Current Configuration
Current button mapping in the firmware:
- Button 1: PB0 → DB15 pin 2
- Button 2: PB1 → DB15 pin 7
- Button 3: PB2 → DB15 pin 10  (problematic)
- Button 4: PB3 → DB15 pin 14

## Proposed Solution
Move Button 3 from PB2 to PB7:
- Keep all other button mappings the same
- PB7 is available and 5V tolerant
- Uses EXTI9_5 interrupt instead of EXTI2

## Required Code Changes

### 1. In main.h
Change these lines:
```c
#define BTN3_Pin GPIO_PIN_2
#define BTN3_GPIO_Port GPIOB
#define BTN3_EXTI_IRQn EXTI2_IRQn
```
to:
```c
#define BTN3_Pin GPIO_PIN_7
#define BTN3_GPIO_Port GPIOB
#define BTN3_EXTI_IRQn EXTI9_5_IRQn
```

### 2. In stm32f4xx_it.c
Replace:
```c
void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BTN3_Pin);
}
```
with:
```c
void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BTN3_Pin);
}
```

### 3. Physical Changes Required
- Move the wire from DB15 pin 10 (Button 3) connection from STM32 PB2 to PB7
- All other physical connections remain the same

## Implementation Notes
1. No changes to gpio.c are required as it uses the pin definitions from main.h
2. No regeneration of project files from STM32CubeMX is needed
3. Keep the same input protection resistors as in the original design

## Verification Steps
After making these changes:
1. Verify that Button 3 works correctly with the new pin
2. Check that bootloader functionality is not affected
3. Verify that other buttons continue to work as expected

## References
- WeAct STM32F411 documentation warning about PB2
- Original Sidewinder FFB Pro pinout documentation
- Current project pin mapping configuration
