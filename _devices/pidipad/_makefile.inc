
# ASM source files
ifneq (${NOBOOT},1)
ASRC += ${CH32LIBSDK_LOADER_DIR}/loader_PidiPad.S
endif
ASRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad_vga_asm.S

# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad_vga.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidipad/pidipad_init.c
endif

DEFINE += -D USE_PIDIPAD=1

MCU=CH32V006x8
