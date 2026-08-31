# BCORNE (M57)

Vial-QMK firmware for the "M57" split keyboard (Corne-style, STM32F401, TinyUF2 bootloader) — four keymaps, pick one, build it, flash it.

Don't want to build it yourself? Prebuilt `.uf2` files are in [`firmware/`](firmware/) — skip to [Flashing](docs/flashing.md).

## Which keymap?

| Keymap | What it is |
|---|---|
| `default` | Stock QMK, no VIA/Vial, stock RGB Matrix effects |
| `vial` | Vial-enabled version of `default` |
| `keyColors` | Pick your own key colors via [browser WebGUI](docs/webgui-usage.md) |
| `comboRGB` | Same, plus [audio visualizer](docs/audio-visualizer.md) |

## Quick start

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk
git submodule update --init --recursive --depth 1
python3 -m pip install -r requirements.txt

git clone --recursive https://github.com/kolbenhans/BCORNE.git ~/projects/BCORNE
ln -s ~/projects/BCORNE/ymdk/m57_bcorne ~/projects/vial-qmk/keyboards/m57_bcorne
cp ~/projects/BCORNE/ymdk/ld/*.ld ~/projects/vial-qmk/platforms/chibios/boards/common/ld/

cd ~/projects/vial-qmk
qmk compile -kb m57_bcorne -km keyColors   # or: default, vial, comboRGB
```

Copy `.build/m57_bcorne_<keymap>.uf2` onto the keyboard's UF2 drive — **both halves separately**. Details: [Build Guide](docs/build.md), [Flashing Guide](docs/flashing.md).

`keyColors`/`comboRGB` need modules — see [kolbenhans/qmk-modules](https://github.com/kolbenhans/qmk-modules).

## Contents

- `m57_bcorne/` — keyboard source
- `ld/` — custom linker script required by the vendor bootloader

## Docs

[Build](docs/build.md) · [Flashing](docs/flashing.md) · [Audio Visualizer](docs/audio-visualizer.md) · [WebGUI Usage](docs/webgui-usage.md)
