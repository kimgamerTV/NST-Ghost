#!/usr/bin/env bash

set -euo pipefail

BUILD_DIR="${1:-build-fedora}"
PREFIX="${2:-/usr/local}"

echo "[INFO] Installing Fedora dependencies..."
sudo dnf install -y cmake ninja-build gcc-c++ qt6-qtbase-devel qt6-qttools-devel qt6-linguist qt6-qtdeclarative-devel

echo "[INFO] Configuring project..."
cmake -S "$(dirname "${BASH_SOURCE[0]}")/.." -B "${BUILD_DIR}" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${PREFIX}"

echo "[INFO] Building..."
cmake --build "${BUILD_DIR}" --config Release

echo "[INFO] Running tests..."
ctest --output-on-failure --test-dir "${BUILD_DIR}"

echo "[INFO] Installing to ${PREFIX} (sudo may be required)..."
cmake --install "${BUILD_DIR}" --config Release

echo "[INFO] Done. Binaries available under ${PREFIX}/bin"
