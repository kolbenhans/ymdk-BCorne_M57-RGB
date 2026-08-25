# BCORNE (M57)

Vial QMK firmware source for the "M57" split keyboard (Corne-style, STM32F401, TinyUF2 bootloader).

## Contents

- `m57_bcorne/` — keyboard source (drop into `vial-qmk/keyboards/ymdk/m57_bcorne`)
- `ld/` — custom linker script required by the vendor bootloader
- `firmware/` — pre-compiled `.uf2` files, one per keymap, for flashing without a build setup
- `LICENSE`

### Keymaps

- `default` — stock VIA keymap, full standard RGB Matrix effect set, no custom lighting.
- `vial` — Vial-enabled version of the above.
- `keyColors` — per-key RGB assignment over Raw HID (see the [webgui](../webgui) in the sibling `ymdk-id75v3rp-RGB` repo — same protocol as `sofle_panda`/`id75v3rp`). Pulls in the [`srwi/keypeek_layer_notify`](https://github.com/srwi/qmk-modules) community module to notify the [keypeek](https://github.com/srwi/keypeek) desktop overlay of layer changes.

No `comboRGB` keymap here — that build (keyColors + the Python viz/audio-visualizer interface) only exists for `sofle_panda` and `id75v3rp`, was never built for M57.

## Build

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk && git submodule update --init --recursive --depth 1

ln -s ~/projects/BCORNE/ymdk/m57_bcorne ~/projects/vial-qmk/keyboards/ymdk/m57_bcorne
cp ~/projects/BCORNE/ymdk/ld/*.ld ~/projects/vial-qmk/platforms/chibios/boards/common/ld/

# only needed for the keyColors keymap:
git clone --depth 1 https://github.com/srwi/qmk-modules ~/projects/qmk-modules
mkdir -p ~/projects/vial-qmk/modules/srwi
cp -r ~/projects/qmk-modules/srwi/keypeek_layer_notify ~/projects/vial-qmk/modules/srwi/keypeek_layer_notify

qmk compile -kb ymdk/m57_bcorne -km vial       # or: default, keyColors
```

Output: `.build/ymdk_m57_bcorne_<keymap>.uf2`

Prebuilt `.uf2` files for all three keymaps are already in `firmware/` if you just want to flash without setting up a build environment.

## Flashing

Split keyboard — flash **both halves individually**. `QK_BOOT` is present on layer 3 (top-left/top-right keys).
