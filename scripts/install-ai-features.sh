#!/bin/bash
# =============================================================================
# NST AI Features Installer
# =============================================================================
# This script installs the AI-powered features for NST (Neural Screenshot Tool)
# including OCR (text detection) and AI inpainting (text removal).
#
# Uses the bundled Python and pip from the AppImage - no system Python required!
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

# Check if we're in a valid NST installation
if [ ! -d "$NST_ROOT/usr/lib" ]; then
    NST_ROOT="$(dirname "$SCRIPT_DIR")"
    if [ ! -d "$NST_ROOT/usr/lib" ]; then
        echo "❌ Error: Cannot find NST installation directory."
        echo "   Please run this script from the NST folder."
        exit 1
    fi
fi

# Find bundled Python
BUNDLED_PYTHON=""
PY_SITE_PACKAGES=""
PY_VERSION=""

for pydir in "$NST_ROOT/usr/lib"/python3.*; do
    if [ -d "$pydir/site-packages" ]; then
        PY_SITE_PACKAGES="$pydir/site-packages"
        PY_VERSION=$(basename "$pydir")
        break
    fi
done

# Find bundled Python executable
for pybin in "$NST_ROOT/usr/bin/python3" "$NST_ROOT/usr/bin/python"; do
    if [ -x "$pybin" ]; then
        BUNDLED_PYTHON="$pybin"
        break
    fi
done

# Check for bundled pip
if [ -z "$PY_SITE_PACKAGES" ]; then
    echo "❌ Error: Cannot find bundled Python in NST."
    exit 1
fi

# Check if pip module is available in bundled Python
if [ -d "$PY_SITE_PACKAGES/pip" ]; then
    echo "✓ Found bundled Python: $PY_VERSION"
    echo "✓ Found bundled pip"
    echo "✓ Install target: $PY_SITE_PACKAGES"
    
    # Use bundled Python with bundled pip
    # Set PYTHONPATH to use bundled site-packages
    export PYTHONHOME="$NST_ROOT/usr"
    export PYTHONPATH="$PY_SITE_PACKAGES:$NST_ROOT/usr/lib/$PY_VERSION"
    
    # Try to find system python that matches bundled version
    PY_MINOR=$(echo "$PY_VERSION" | sed 's/python3\.//')
    PYTHON_CMD=""
    for py in "/usr/bin/python3.$PY_MINOR" "/usr/bin/python3" "python3.$PY_MINOR" "python3"; do
        if command -v "$py" &>/dev/null; then
            PYTHON_CMD="$py"
            break
        fi
    done
    
    if [ -z "$PYTHON_CMD" ]; then
        echo "❌ Error: Cannot find Python 3 on your system."
        echo "   Please install Python 3: sudo apt install python3"
        exit 1
    fi
    
    PIP_CMD="$PYTHON_CMD -m pip"
    INSTALL_TARGET="--target=$PY_SITE_PACKAGES"
else
    echo "⚠️  Bundled pip not found. Using system pip..."
    
    if command -v pip3 &>/dev/null; then
        PIP_CMD="pip3"
    elif command -v pip &>/dev/null; then
        PIP_CMD="pip"
    else
        echo "❌ Error: pip is required but not installed."
        exit 1
    fi
    INSTALL_TARGET="--target=$PY_SITE_PACKAGES"
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
$PIP_CMD install "$INSTALL_TARGET" torch torchvision --index-url https://download.pytorch.org/whl/cpu

# Install EasyOCR
echo ""
echo "[2/2] Installing EasyOCR..."
$PIP_CMD install "$INSTALL_TARGET" easyocr

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
