#!/usr/bin/env bash
# Builds dist/SEPAKAT.app - a double-clickable bundle that runs on a Mac with
# no Homebrew and no compiler installed.
#
# The whole point of this script is the last two steps: a plain `make` build
# links against Homebrew's libraylib.dylib, so the binary would die with
# "image not found" on any machine that has not run `brew install raylib`.
# We relink against the static archive when there is one, and copy the dylib
# into the bundle when there is not.
#
#   ./packaging/package_macos.sh [version]

set -euo pipefail

VERSION="${1:-v0}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

APP="dist/SEPAKAT.app"
EXE="$APP/Contents/MacOS/SEPAKAT"
PREFIX="$(brew --prefix raylib)"

rm -rf dist
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"

FRAMEWORKS="-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework CoreAudio -framework AudioToolbox"

make fclean
if [ -f "$PREFIX/lib/libraylib.a" ]; then
    echo "==> linking raylib statically"
    make RAYLIB_LIBS="$PREFIX/lib/libraylib.a $FRAMEWORKS"
else
    echo "==> no static raylib, will bundle the dylib"
    make
fi

cp game-jam "$EXE"

# Rewrite any absolute Homebrew reference to a path inside the bundle.
DYLIBS="$(otool -L "$EXE" | awk '/homebrew|\/usr\/local\/(opt|Cellar)/ {print $1}')"
if [ -n "$DYLIBS" ]; then
    mkdir -p "$APP/Contents/Frameworks"
    for lib in $DYLIBS; do
        base="$(basename "$lib")"
        cp "$lib" "$APP/Contents/Frameworks/$base"
        chmod u+w "$APP/Contents/Frameworks/$base"
        install_name_tool -change "$lib" "@executable_path/../Frameworks/$base" "$EXE"
    done
fi

cat > "$APP/Contents/Info.plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key><string>SEPAKAT</string>
    <key>CFBundleDisplayName</key><string>SEPAKAT</string>
    <key>CFBundleExecutable</key><string>SEPAKAT</string>
    <key>CFBundleIdentifier</key><string>io.github.xw675.sepakat</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>${VERSION#v}</string>
    <key>CFBundleVersion</key><string>${VERSION#v}</string>
    <key>NSHighResolutionCapable</key><true/>
    <key>LSMinimumSystemVersion</key><string>11.0</string>
</dict>
</plist>
PLIST

# install_name_tool invalidates the linker's signature, and an arm64 binary
# with a broken signature is killed on launch. Ad-hoc sign it again.
codesign --force --sign - "$APP"

# Fail loudly rather than shipping a bundle that only runs on this machine.
if otool -L "$EXE" | grep -qE 'homebrew|/usr/local/(opt|Cellar)'; then
    echo "ERROR: bundle still references Homebrew:" >&2
    otool -L "$EXE" >&2
    exit 1
fi

ARCH="$(uname -m)"
ditto -c -k --keepParent "$APP" "dist/SEPAKAT-${VERSION}-macos-${ARCH}.zip"

echo "==> dist/SEPAKAT-${VERSION}-macos-${ARCH}.zip"
otool -L "$EXE"
