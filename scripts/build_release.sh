#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Starting release build process..."

TARGET_OS=""
if [ -n "$1" ]; then
    TARGET_OS="$1"
fi

CURRENT_OS=$(uname -s)
CPACK_GENERATOR=""
ASSET_NAME="nst"

case "$TARGET_OS" in
    "linux")
        if [ "$CURRENT_OS" != "Linux" ]; then
            echo "Error: Cannot build Linux package on $CURRENT_OS. Please run this script on a Linux system."
            exit 1
        fi
        CPACK_GENERATOR="ZIP"
        ASSET_NAME="NST-Linux"
        ;;
    "macos")
        if [ "$CURRENT_OS" != "Darwin" ]; then
            echo "Error: Cannot build macOS package on $CURRENT_OS. Please run this script on a macOS system."
            exit 1
        fi
        CPACK_GENERATOR="DragNDrop"
        ASSET_NAME="NST-macOS"
        ;;
    "windows")
        # Cross-compiling for Windows from Linux is complex and not supported by this script.
        # This check is mainly for when the script is run on a Windows system (e.g., Git Bash/WSL)
        # and the user explicitly requests a Windows build.
        if [[ "$CURRENT_OS" != *"NT"* && "$CURRENT_OS" != "MINGW64_NT"* ]]; then
            echo "Error: Cannot build Windows package on $CURRENT_OS. Please run this script on a Windows system (e.g., Git Bash/WSL) or use a dedicated Windows build environment."
            exit 1
        fi
        CPACK_GENERATOR="ZIP"
        ASSET_NAME="NST-windows"
        ;;
    "")
        # Default to current OS if no target is specified
        if [ "$CURRENT_OS" == "Linux" ]; then
            CPACK_GENERATOR="ZIP"
            ASSET_NAME="NST-Linux"
        elif [ "$CURRENT_OS" == "Darwin" ]; then
            CPACK_GENERATOR="DragNDrop"
            ASSET_NAME="NST-macOS"
        elif [[ "$CURRENT_OS" == *"NT"* || "$CURRENT_OS" == "MINGW64_NT"* ]]; then
            CPACK_GENERATOR="ZIP"
            ASSET_NAME="NST-windows"
        else
            echo "Error: Unsupported operating system: $CURRENT_OS. Please specify target OS (linux, macos, windows)."
            exit 1
        fi
        ;;
    *)
        echo "Error: Invalid target OS specified: $TARGET_OS. Supported targets are: linux, macos, windows."
        exit 1
        ;;
esac

echo "Building for target OS: $TARGET_OS (CPack Generator: $CPACK_GENERATOR)"

# Create build directory
echo "Creating build directory..."
mkdir -p build

# Configure CMake for Release build
echo "Configuring CMake..."
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON

# Build the project
echo "Building project..."
cmake --build . --config Release --parallel

# Create the package using CPack
echo "Creating package with CPack..."
cpack -G "$CPACK_GENERATOR" -C Release

# Find the generated package file
PACKAGE_FILE=$(ls "NST-"* 2>/dev/null | head -n 1 || true)

if [ -z "$PACKAGE_FILE" ]; then
    echo "Error: No package file found for $ASSET_NAME."
    exit 1
fi

# Move the package to the root directory
echo "Moving package to root directory: $PACKAGE_FILE"
mv "$PACKAGE_FILE" ../"$PACKAGE_FILE"

# Clean up build directory
echo "Cleaning up build directory..."
cd ..
rm -rf build

echo "Release build process completed successfully!"
echo "Package created: $PACKAGE_FILE"