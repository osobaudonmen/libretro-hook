# libretro-hook

[Japanese README](README.ja.md)

Overview

libretro-hook is a RetroArch core that runs a script before a game starts. The bundled sample script demonstrates automatic overlay loading for Mahjong titles on MAME/FBNeo: https://github.com/osobaudonmen/retroarch_mahjong_overlays

Downloads

- Artifacts from `master` pushes: CI uploads workflow artifacts for pushes to `master`. Download the matching `libretro-hook-release-*` artifact from the GitHub Actions run.
- GitHub Releases (tagged): Pushing a tag (e.g. `v0.1`) creates a Release with platform ZIP files attached.

Installation

Unpack the downloaded archive and place the files into your RetroArch installation. Examples:

Windows example

```text
<RETROARCH_HOME>\cores\hook_libretro.dll
<RETROARCH_HOME>\info\hook_libretro.info
<RETROARCH_HOME>\system\hook\before_load.bat
```

Linux example

```text
<RETROARCH_HOME>/cores/hook_libretro.so
<RETROARCH_HOME>/info/hook_libretro.info
<RETROARCH_HOME>/system/hook/before_load.sh
```

After placing files, restart RetroArch or reload cores and verify operation.

Build

Build scripts are located in `main/build`. Typical targets:

- Linux: `cd main/build && make linux`
- Windows: `cd main/build && make windows64`

Build artifacts are written to `main/generated/`.

Usage

1. Load this core in RetroArch and select a ROM.
2. The core executes `before_load.sh` (Unix) or `before_load.bat` (Windows). The script receives the RetroArch System directory and the ROM full path as arguments.
3. If the script prints a core name to stdout in the following format, libretro-hook will launch `retroarch` as a child process to run that core with the same ROM:

```text
<core:mame_libretro.so>
```

4. If no `<core:...>` output is produced, the script's stdout/stderr will be shown on the game screen.

Verification

- Check RetroArch logs (e.g. `~/.config/retroarch/retroarch.log`) for script output and errors.
- Test scripts locally before deployment by running:

```bash
bash scripts/before_load.sh /path/to/system /path/to/rom
```

Development

- Follow the libretro API (`libretro.h`) when modifying the core.
- Prefer `libretro-common` for portability where possible.

Contributing

Issues and pull requests are welcome. Please write commit messages in English.
