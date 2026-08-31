# Flashing Guide

## Linux: USB permissions

See [QMK's udev rules guide](https://docs.qmk.fm/faq_build#linux-udev-rules).

## Build & flash

```bash
qmk compile -kb m57_bcorne -km <keymap>
```
Copy `.build/m57_bcorne_<keymap>.uf2` onto the keyboard's UF2 drive. **Both halves, individually.**

## Entering bootloader mode

- Double-press the reset button on the PCB, or
- Press a key bound to `QK_BOOT` (layer 3, top-left/top-right — assignable in Vial)

Keyboard appears as a USB drive — drop the `.uf2` file on it.
