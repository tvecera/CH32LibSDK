
##############################################################################
#               Include Makefile 1st stage - prepare MCU type
##############################################################################

#  MCU ... target MCU = CH32V002x4, CH32V003x4
# MCU=CH32V002x4

# Setup device class
DEVCLASS=rvpc

# Path to root directory from the project directory (without trailing '/' delimiter)
CH32_ROOT_PATH = ../../..

# Makefile includes
include ${CH32_ROOT_PATH}/Makefile1.inc

##############################################################################
#                           Project base configuration
##############################################################################

# Target project name
TARGET=RVMON

# Destination directory
TARGETDIR=Games

##############################################################################
#                             Input files
##############################################################################

# ASM source files
ASRC +=

# C source files
CSRC += src/main.c
CSRC += src/rvmon.c
CSRC += src/print.c
CSRC += src/keyboard.c
CSRC += src/vga.c
CSRC += src/mini-printf.c

# C++ source files
SRC +=

##############################################################################
#                  Include build Makefile 2nd stage - Build
##############################################################################

# Makefile includes
include ${CH32_ROOT_PATH}/Makefile2.inc
