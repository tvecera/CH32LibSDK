
# ASM source files
ASRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad_vga_asm.S

# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad_vga.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypad/babypad_init.c
endif

DEFINE += -D USE_BABYPAD=1

MCU=CH32V002x4
