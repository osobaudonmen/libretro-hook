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
