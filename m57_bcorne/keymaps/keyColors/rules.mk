RGB_MATRIX_ENABLE = yes # required
SRC += key_colors_hid.c

RAW_ENABLE = yes
VIA_ENABLE = yes
VIAL_ENABLE = yes
VIALRGB_ENABLE = yes
VIAL_INSECURE = yes

# Brightness-only VIA value (id_qmk_rgb_matrix_brightness) alongside VialRGB --
# sets rgb_matrix's hsv.v directly without touching mode/enable, unlike
# VialRGB's own SET_MODE (which can't represent our key_colors community-
# module effect and collapses to "Off").
OPT_DEFS += -DVIA_QMK_RGB_MATRIX_ENABLE

ENCODER_MAP_ENABLE = yes

# Debug
CONSOLE_ENABLE = no
