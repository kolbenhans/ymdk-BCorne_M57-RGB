# BCORNE

Blank Vial QMK firmware source for the "M57" split keyboard (Corne-style, STM32F401, TinyUF2 bootloader).

Stock VIA/Vial keymap with the full standard RGB Matrix effect set. No custom RGB effects, no host-side lighting tools — just the keyboard source as dropped into a `vial-qmk` tree.

## Contents

- `m57/` — keyboard source (drop into `vial-qmk/keyboards/m57`)
- `ld/` — custom linker scripts required by the vendor bootloader
- `modules/signalrgb/` — [SignalRGB](https://signalrgb.com/) community module (see below)
- `LICENSE`

## Build

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk && git submodule update --init --recursive --depth 1

ln -s ~/projects/BCORNE/m57 ~/projects/vial-qmk/keyboards/m57
cp ~/projects/BCORNE/ld/*.ld ~/projects/vial-qmk/platforms/chibios/boards/common/ld/

mkdir -p ~/projects/vial-qmk/modules
cp -r ~/projects/BCORNE/modules/signalrgb ~/projects/vial-qmk/modules/signalrgb

qmk compile -kb m57 -km via
```

Output: `.build/m57_via.uf2`

## Flashing

Split keyboard — flash **both halves individually**. `QK_BOOT` is present on layer 3 (top-left/top-right keys).

## SignalRGB

Vendored copy of [SRGBmods/QMK_Community_Module](https://github.com/SRGBmods/QMK_Community_Module) (early WIP upstream), minus its `config.h` — that file `#undef`s most stock `ENABLE_RGB_MATRIX_*` effects to save flash, which conflicts with this repo's "keep the full stock effect set" goal. Enabled via `keymaps/via/keymap.json` (`"modules": ["signalrgb"]`).

The module streams arbitrary per-LED colors over raw HID to the master half only. Since stock `SPLIT_RGB_MATRIX` only syncs mode/hsv/speed (not raw pixel data), `keymaps/via/signalrgb_split.c` mirrors the remote half's colors to the slave over its own RPC transaction (`USER_SYNC_SIGNALRGB`) — same pattern the original dynamic-lights firmware used for its direct-color modes.

Module dir must be a **real copy**, not a symlink — QMK's module discovery (`lib/python/qmk/community_modules.py`) uses `Path.rglob()`, which does not traverse symlinked directories on Python 3.13+ (confirmed against a real build: `qmk compile` silently reports `Module 'signalrgb' not found` with a symlink, works fine with a copy). Re-copy after pulling changes from this repo.

`modules/signalrgb/qmk_version.h` has placeholder version bytes. From `~/projects/vial-qmk/modules/signalrgb/`, run `./gen-version.sh` to stamp the real version before relying on `GET_QMK_VERSION`.
