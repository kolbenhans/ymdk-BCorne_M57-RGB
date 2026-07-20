#pragma once

// -----------------------------------------------------------------------------
// MCU / clock
// -----------------------------------------------------------------------------

#undef STM32_HSECLK
#define STM32_HSECLK 16000000

#define NEW_401_BL

// -----------------------------------------------------------------------------
// Vial / dynamic keymap
// -----------------------------------------------------------------------------

#define TAPPING_TOGGLE 2

#define DYNAMIC_KEYMAP_LAYER_COUNT 10
#define DYNAMIC_KEYMAP_MACRO_COUNT 15

#define WEAR_LEVELING_LOGICAL_SIZE 4096
#define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2)
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 4095

// -----------------------------------------------------------------------------
// Matrix
// Matrix size is defined in keyboard.json.
// Do not reintroduce MATRIX_ROWS or MATRIX_COLS here.
// -----------------------------------------------------------------------------
#define MATRIX_ROW_PINS { C8, C7, B2, A6, A5 }
#define MATRIX_COL_PINS { B13, B14, B15, C6, C9, A8, C12 }

#define MATRIX_ROW_PINS_RIGHT { A1, B7, C5, B0, B1 }
#define MATRIX_COL_PINS_RIGHT { B8, C10, C8, C7, C6, B15, B14 }

/*
 * Alternative half assignment kept for hardware reference.
 *
 * #define MATRIX_ROW_PINS { A1, B7, C5, B0, B1 }
 * #define MATRIX_COL_PINS { B8, C9, C8, C7, C6, B15, B14 }
 * #define MATRIX_ROW_PINS_RIGHT { C8, C7, B2, A6, A5 }
 * #define MATRIX_COL_PINS_RIGHT { B13, B14, B15, C6, C9, A8, C12 }
 */

#define DIODE_DIRECTION COL2ROW
#define DEBOUNCE 5

// Bootmagic Lite default (row0/col0) only matches the left half's matrix.
// Right half's local row0 lands at global row 5 (thisHand offset) — without
// this, holding any key on power-up can never trigger bootloader on the right.
#define BOOTMAGIC_ROW_RIGHT 5
#define BOOTMAGIC_COLUMN_RIGHT 1

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

#define MASTER_LEFT

#define SPLIT_HAND_PIN C1
#define SPLIT_HAND_PIN_LOW_IS_LEFT

// Tested 2026-06:
// No observable issues without USB detect/watchdog.
// Keeping these settings here for troubleshooting purposes.
// #define SPLIT_USB_DETECT
// #define SPLIT_USB_TIMEOUT 2000
// #define SPLIT_USB_TIMEOUT_POLL 10
// #define SPLIT_WATCHDOG_ENABLE
// #define SPLIT_WATCHDOG_TIMEOUT 3000

#define SPLIT_MODS_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
//#define SPLIT_LED_STATE_ENABLE
#define SPLIT_TRANSPORT_MIRROR

// -----------------------------------------------------------------------------
// RGB matrix / WS2812
// -----------------------------------------------------------------------------

#define WS2812_PWM_DRIVER PWMD3
#define WS2812_PWM_CHANNEL 2
#define WS2812_PWM_PAL_MODE 2
#define WS2812_DMA_STREAM STM32_DMA1_STREAM2
#define WS2812_DMA_CHANNEL 5

#define RGB_MATRIX_SLEEP
#define RGB_MATRIX_TIMEOUT 14400000   // turn off keyboard lighting after 4 hours without usage

#define ENABLE_RGB_MATRIX_ALPHAS_MODS
#define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_BAND_SAT
#define ENABLE_RGB_MATRIX_BAND_VAL
#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#define ENABLE_RGB_MATRIX_DUAL_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#define ENABLE_RGB_MATRIX_RAINDROPS
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#define ENABLE_RGB_MATRIX_HUE_BREATHING
#define ENABLE_RGB_MATRIX_HUE_PENDULUM
#define ENABLE_RGB_MATRIX_HUE_WAVE
#define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
#define ENABLE_RGB_MATRIX_PIXEL_FLOW
#define ENABLE_RGB_MATRIX_PIXEL_RAIN
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#define ENABLE_RGB_MATRIX_DIGITAL_RAIN
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#define ENABLE_RGB_MATRIX_SPLASH
#define ENABLE_RGB_MATRIX_MULTISPLASH
#define ENABLE_RGB_MATRIX_SOLID_SPLASH
#define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH

// -----------------------------------------------------------------------------
// USB
// -----------------------------------------------------------------------------

#define USB_POLLING_INTERVAL_MS 1