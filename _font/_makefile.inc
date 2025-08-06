ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_FONT_DIR}/font.c
else
CSRC += ${CH32LIBSDK_FONT_DIR}/font_bold_8x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_game_8x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_ibmtiny_8x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_ibm_8x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_italic_8x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_thin_8x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_cond_6x8.c
CSRC += ${CH32LIBSDK_FONT_DIR}/font_cond_6x6.c
endif
