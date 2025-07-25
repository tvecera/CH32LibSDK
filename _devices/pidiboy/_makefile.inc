# ASM source files
ifneq (${NOBOOT},1)
ASRC += ${CH32LIBSDK_LOADER_DIR}/loader_PidiBoy.S
endif

# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy_disp.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy_ss.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/pidiboy/pidiboy_init.c
endif

DEFINE += -D USE_PIDIBOY=1

MCU=CH32V006x8
