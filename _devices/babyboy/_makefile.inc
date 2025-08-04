# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babyboy/babyboy.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babyboy/babyboy_disp.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babyboy/babyboy_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babyboy/babyboy_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babyboy/babyboy_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babyboy/babyboy_init.c
endif

DEFINE += -D USE_BABYBOY=1

MCU=CH32V002x4
