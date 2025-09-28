#!/bin/bash
# Rebuild and write...

./c.sh
if [ $? -ne 0 ]; then
    exit 1
fi
./e.sh
