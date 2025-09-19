#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/main/build"
DIST_DIR="$ROOT_DIR/dist"
REL_DIR="$DIST_DIR/release"
INFO_DIR="$DIST_DIR/info"
SYSTEM_HOOK_DIR="$DIST_DIR/system/hook"

# Clear release directory
rm -rf "$REL_DIR"/*
mkdir -p "$REL_DIR"

# Build for each platform
echo "Building for Linux..."
cd "$BUILD_DIR" && make clean && make linux
echo "Building for Windows..."
cd "$BUILD_DIR" && make clean && make windows64
echo "Building for Android ARMv7..."
cd "$BUILD_DIR" && make clean && make android32
echo "Building for Android ARMv8..."
cd "$BUILD_DIR" && make clean && make android64

# Copy built artifacts
# Unix
if [[ -f "$ROOT_DIR/main/generated/unix/hook_libretro.so" ]]; then
  mkdir -p "$REL_DIR/unix"; cp "$ROOT_DIR/main/generated/unix/hook_libretro.so" "$REL_DIR/unix/";
fi
# Windows
if [[ -f "$ROOT_DIR/main/generated/wincross64/hook_libretro.dll" ]]; then
  mkdir -p "$REL_DIR/windows-x64"; cp "$ROOT_DIR/main/generated/wincross64/hook_libretro.dll" "$REL_DIR/windows-x64/";
fi
# Android
for ABI in armeabi-v7a arm64-v8a; do
  if [[ -f "$ROOT_DIR/main/generated/$ABI/hook_libretro.so" ]]; then
    mkdir -p "$REL_DIR/android-$ABI"; cp "$ROOT_DIR/main/generated/$ABI/hook_libretro.so" "$REL_DIR/android-$ABI/";
  fi
done

# Copy .info
if [[ -f "$INFO_DIR/hook_libretro.info" ]]; then
  mkdir -p "$REL_DIR/info"; cp "$INFO_DIR/hook_libretro.info" "$REL_DIR/info/";
fi

# Copy system hook files
if [[ -d "$SYSTEM_HOOK_DIR" ]]; then
  mkdir -p "$REL_DIR/system/hook"; cp -r "$SYSTEM_HOOK_DIR"/* "$REL_DIR/system/hook/";
fi

cat <<EOF
Package layout generated under: $REL_DIR
EOF
