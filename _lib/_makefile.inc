
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_LIB_DIR}/lib.c
else
CSRC += ${CH32LIBSDK_LIB_DIR}/src/lib_decnum.c
CSRC += ${CH32LIBSDK_LIB_DIR}/src/lib_rand.c
CSRC += ${CH32LIBSDK_LIB_DIR}/src/lib_sd.c
CSRC += ${CH32LIBSDK_LIB_DIR}/src/lib_fat.c
CSRC += ${CH32LIBSDK_LIB_DIR}/src/lib_crc.c
endif
