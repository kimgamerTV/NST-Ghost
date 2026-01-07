#!/bin/bash
# =============================================================================
# NST AI Features Installer
# =============================================================================
# This script installs the AI-powered features for NST (Neural Screenshot Tool)
# including OCR (text detection) and AI inpainting (text removal).
#
# The packages are installed directly into NST's bundled Python environment,
# so they work regardless of your system's Python version.
# =============================================================================

set -e

echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║           NST AI Features Installer                              ║"
echo "╠══════════════════════════════════════════════════════════════════╣"
echo "║  This will install:                                              ║"
echo "║  • EasyOCR        - Text detection from images                   ║"
echo "║  • PyTorch        - AI framework (CPU version)                   ║"
echo "╚══════════════════════════════════════════════════════════════════╝"
echo ""

# Find NST root directory from script location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Script is in: NST_ROOT/usr/bin/scripts/
# So NST_ROOT is 3 levels up
NST_ROOT="$(dirname "$(dirname "$(dirname "$SCRIPT_DIR")")")"

# Check if we're in a valid NST installation
if [ ! -d "$NST_ROOT/usr/lib" ]; then
    # Maybe running from source (scripts/ is at root level)
    NST_ROOT="$(dirname "$SCRIPT_DIR")"
    if [ ! -d "$NST_ROOT/usr/lib" ] && [ ! -f "$NST_ROOT/src/core/main.cpp" ]; then
        echo "❌ Error: Cannot find NST installation directory."
        echo "   Please run this script from the NST folder."
        exit 1
    fi
fi

# Find bundled Python version and its pip
BUNDLED_PYTHON=""
PY_SITE_PACKAGES=""
PY_VERSION=""

for pydir in "$NST_ROOT/usr/lib"/python3.*; do
    if [ -d "$pydir/site-packages" ]; then
        PY_SITE_PACKAGES="$pydir/site-packages"
        PY_VERSION=$(basename "$pydir")
        # Try to find bundled python executable
        for pybin in "$NST_ROOT/usr/bin/python3" "$NST_ROOT/usr/bin/python" "/usr/bin/$PY_VERSION"; do
            if [ -x "$pybin" ]; then
                BUNDLED_PYTHON="$pybin"
                break
            fi
        done
        break
    fi
done

# Determine pip command and install target
if [ -n "$BUNDLED_PYTHON" ] && "$BUNDLED_PYTHON" -m pip --version &>/dev/null 2>&1; then
    # Use bundled Python's pip
    PIP_CMD="$BUNDLED_PYTHON -m pip"
    INSTALL_TARGET="--target=${PY_SITE_PACKAGES}"
    echo "✓ Using bundled Python: $BUNDLED_PYTHON"
    echo "✓ Install target: $PY_SITE_PACKAGES"
elif [ -n "$PY_SITE_PACKAGES" ]; then
    # Try system pip with platform flag for compatibility
    if command -v pip3 &>/dev/null; then
        PIP_CMD="pip3"
    elif command -v pip &>/dev/null; then
        PIP_CMD="pip"
    else
        echo "❌ Error: pip is required but not installed."
        exit 1
    fi
    INSTALL_TARGET="--target=${PY_SITE_PACKAGES}"
    echo "✓ Found bundled Python: $PY_VERSION"
    echo "✓ Install target: $PY_SITE_PACKAGES"
    echo ""
    echo "⚠️  Warning: Using system pip. Some packages may need rebuilding."
else
    echo "❌ Error: Cannot find bundled Python in NST."
    echo "   Expected: $NST_ROOT/usr/lib/python3.X/site-packages"
    echo ""
    read -p "Install to user directory (~/.local) instead? [Y/n] " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        echo "Installation cancelled."
        exit 0
    fi
    
    if command -v pip3 &>/dev/null; then
        PIP_CMD="pip3"
    else
        PIP_CMD="pip"
    fi
    INSTALL_TARGET="--user"
    echo "📦 Installing to user directory..."
fi

echo ""

# Ask for confirmation
echo "This may take 10-20 minutes and download ~500MB of data."
read -p "Install AI features now? [Y/n] " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Nn]$ ]]; then
    echo "Installation cancelled."
    exit 0
fi

# Extract Python minor version (e.g., "12" from "python3.12")
PY_MINOR_VER=$(echo "$PY_VERSION" | sed 's/python3\.//')
if [ -n "$PY_MINOR_VER" ]; then
    # Force pip to download wheels for the bundled Python version
    PIP_PLATFORM_FLAGS="--python-version=3.${PY_MINOR_VER} --platform=manylinux2014_x86_64 --only-binary=:all:"
else
    PIP_PLATFORM_FLAGS=""
fi

echo ""
echo "📦 Installing packages..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Install PyTorch CPU version (smaller than GPU version)
echo ""
echo "[1/2] Installing PyTorch (CPU) for Python 3.${PY_MINOR_VER}..."
$PIP_CMD install "$INSTALL_TARGET" $PIP_PLATFORM_FLAGS torch torchvision --index-url https://download.pytorch.org/whl/cpu

# Install EasyOCR
echo ""
echo "[2/2] Installing EasyOCR for Python 3.${PY_MINOR_VER}..."
$PIP_CMD install "$INSTALL_TARGET" $PIP_PLATFORM_FLAGS easyocr

# NOTE: simple-lama-inpainting is skipped due to incompatible dependencies
# (requires pillow<10, numpy<2 which conflict with modern Python)
# The app will fall back to OpenCV inpainting which works fine

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║  ✅ Installation Complete!                                       ║"
echo "╠══════════════════════════════════════════════════════════════════╣"
echo "║  Please restart NST to enable AI features.                       ║"
echo "║                                                                  ║"
echo "║  Note: First OCR run will download language models (~100MB).    ║"
echo "║                                                                  ║"
echo "║  Inpainting: Uses OpenCV (bundled). For LaMa AI inpainting,     ║"
echo "║  manually run: pip install simple-lama-inpainting               ║"
echo "╚══════════════════════════════════════════════════════════════════╝"
echo ""
