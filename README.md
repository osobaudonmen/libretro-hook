# libretro-hook

[日本語 README](README.ja.md)

Libretro Hook is a libretro core that runs an arbitrary script (`before_load.sh` / `before_load.bat`) before a game starts.
Main use cases:
- Run per-ROM preparatory tasks when loaded by a libretro frontend (switch config, launch external process, etc.).
- The provided sample script demonstrates prioritizing per-game overlays for Mahjong arcade titles (MAME/FBNeo).
Reference:
- https://github.com/osobaudonmen/retroarch_mahjong_overlays
Supported platforms
- Linux (unix)
- Windows (wincross64)
- Android (arm64-v8a, armeabi-v7a) — Note: Android currently has limitations around spawning child processes or launching RetroArch via intents; functionality may be limited.
Downloads

- Artifacts from `master` pushes:
  - CI runs when `master` receives a push. Build artifacts are uploaded as workflow artifacts.
- GitHub Releases (tagged):
  - Pushing a tag like `v0.1` triggers a workflow that creates a GitHub Release and attaches per-platform ZIPs.
  - Download releases from: `https://github.com/osobaudonmen/libretro-hook/releases`
Building locally

Build scripts are in `main/build`. Typical targets:
- Linux:
  - `cd main/build && make linux`
- Windows (cross-build):
  - `cd main/build && make windows64`
- Android (ARMv7):
  - `cd main/build && make android32`
- Android (ARM64):
  - `cd main/build && make android64`
Artifacts are generated under `main/generated/` or `generated/` in platform-specific subdirectories.

Android builds require the Android NDK. If `ndk-build` is not on `PATH`, use `/opt/android-ndk-{VER}/ndk-build` (default: `r27c`).
If a build fails, try `make clean` and build again.

Usage (how it works)
1. When this core is loaded by a libretro frontend and a ROM is provided, the core runs `before_load.sh` (Unix) or `before_load.bat` (Windows).
2. The script receives command-line arguments and may write to stdout/stderr.
3. If the script prints a core name in the following format to stdout, libretro-hook will launch `retroarch` as a child process to run the selected core with the same ROM:
```
<core:mame_libretro.so>
```
4. When that output is produced, libretro-hook spawns `retroarch` and then exits with `exit(0)`.
5. If the script does not produce the `<core:...>` output, the script's arguments and stdout/stderr are shown on the game screen.

Examples
- Edit `scripts/before_load.sh` to suit your environment — an example is included in the `scripts/` directory.

Development guidelines
- Follow the libretro API (`libretro.h`) for core implementation.
- Prefer using `libretro-common` for portability.
- Remove duplicate, unused, or noisy logging.

Contributing
- Issues and pull requests are welcome.
- Please write commit messages in English.

See `README.ja.md` for the Japanese README.
# Libretro Hook

## EN

Libretro Hook is a libretro core that runs an arbitrary script before a game starts. It can be used to perform preparatory tasks such as changing configuration or launching a separate frontend with a selected core.

A sample script is provided that prioritizes loading per-game overlays for Mahjong arcade titles in MAME/FBNeo.

## Ref

[RetroArch Mahjong Overlay](https://github.com/osobaudonmen/retroarch_mahjong_overlays)

## Downloads

- Artifacts from master pushes:
  - CI runs on pushes to `master`. Build artifacts are uploaded as workflow artifacts.
  - To download the latest artifact produced by a `master` push, go to the repository's GitHub Actions page, select the most recent `Build and Package Release (Linux + Windows)` run, then open the "Artifacts" panel and download `libretro-hook-release-linux` or `libretro-hook-release-windows`.

- GitHub Releases (tagged):
  - When a tag like `v0.1` is pushed, the workflow creates a GitHub Release and attaches per-platform ZIP files.
  - Go to the repository's Releases page (`https://github.com/osobaudonmen/libretro-hook/releases`) and download the attached ZIP for the desired platform.

(See `README.ja.md` for Japanese.)
