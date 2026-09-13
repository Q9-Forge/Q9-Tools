#!/bin/bash
#===============================================================================
# build.sh - Build iso2kr and kr2iso for Q9-Flux Emulator
#===============================================================================

set -e

ROOT=$(cd "$(dirname "$0")" && pwd)
ISO2KR_DIR="$ROOT/iso2kr"
KR2ISO_DIR="$ROOT/kr2iso"

echo "Building iso2kr and kr2iso converters..."
echo "=========================================="
echo

# Build iso2kr
echo "Building iso2kr..."
cd "$ISO2KR_DIR"
make clean
make

# Build kr2iso
echo
echo "Building kr2iso..."
cd "$KR2ISO_DIR"
make clean
make

echo
echo "=========================================="
echo "✓ Build complete!"
echo
echo "Next steps:"
echo "1. Copy iso2kr/iso2kr.c and .clang-format to /dd/PROJECTS/iso2kr/ in OS9 image"
echo "2. Copy kr2iso/kr2iso.c and .clang-format to /dd/PROJECTS/kr2iso/ in OS9 image"
echo "3. In OS9 emulator:"
echo "   - chd /dd/PROJECTS/iso2kr && qcc iso2kr.c -o /cmds/iso2kr"
echo "   - chd /dd/PROJECTS/kr2iso && qcc kr2iso.c -o /cmds/kr2iso"
echo "4. Test: iso2kr < file.c > output.kr"
