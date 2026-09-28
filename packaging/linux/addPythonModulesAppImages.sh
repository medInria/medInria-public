#!/bin/bash

########### Prerequisites ###########
# wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
# chmod +x appimagetool-x86_64.AppImage
# sudo mv appimagetool-x86_64.AppImage /usr/local/bin/appimagetool
# sudo apt install patchelf

########### Usage ###########
# chmod +x addPythonModulesAppImages.sh
# ./addPythonModulesAppImages.sh medInria-*.AppImage [module_name1] [module_name2] ...

set -e

ORIGINAL_DIR="$(pwd)"
APPIMAGE="$(realpath "$1")"
MODULES="${@:2}"

# Checkups
[ ! -f "$APPIMAGE" ] && echo "Error: $APPIMAGE does not exist" && exit 1
command -v appimagetool >/dev/null || { echo "Error: appimagetool is not found"; exit 1; }

# Create a temporary secured directory,
# at the end of the script, it's safely removed
TEMP_DIR=$(mktemp -d)
trap "rm -rf '$TEMP_DIR'" EXIT 
cd "$TEMP_DIR"

# Extraction of the AppImage in the temporary directory
echo "# Extraction of the AppImage..."
"$APPIMAGE" --appimage-extract

# Install Python modules
echo "# Installation of modules: $MODULES"
PYTHON_BIN="squashfs-root/lib/python3.12/bin/python3.12_bin"
[ ! -f "$PYTHON_BIN" ] && echo "Error: embedded Python is not found" && exit 1
"$PYTHON_BIN" -m pip install  --upgrade "$MODULES"

# Recreate an AppImage bundle
echo "# Rebundling of the AppImage..."
NAME_BASE="$(basename "$APPIMAGE" .AppImage)"
appimagetool squashfs-root "${ORIGINAL_DIR}/${NAME_BASE}-updated.AppImage"
