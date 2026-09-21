# QMK core patches

These aren't applied automatically — apply after cloning/updating the QMK
core (`vial-qmk`/`qmk_firmware`) this board builds against:

```sh
cd /path/to/vial-qmk
git apply /path/to/BCORNE/patches/chibios-usb-wait-timeout.patch
```

- **chibios-usb-wait-timeout.patch**: makes `USB_WAIT_FOR_ENUMERATION`
  respect an optional `USB_WAIT_FOR_ENUMERATION_TIMEOUT_MS` (used by
  `m57_bcorne/config.h`). Without this patch, `USB_WAIT_FOR_ENUMERATION`
  waits forever for `USB_ACTIVE` — fine for a non-split board, but it hangs
  a split board's peripheral half forever, since only the host-connected
  half ever reaches `USB_ACTIVE`.
