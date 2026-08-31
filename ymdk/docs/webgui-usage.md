# WebGUI Usage — comboRGB & keyColors

Pick key colors from a browser. **https://webgui.212-227-193-242.sslip.io/**
(Chrome/Edge/Opera — no Firefox/Safari).

Self-host instead: [kolbenhans/qmk-webgui](https://github.com/kolbenhans/qmk-webgui).

1. Flash `comboRGB` or `keyColors`.
2. Connect, pick keyboard, pick layer, click keys, pick colors.
3. Save.
4. Export before reflashing — Import to restore.

Keys not clickable / layout looks wrong? Click **Load layout…**, load a
`qmk info -f json` export — see
[kolbenhans/qmk-modules docs](https://github.com/kolbenhans/qmk-modules/blob/master/docs/vial-json-rgb-matrix-layout.md).
