#!/bin/bash
# Reset the device

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Detect operating system and set minichlink path
case "$(uname -s)" in
    Darwin*)
        MINICHLINK="$SCRIPT_DIR/_tools/macos/minichlink"
        ;;
    Linux*)
        MINICHLINK="$SCRIPT_DIR/_tools/linux/minichlink"
        ;;
    *)
        echo "Unsupported operating system: $(uname -s)"
        exit 1
        ;;
esac

# Check if minichlink exists
if [ ! -x "$MINICHLINK" ]; then
    echo "Error: minichlink not found at $MINICHLINK"
    exit 1
fi

"$MINICHLINK" -b

if [ $? -ne 0 ]; then
    read -p "Press any key to continue..."
fi