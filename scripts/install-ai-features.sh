#!/bin/bash
# =============================================================================
# NST AI Features Installer
# =============================================================================
# This script installs the AI-powered features for NST (Neural Screenshot Tool)
# including OCR (text detection) and AI inpainting (text removal).
#
# Requirements: Python 3.8+ with pip
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

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "❌ Error: Python 3 is required but not installed."
    echo ""
    echo "Please install Python 3 first:"
    echo "  Ubuntu/Debian: sudo apt install python3 python3-pip"
    echo "  Fedora:        sudo dnf install python3 python3-pip"
    echo "  Arch:          sudo pacman -S python python-pip"
    exit 1
fi

PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
echo "✓ Found Python $PYTHON_VERSION"
echo ""

# Ask for confirmation
read -p "Install AI features now? This may take 10-20 minutes. [Y/n] " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]] && [[ ! -z $REPLY ]]; then
    echo "Installation cancelled."
    exit 0
fi

echo ""
echo "📦 Installing packages..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Install PyTorch CPU version (smaller than GPU version)
echo ""
echo "[1/3] Installing PyTorch (CPU)..."
pip3 install --user torch torchvision --index-url https://download.pytorch.org/whl/cpu

# Install EasyOCR
echo ""
echo "[2/3] Installing EasyOCR..."
pip3 install --user easyocr

# Install LaMa Inpainting
echo ""
echo "[3/3] Installing LaMa Inpainting..."
pip3 install --user simple-lama-inpainting

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
