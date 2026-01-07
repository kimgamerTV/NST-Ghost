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
echo "║  • PyTorch        - AI framework (CPU version, ~800MB)           ║"
echo "║  • LaMa Inpainting - AI text removal                             ║"
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

# Find bundled Python version
PY_SITE_PACKAGES=""
for pydir in "$NST_ROOT/usr/lib"/python3.*; do
    if [ -d "$pydir/site-packages" ]; then
        PY_SITE_PACKAGES="$pydir/site-packages"
        PY_VERSION=$(basename "$pydir")
        break
    fi
done

if [ -z "$PY_SITE_PACKAGES" ]; then
    echo "❌ Error: Cannot find bundled Python in NST."
    echo "   Expected: $NST_ROOT/usr/lib/python3.X/site-packages"
    echo ""
    echo "   If you're running from source, packages will be installed"
    echo "   to your user directory instead."
    echo ""
    read -p "Install to user directory (~/.local) instead? [Y/n] " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        echo "Installation cancelled."
        exit 0
    fi
    INSTALL_TARGET="--user"
    echo "📦 Installing to user directory..."
else
    INSTALL_TARGET="--target=${PY_SITE_PACKAGES}"
    echo "✓ Found bundled Python: $PY_VERSION"
    echo "✓ Install target: $PY_SITE_PACKAGES"
fi

echo ""

# Check if pip is available
if ! command -v pip3 &> /dev/null && ! command -v pip &> /dev/null; then
    echo "❌ Error: pip is required but not installed."
    echo ""
    echo "Please install pip first:"
    echo "  Ubuntu/Debian: sudo apt install python3-pip"
    echo "  Fedora:        sudo dnf install python3-pip"
    echo "  Arch:          sudo pacman -S python-pip"
    exit 1
fi

PIP_CMD="pip3"
if ! command -v pip3 &> /dev/null; then
    PIP_CMD="pip"
fi

# Ask for confirmation
echo "This may take 10-20 minutes and download ~1GB of data."
read -p "Install AI features now? [Y/n] " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Nn]$ ]]; then
    echo "Installation cancelled."
    exit 0
fi

echo ""
echo "📦 Installing packages..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Install PyTorch CPU version (smaller than GPU version)
echo ""
echo "[1/3] Installing PyTorch (CPU)..."
$PIP_CMD install "$INSTALL_TARGET" torch torchvision --index-url https://download.pytorch.org/whl/cpu

# Install EasyOCR
echo ""
echo "[2/3] Installing EasyOCR..."
$PIP_CMD install "$INSTALL_TARGET" easyocr

# Install LaMa Inpainting
echo ""
echo "[3/3] Installing LaMa Inpainting..."
$PIP_CMD install "$INSTALL_TARGET" simple-lama-inpainting

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║  ✅ Installation Complete!                                       ║"
echo "╠══════════════════════════════════════════════════════════════════╣"
echo "║  Please restart NST to enable AI features.                       ║"
echo "║                                                                  ║"
echo "║  Note: First OCR run will download language models (~100MB).    ║"
echo "╚══════════════════════════════════════════════════════════════════╝"
echo ""
