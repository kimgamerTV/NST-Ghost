#!/bin/bash
# =============================================================================
# NST AI Features Installer
# =============================================================================
# This script installs the AI-powered features for NST (Neural Screenshot Tool)
# including OCR (text detection) and AI inpainting (text removal).
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
NST_ROOT="$(dirname "$(dirname "$(dirname "$SCRIPT_DIR")")")"

if [ ! -d "$NST_ROOT/usr/lib" ]; then
    NST_ROOT="$(dirname "$SCRIPT_DIR")"
    if [ ! -d "$NST_ROOT/usr/lib" ]; then
        echo "❌ Error: Cannot find NST installation directory."
        exit 1
    fi
fi

# Find bundled Python version
PY_SITE_PACKAGES=""
BUNDLED_PY_VER=""

for pydir in "$NST_ROOT/usr/lib"/python3.*; do
    if [ -d "$pydir/site-packages" ]; then
        PY_SITE_PACKAGES="$pydir/site-packages"
        BUNDLED_PY_VER=$(basename "$pydir" | sed 's/python//')
        break
    fi
done

if [ -z "$PY_SITE_PACKAGES" ]; then
    echo "❌ Error: Cannot find bundled Python in NST."
    exit 1
fi

echo "✓ NST bundled Python: $BUNDLED_PY_VER"
echo "✓ Install target: $PY_SITE_PACKAGES"

# Check system Python version
if command -v python3 &>/dev/null; then
    SYSTEM_PY_VER=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
    echo "✓ System Python: $SYSTEM_PY_VER"
else
    echo "❌ Error: Python 3 is required but not installed."
    exit 1
fi

# Check if versions match
if [ "$BUNDLED_PY_VER" != "$SYSTEM_PY_VER" ]; then
    echo ""
    echo "⚠️  Python version mismatch detected!"
    echo "   NST uses Python $BUNDLED_PY_VER but your system has Python $SYSTEM_PY_VER"
    echo ""
    echo "   This may cause compatibility issues with binary packages."
    echo "   The installer will still try to install, but OCR may not work."
    echo ""
fi

# Check for pip
if command -v pip3 &>/dev/null; then
    PIP_CMD="pip3"
elif command -v pip &>/dev/null; then
    PIP_CMD="pip"
else
    echo "❌ Error: pip is required but not installed."
    exit 1
fi

echo ""
echo "This may take 10-20 minutes and download ~500MB of data."
read -p "Install AI features now? [Y/n] " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Nn]$ ]]; then
    echo "Installation cancelled."
    exit 0
fi

echo ""
echo "📦 Installing packages..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Install PyTorch CPU version
echo ""
echo "[1/2] Installing PyTorch (CPU)..."
$PIP_CMD install --target="$PY_SITE_PACKAGES" torch torchvision --index-url https://download.pytorch.org/whl/cpu

# Install EasyOCR  
echo ""
echo "[2/2] Installing EasyOCR..."
$PIP_CMD install --target="$PY_SITE_PACKAGES" easyocr

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

if [ "$BUNDLED_PY_VER" != "$SYSTEM_PY_VER" ]; then
    echo "╔══════════════════════════════════════════════════════════════════╗"
    echo "║  ⚠️  Installation completed with warnings                        ║"
    echo "╠══════════════════════════════════════════════════════════════════╣"
    echo "║  Packages were installed, but Python version mismatch means     ║"
    echo "║  OCR may not work. To fix this, either:                         ║"
    echo "║                                                                  ║"
    echo "║  1. Install Python $BUNDLED_PY_VER on your system, or           ║"
    echo "║  2. Wait for a newer NST release matching your Python version   ║"
    echo "╚══════════════════════════════════════════════════════════════════╝"
else
    echo "╔══════════════════════════════════════════════════════════════════╗"
    echo "║  ✅ Installation Complete!                                       ║"
    echo "╠══════════════════════════════════════════════════════════════════╣"
    echo "║  Please restart NST to enable AI features.                       ║"
    echo "║                                                                  ║"
    echo "║  Note: First OCR run will download language models (~100MB).    ║"
    echo "╚══════════════════════════════════════════════════════════════════╝"
fi
echo ""
