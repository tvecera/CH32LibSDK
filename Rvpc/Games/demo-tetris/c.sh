#!/bin/bash

# Compilation...

export TARGET="RVPC"
export GRPDIR="CH32"
export MEMMAP=""
export MCU="$1"

${CH32_ROOT_PATH}/_c1.sh $1
