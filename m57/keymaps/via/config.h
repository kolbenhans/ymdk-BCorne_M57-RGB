#pragma once

#define VIAL_KEYBOARD_UID {0x89, 0x36, 0x2A, 0xC7, 0xFA, 0xD8, 0x89, 0x45}

// Split sync buffer large enough for 29 LEDs × 3 bytes RGB (87 bytes)
#define RPC_M2S_BUFFER_SIZE 96

// Sync RGB matrix mode/speed/HSV from master to slave
#define SPLIT_RGB_MATRIX_ENABLE
