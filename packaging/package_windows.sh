#!/usr/bin/env bash
# Builds dist/SEPAKAT-<version>-windows-x64.zip - unzip, double-click the .exe.
# Run from an MSYS2 MINGW64 shell:
#
#   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib make zip
#   ./packaging/package_windows.sh [version]
#
# The DLL copy at the bottom is the part that matters: MSYS2 links raylib as a
# DLL, so the .exe alone would fail on a machine with no MSYS2 installed.

set -euo pipefail

VERSION="${1:-v0}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

OUT="dist/SEPAKAT"

rm -rf dist
mkdir -p "$OUT"

make fclean
make

cp game-jam.exe "$OUT/SEPAKAT.exe"

# Everything under /mingw64 is ours to ship; C:\Windows\System32 is the player's.
ldd "$OUT/SEPAKAT.exe" \
    | awk 'tolower($3) ~ /mingw64/ {print $3}' \
    | while read -r dll; do cp -n "$dll" "$OUT/"; done

cat > "$OUT/README.txt" <<TXT
SEPAKAT ${VERSION}

Double-click SEPAKAT.exe.

Windows will show "Windows protected your PC" because the game is not
code-signed. Click "More info", then "Run anyway". The source is at
https://github.com/xw675/game-jam if you would rather build it yourself.

Arrows or WASD to move. Walk into a guardian to strike it. R restarts.
Take all four tokens, then reach the Hall.
TXT

cd dist && zip -r "SEPAKAT-${VERSION}-windows-x64.zip" SEPAKAT >/dev/null

echo "==> dist/SEPAKAT-${VERSION}-windows-x64.zip"
ls -la SEPAKAT/
