
# Libretro Hook

[日本語](README.ja.md)

## Overview

`libretro-hook` is a core for RetroArch that can execute arbitrary scripts before starting a game.<br>
The included sample scripts automatically load RetroArch mahjong overlays for MAME/FBNeo ([retroarch_mahjong_overlays](https://github.com/osobaudonmen/retroarch_mahjong_overlays)).

## Downloads

Download `libretro-hook-release-*` from the corresponding GitHub Actions workflow run page.

## Installation

Unpack the downloaded archive and place the files into your RetroArch directory as shown below.

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

After placing the files, restart RetroArch or reload the cores and verify the behavior.

## Building

Build scripts are located in `main/build`. Typical targets:

- Linux
  - `cd main/build && make linux`
- Windows
  - `cd main/build && make windows64`

Artifacts are created under `main/generated/`.

## Usage

1. Load this core from RetroArch and select a ROM.
2. The core runs the script `before_load.sh` (Unix) or `before_load.bat` (Windows). The script receives the RetroArch System directory and the full path to the ROM as arguments.
3. If the script outputs a core name in the format `<core:mame_libretro.so>` to stdout, the specified core will be launched with the selected ROM.
4. If the above format is not output, the script's stdout and stderr are displayed on the game screen.


