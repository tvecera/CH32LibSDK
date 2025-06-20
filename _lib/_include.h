// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

#ifndef _LIB_INCLUDE_H
#define _LIB_INCLUDE_H

//#define INCLUDE_SDK_FILE(file) STRINGIFY(SDK_SUBDIR/file)

#include STRINGIFY(../_sdk/SDK_SUBDIR/sdk_cpu.h) // CPU control

#include "inc/lib_decnum.h"		// decode number
#include "inc/lib_rand.h"		// random generator
#include "inc/lib_sd.h"			// SD card
#include "inc/lib_fat.h"		// FAT file system
#include "inc/lib_crc.h"		// check sum

#endif // _LIB_INCLUDE_H
