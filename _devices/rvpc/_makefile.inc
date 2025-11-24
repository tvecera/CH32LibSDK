
# ASM source files
ifeq ($(USE_RCA),)
  ASRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc_vga_asm.S
else
  DEFINE += -D USE_RCA=1
endif

# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc_vga.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/rvpc_init.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars64/text_mode_font_80.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars64/text_mode_font_81.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars64/text_mode_font_thin_8x8_64.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars64/text_mode_font_bold_8x8_64.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars128/text_mode_font_rvpc.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars128/text_mode_font_zx.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars128/text_mode_font_thin_8x8.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/rvpc/fonts/chars128/text_mode_font_bold_8x8.c
endif

DEFINE += -D USE_RVPC=1
EXTRA_AFLAGS += -D REDUCE_CRT0_SIZE=1

ifeq ($(MCU),)
MCU=CH32V003x4
endif

