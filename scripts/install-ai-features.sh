#!/bin/bash
# =============================================================================
# NST AI Features Installer
# =============================================================================
# This script installs the AI-powered features for NST using uv.
# uv ensures correct Python version wheels are downloaded.
#
# Usage:
#   ./install-ai-features.sh          # Install CPU version (default)
#   ./install-ai-features.sh --gpu    # Install GPU version (requires CUDA)
# =============================================================================

set -e

# Parse arguments
USE_GPU=false
if [[ "$1" == "--gpu" ]] || [[ "$1" == "-g" ]]; then
    USE_GPU=true
fi

echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║           NST AI Features Installer                              ║"
echo "╠══════════════════════════════════════════════════════════════════╣"
echo "║  This will install:                                              ║"
echo "║  • EasyOCR        - Text detection from images                   ║"
if [ "$USE_GPU" = true ]; then
echo "║  • PyTorch (GPU)  - AI framework with CUDA support               ║"
else
echo "║  • PyTorch (CPU)  - AI framework (no GPU required)               ║"
fi
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

# Check if uv is installed
UV_CMD=""
if command -v uv &>/dev/null; then
    UV_CMD="uv"
elif [ -f "$HOME/.local/bin/uv" ]; then
    UV_CMD="$HOME/.local/bin/uv"
elif [ -f "$HOME/.cargo/bin/uv" ]; then
    UV_CMD="$HOME/.cargo/bin/uv"
fi

if [ -z "$UV_CMD" ]; then
    echo ""
    echo "📦 Installing uv (fast Python package manager)..."
    curl -LsSf https://astral.sh/uv/install.sh | sh
    
    # Add to path for this session
    export PATH="$HOME/.local/bin:$PATH"
    UV_CMD="$HOME/.local/bin/uv"
    
    if [ ! -f "$UV_CMD" ]; then
        UV_CMD="$HOME/.cargo/bin/uv"
    fi
    
    if [ ! -f "$UV_CMD" ]; then
        echo "❌ Error: Failed to install uv."
        exit 1
    fi
fi

echo "✓ Using uv: $UV_CMD"

# Create temp venv with correct Python version
TEMP_VENV="/tmp/nst-install-venv"
echo ""
echo "📦 Setting up Python $BUNDLED_PY_VER environment..."

# Clean up any previous venv
rm -rf "$TEMP_VENV"

# uv can download Python if needed
$UV_CMD venv --python "$BUNDLED_PY_VER" "$TEMP_VENV" 2>/dev/null || {
    echo "Installing Python $BUNDLED_PY_VER..."
    $UV_CMD python install "$BUNDLED_PY_VER"
    $UV_CMD venv --python "$BUNDLED_PY_VER" "$TEMP_VENV"
}

# Install pip in the venv (uv venv doesn't include pip by default)
echo "Installing pip in temporary environment..."
$UV_CMD pip install --python "$TEMP_VENV/bin/python" pip

echo "✓ Python $BUNDLED_PY_VER environment ready"

# Ask for confirmation
echo ""
if [ "$USE_GPU" = true ]; then
    echo "This will download PyTorch GPU (~2GB) and EasyOCR."
else
    echo "This will download PyTorch CPU (~200MB) and EasyOCR."
fi
echo "Estimated time: 5-15 minutes depending on internet speed."
echo ""
read -p "Install AI features now? [Y/n] " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Nn]$ ]]; then
    echo "Installation cancelled."
    rm -rf "$TEMP_VENV"
    exit 0
fi

echo ""
echo "📦 Installing packages..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Install PyTorch
echo ""
if [ "$USE_GPU" = true ]; then
    echo "[1/2] Installing PyTorch (GPU with CUDA)..."
    # Use pip from venv directly since uv ignores --target flag
    "$TEMP_VENV/bin/pip" install \
        --target="$PY_SITE_PACKAGES" \
        --upgrade --no-user \
        torch torchvision
else
    echo "[1/2] Installing PyTorch (CPU)..."
    "$TEMP_VENV/bin/pip" install \
        --target="$PY_SITE_PACKAGES" \
        --upgrade --no-user \
        torch torchvision --index-url https://download.pytorch.org/whl/cpu
fi

# Install EasyOCR (without deps to prevent overwriting torch CPU with CUDA)
echo ""
echo "[2/2] Installing EasyOCR..."
# First install easyocr without dependencies
"$TEMP_VENV/bin/pip" install \
    --target="$PY_SITE_PACKAGES" \
    --upgrade --no-user --no-deps \
    easyocr

# Then install remaining easyocr dependencies (excluding torch/torchvision which we already have)
echo "Installing EasyOCR dependencies..."
"$TEMP_VENV/bin/pip" install \
    --target="$PY_SITE_PACKAGES" \
    --upgrade --no-user \
    opencv-python-headless scipy numpy Pillow scikit-image python-bidi PyYAML Shapely pyclipper ninja

# Cleanup
rm -rf "$TEMP_VENV"

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
