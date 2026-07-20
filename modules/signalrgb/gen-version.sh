#!/bin/sh
rm -f qmk_version.h
cd ../../
git describe --abbrev=0 --tags
echo "#define QMK_VERSION_BYTE_1 `(git describe --tags | cut -d. -f1)`" >> modules/signalrgb/qmk_version.h
echo "#define QMK_VERSION_BYTE_2 `(git describe --tags | cut -d. -f2)`" >> modules/signalrgb/qmk_version.h
echo "#define QMK_VERSION_BYTE_3 `(git describe --tags | cut -d. -f3 | cut -d- -f1)`" >> modules/signalrgb/qmk_version.h
