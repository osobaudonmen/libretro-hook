

# Libretro Hook

[日本語](README.ja.md)

## Overview

`libretro-hook` is a libretro core for RetroArch that runs a script before starting a game.<br>
The included sample scripts automatically load overlays for MAME/FBNeo used by the RetroArch Mahjong overlays project: https://github.com/osobaudonmen/retroarch_mahjong_overlays

## Downloads

- Artifacts from `master` pushes:
  - Pushing to `master` triggers CI and uploads workflow artifacts.
  - Download the artifact named `libretro-hook-release-*` from the corresponding GitHub Actions workflow run page.

- GitHub Releases (tagged):
  - Pushing a tag (for example `v0.1`) creates a release and attaches platform-specific ZIP archives.

## Installation

Unpack the archive you downloaded and place files into your RetroArch tree as shown below.

Windows example

```
<RETROARCH_HOME>\cores\hook_libretro.dll
<RETROARCH_HOME>\info\hook_libretro.info
<RETROARCH_HOME>\system\hook\before_load.bat
```

Linux example

```
<RETROARCH_HOME>/cores/hook_libretro.so
<RETROARCH_HOME>/info/hook_libretro.info
<RETROARCH_HOME>/system/hook/before_load.sh
```

After copying the files, restart RetroArch or reload cores and verify the behavior.

## Building

Build scripts live in `main/build`. Typical targets:

- Linux
  - `cd main/build && make linux`
- Windows
  - `cd main/build && make windows64`

Artifacts are created under `main/generated/`.

## Usage

1. Load this core from RetroArch and select a ROM.
2. The core runs the script `before_load.sh` (Unix) or `before_load.bat` (Windows). The script receives the RetroArch System directory and the full path to the ROM as arguments.
3. If the script writes a core name to stdout in the format `<core:mame_libretro.so>`, the core specified will be launched with the selected ROM.
4. If the script does not output a `<core:...>` line, the script's stdout and stderr are shown on the game screen.


