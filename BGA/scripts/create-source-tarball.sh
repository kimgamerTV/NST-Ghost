#!/usr/bin/env bash
set -euo pipefail

VERSION="${1:-0.1.0}"
ARCHIVE="bga-${VERSION}.tar.gz"

git archive --format=tar --prefix="bga-${VERSION}/" HEAD | gzip > "${ARCHIVE}"
echo "Created ${ARCHIVE}"
