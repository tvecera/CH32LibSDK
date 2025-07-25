CH32LibSDK - SDK library for low-end CH32 RISC-V microcontrollers
=================================================================
Pre-alpha version 0.10, under development, a lot remains to be done

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
- Flash compiled sample using e.bat with WCH-LinkE programmer (available here,
  for example: https://pajenicko.cz/usb-programator-a-debug-adapter-wch-link ).
  For the SD card version of the console, simply upload the program to the
  SD card and load it into the processor using the boot loader.
- Clear compilation using d.bat.


BabyPad
-------
BabyPad is a mini game console with 7 buttons, CH32V002A4M6 processor,
25 MHz crystal (optional), audio output, and VGA mono B&W output to a
monitor. The crystal and associated capacitors are not required - if
the software detects that the crystal is not available, it will use the
internal HSI oscillator, tuned to a frequency close to 25 MHz. In this
case, the image will be slightly noisy, but still usable - it depends
on the monitor used how well it handles image stabilization. BabyPad
uses a processor in an SOP16 package with a pin pitch of 1.27 mm, which
makes soldering easy even for beginners. Together with its simple design
(and the option of not installing a crystal), it is primarily intended
for beginners as a kit for their first introduction to electronics.

BabyPad does not include an SD card. Programs must be uploaded to the
processor using the WCH-LinkE programmer (available here, for example:
https://pajenicko.cz/usb-programator-a-debug-adapter-wch-link).

In the _devices\babypad\diagram\ folder, you will find diagram of the
BabyPad console. In the !Babypad folder, you will find a ready-made
sample programs for the BabyPad. The Babypad folder contains source
codes of sample applications for BabyPad.


Pidipad
-------
Pidipad is mini game console with 8 buttons, CH32V006E8R6 processor,
25 MHz crystal (optional), microSD card, audio output, and VGA output
to monitor. The crystal and associated capacitors do not need to be
used - if the software detects that the crystal is not present, it will
use the internal HSI oscillator, tuned to a frequency close to 25 MHz.
In this case, the image will be slightly noisy, but still usable - it
depends on the monitor used how well it handles image stabilization.

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

In the _devices\pidipad\diagram\ folder, you will find diagram of the
PidiPad console. In the !Pidipad folder, you will find a ready-made SD
card image. The Pidipad folder contains source codes of sample
applications for PidiPad.
