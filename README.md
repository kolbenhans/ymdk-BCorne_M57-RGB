# BCORNE

Blank Vial QMK firmware source for the "M57" split keyboard (Corne-style, STM32F401, TinyUF2 bootloader).

Stock VIA/Vial keymap with the full standard RGB Matrix effect set. No custom RGB effects, no host-side lighting tools — just the keyboard source as dropped into a `vial-qmk` tree.

## Contents

- `m57/` — keyboard source (drop into `vial-qmk/keyboards/m57`)
- `ld/` — custom linker scripts required by the vendor bootloader
- `LICENSE`

## Build

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk && git submodule update --init --recursive --depth 1

ln -s ~/projects/BCORNE/m57 ~/projects/vial-qmk/keyboards/m57
cp ~/projects/BCORNE/ld/*.ld ~/projects/vial-qmk/platforms/chibios/boards/common/ld/

qmk compile -kb m57 -km via
```

Output: `.build/m57_via.uf2`

## Flashing

Split keyboard — flash **both halves individually**. `QK_BOOT` is present on layer 3 (top-left/top-right keys).
