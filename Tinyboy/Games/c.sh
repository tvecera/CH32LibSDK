#!/bin/bash
# Compilation... Compile all projects in all sub-directories

# Function to compile one project in subdirectory
comp1() {
    local dir="$1"
    if [ ! -f "$dir/c.sh" ]; then
        return
    fi
    cd "$dir" || return
    echo
    echo "======== Compiling $dir ========"
    ./c.sh
    cd ..
}

# Loop through all subdirectories
for dir in */; do
    if [ -d "$dir" ]; then
        comp1 "${dir%/}"
    fi
done