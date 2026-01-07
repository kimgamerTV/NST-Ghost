#!/bin/bash
# =============================================================================
# NST AI Features Check
# =============================================================================
# This script checks if AI features are properly installed.
# In newer versions of NST, AI features (EasyOCR, PyTorch) come pre-installed.
# =============================================================================

echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║           NST AI Features                                        ║"
echo "╚══════════════════════════════════════════════════════════════════╝"
echo ""

# Find NST root directory from script location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
NST_ROOT="$(dirname "$(dirname "$(dirname "$SCRIPT_DIR")")")"

if [ ! -d "$NST_ROOT/usr/lib" ]; then
    NST_ROOT="$(dirname "$SCRIPT_DIR")"
fi

# Find bundled Python site-packages
PY_SITE_PACKAGES=""
for pydir in "$NST_ROOT/usr/lib"/python3.*; do
    if [ -d "$pydir/site-packages" ]; then
        PY_SITE_PACKAGES="$pydir/site-packages"
        break
    fi
done

echo "Checking AI features..."
echo ""

# Check for torch
if [ -d "$PY_SITE_PACKAGES/torch" ]; then
    echo "✅ PyTorch: Installed"
else
    echo "❌ PyTorch: Not found"
fi

# Check for easyocr
if [ -d "$PY_SITE_PACKAGES/easyocr" ]; then
    echo "✅ EasyOCR: Installed"
else
    echo "❌ EasyOCR: Not found"
fi

# Check for numpy
if [ -d "$PY_SITE_PACKAGES/numpy" ]; then
    echo "✅ NumPy: Installed"
else
    echo "❌ NumPy: Not found"
fi

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

if [ -d "$PY_SITE_PACKAGES/torch" ] && [ -d "$PY_SITE_PACKAGES/easyocr" ]; then
    echo ""
    echo "✅ All AI features are installed and ready!"
    echo ""
    echo "Just run NST and the Image Translation feature should work."
    echo "First OCR run will download language models (~100MB)."
else
    echo ""
    echo "⚠️  Some AI features are missing."
    echo ""
    echo "Please download the latest version of NST which includes"
    echo "pre-bundled AI features, or build from source."
fi
echo ""
