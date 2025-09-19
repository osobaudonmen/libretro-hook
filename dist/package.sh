#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/src/build"
DIST_DIR="$ROOT_DIR/dist"
REL_DIR="$DIST_DIR/release"
INFO_DIR="$DIST_DIR/info"
PATTERN_SAMPLE="$DIST_DIR/patterns/path_patterns.sample.txt"

mkdir -p "$REL_DIR"

# Platforms map: src artifact -> dest subdir -> final filename
# Unix
if [[ -f "$BUILD_DIR/hook_libretro.so" ]]; then
  mkdir -p "$REL_DIR/unix"; cp "$BUILD_DIR/hook_libretro.so" "$REL_DIR/unix/";
fi
# Windows
if [[ -f "$BUILD_DIR/hook_libretro.dll" ]]; then
  mkdir -p "$REL_DIR/windows-x64"; cp "$BUILD_DIR/hook_libretro.dll" "$REL_DIR/windows-x64/";
fi
# Android
for ABI in armeabi-v7a arm64-v8a; do
  if [[ -f "$BUILD_DIR/libs/$ABI/libhook.so" ]]; then
    mkdir -p "$REL_DIR/android-$ABI"; cp "$BUILD_DIR/libs/$ABI/libhook.so" "$REL_DIR/android-$ABI/hook_libretro.so";
  fi
done

# Copy .info
if [[ -f "$INFO_DIR/hook_libretro.info" ]]; then
  mkdir -p "$REL_DIR/info"; cp "$INFO_DIR/hook_libretro.info" "$REL_DIR/info/";
fi

# Copy sample patterns
mkdir -p "$REL_DIR/docs"; cp "$PATTERN_SAMPLE" "$REL_DIR/docs/";

cat <<EOF
Package layout generated under: $REL_DIR
EOF
