# ASM source files
ifneq (${NOBOOT},1)
ASRC += ${CH32LIBSDK_LOADER_DIR}/loader_tweetyboy.S
endif

# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy_disp.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy_ss.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tweetyboy/tweetyboy_init.c
endif

DEFINE += -D USE_TWEETYBOY=1

MCU=CH32X035x8
