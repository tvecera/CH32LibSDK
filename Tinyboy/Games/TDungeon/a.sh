#!/bin/bash
# All Re-Compilation...

./d.sh
./c.sh
if [ $? -ne 0 ]; then exit 1; fi
./e.sh