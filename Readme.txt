CH32LibSDK - SDK library for low-end CH32 RISC-V microcontrollers
=================================================================
Pre-alpha version 0.01, under development, a lot remains to be done

Copyright (c) 2025 Miroslav Nemecek

Panda38@seznam.cz
hardyplotter2@gmail.com
https://github.com/Panda381/CH32LibSDK
https://www.breatharian.eu/hw/ch32libsdk/index_en.html

How to compile on Windows, using command line:

- Use FAR Manager to edit source files and run BAT scripts.
- Download MounRiver Studio v1.92 http://mounriver.com/download
- Install MounRiver Studio. 
- Extract "toolchain\RISC-V Embedded GCC12" compiler to separate folder.
- Edit _c1.bat file and enter path to "bin" folder of the compiler:
   set GCC_CH32_PATH=C:\GCC_CH32\bin
- Extract "toolchain\OpenOCD\bin" tool or use openocd from _tools\openocd.
- Sometimes you may need to use WCH-LinkUtility from the ExTool\SQWTool folder.
  It may sometimes be necessary to erase the flash memory using power-off,
  because OpenOcd cannot do this.
- Compile sample using c.bat.
- Flash compiled sample using e.bat.
- Clear compilation using d.bat.


Pidipad
-------
Programs compiled for PidiPad contain a boot loader. Writing to flash
memory using a programmer writes both the program and the boot loader.
Pressing the Y button (or resetting with an SD card inserted) launches
the boot loader, allowing you to select the program to be run from the
SD card. The boot loader ensures that the program is loaded from the SD
card into the flash memory. Another program can be loaded by pressing Y
or by resetting with an SD card inserted. If no SD card is inserted after
resetting, the boot loader will not start, but the program in the flash
memory will start immediately. The program in the flash memory can also
be started from the boot loader by pressing the Y button.

BIN and ELF files are provided with a checksum during compilation using
the LoaderCrc program. This is used by the boot loader to check the
application in flash memory. If you want to load the application into
flash memory using WCH-LinkUtility, you must use a BIN file and not a HEX
file, because the generated HEX file does not have a checksum.
