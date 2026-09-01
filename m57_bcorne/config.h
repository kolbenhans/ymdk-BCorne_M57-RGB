#pragma once

// -----------------------------------------------------------------------------
// MCU / clock
// -----------------------------------------------------------------------------

#undef STM32_HSECLK
#define STM32_HSECLK 16000000

// -----------------------------------------------------------------------------
// EEPROM flash placement
// -----------------------------------------------------------------------------

#define WEAR_LEVELING_LEGACY_EMULATION_BASE_PAGE_ADDRESS 0x0800C000

// -----------------------------------------------------------------------------
// Vial / dynamic keymap
// -----------------------------------------------------------------------------

#define DYNAMIC_KEYMAP_LAYER_COUNT 10
#define DYNAMIC_KEYMAP_MACRO_COUNT 15

#define WEAR_LEVELING_LOGICAL_SIZE 4096
#define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2)
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 4095

// -----------------------------------------------------------------------------
// Encoder
// -----------------------------------------------------------------------------

#define ENCODER_MAP_KEY_DELAY 10

// -----------------------------------------------------------------------------
// Split transport
// -----------------------------------------------------------------------------

#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN A9
#define SERIAL_USART_RX_PIN A10

// 0: 460800 baud
// 1: 230400 baud (default)
// 2: 115200 baud
// 3: 57600 baud
// 4: 38400 baud
// 5: 19200 baud
#define SELECT_SOFT_SERIAL_SPEED 1

#define SERIAL_USART_DRIVER SD1
#define SERIAL_USART_TX_PAL_MODE 7
#define SERIAL_USART_RX_PAL_MODE 7
#define SERIAL_USART_TIMEOUT 20

#define SPLIT_HAND_PIN_LOW_IS_LEFT

// -----------------------------------------------------------------------------
// RGB matrix / WS2812
// -----------------------------------------------------------------------------

#define WS2812_PWM_DRIVER PWMD3
#define WS2812_PWM_CHANNEL 2
#define WS2812_PWM_PAL_MODE 2
#define WS2812_DMA_STREAM STM32_DMA1_STREAM2
#define WS2812_DMA_CHANNEL 5