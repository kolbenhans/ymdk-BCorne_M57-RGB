# BORNE Knobs Corne4×6 (M57)

Custom **Vial-QMK firmware for the BORNE Knobs Corne4×6 split keyboard** (internal model name `M57`; Corne-style, STM32F401, TinyUF2 bootloader).

Four firmware variants are available:

| Keymap      | Description                                                                 |
| ----------- | --------------------------------------------------------------------------- |
| `default`   | Stock QMK with the standard RGB Matrix effects                              |
| `vial`      | Vial-enabled version of `default`                                           |
| `keyColors` | Custom per-key colors via the [browser-based WebGUI](docs/webgui-usage.md)  |
| `comboRGB`  | `keyColors` with an integrated [audio visualizer](docs/audio-visualizer.md) |

Don't want to build the firmware yourself? Prebuilt `.uf2` files are available in [`firmware/`](firmware/).

---

## Quick Start

The easiest way to build the firmware is to use [Vial-QMK](https://github.com/vial-kb/vial-qmk).

### 1. Set up Vial-QMK

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk

git submodule update --init --recursive --depth 1
python3 -m pip install -r requirements.txt
```

### 2. Clone this repository

```bash
git clone --recursive \
    https://github.com/kolbenhans/ymdk-BCorne_M57-RGB.git \
    ~/projects/BCORNE
```

### 3. Link the keyboard source

```bash
ln -s \
    ~/projects/BCORNE/m57_bcorne \
    ~/projects/vial-qmk/keyboards/m57_bcorne
```

### 4. Install the linker script

The M57 uses a vendor bootloader that requires a custom linker script.

```bash
cp \
    ~/projects/BCORNE/ld/*.ld \
    ~/projects/vial-qmk/platforms/chibios/boards/common/ld/
```

### 5. Build

```bash
cd ~/projects/vial-qmk

qmk compile -kb m57_bcorne -km keyColors
```

Available keymaps:

```text
default
vial
keyColors
comboRGB
```

For example:

```bash
qmk compile -kb m57_bcorne -km comboRGB
```

The resulting firmware will be located at:

```text
.build/m57_bcorne_<keymap>.uf2
```

---

## Flashing

Copy the generated `.uf2` file to the keyboard's **UF2 drive**.

**Both halves must be flashed separately.**

See the [Build Guide](docs/build.md) and [Flashing Guide](docs/flashing.md) for detailed instructions.

---

## QMK Modules

The `keyColors` and `comboRGB` keymaps use additional QMK modules.

See [`kolbenhans/qmk-modules`](https://github.com/kolbenhans/qmk-modules) for the required modules and setup instructions.

The prebuilt firmware does not require any additional module setup.

---

## Contents

* `m57_bcorne/` — keyboard source
* `ld/` — custom linker script required by the vendor bootloader
* `firmware/` — prebuilt UF2 firmware

---

## Documentation

* [Build Guide](docs/build.md)
* [Flashing Guide](docs/flashing.md)
* [Audio Visualizer](docs/audio-visualizer.md)
* [WebGUI Usage](docs/webgui-usage.md)

---

## Hardware

* **Keyboard:** BORNE Knobs Corne4×6
* **Model:** M57
* **Controller:** STM32F401
* **Bootloader:** TinyUF2

This firmware is specifically intended for the M57 hardware. Firmware for other Corne or M57 variants may not be compatible.

---

## License

See [`LICENSE`](LICENSE) for license information.
