# Build Guide

**Windows:** [QMK MSYS](https://msys.qmk.fm/).
**Linux/macOS:** `pip install qmk`, [QMK Getting Started](https://docs.qmk.fm/newbs_getting_started).

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk
git submodule update --init --recursive --depth 1
python3 -m pip install -r requirements.txt

git clone --recursive https://github.com/kolbenhans/ymdk-BCorne_M57-RGB.git ~/projects/BCORNE
ln -s ~/projects/BCORNE/m57_bcorne ~/projects/vial-qmk/keyboards/m57_bcorne
cp ~/projects/BCORNE/ld/*.ld ~/projects/vial-qmk/platforms/chibios/boards/common/ld/
```

Windows without QMK MSYS: `mklink /D` instead of `ln -s` (admin).

## Community Modules

`keyColors`/`comboRGB` need modules from
[kolbenhans/qmk-modules](https://github.com/kolbenhans/qmk-modules) and
[srwi/qmk-modules](https://github.com/srwi/qmk-modules) — install steps
there. `default`/`vial` need none.

## Build

```bash
cd ~/projects/vial-qmk
qmk compile -kb m57_bcorne -km <default|vial|keyColors|comboRGB>
```

Output: `.build/m57_bcorne_<keymap>.uf2`. Already in bootloader mode? `qmk
flash` instead. Split — flash **both halves**, see [Flashing Guide](flashing.md).
