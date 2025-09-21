# Libretro Hook

[English README](README.md)

## 概要

libretro-hook は、ゲーム開始前にスクリプトを実行するための RetroArch 用コアです。


付属スクリプトは MAME/FBNeo の麻雀タイトル向けにオーバーレイを自動で読み込むサンプルです。

## サポートプラットフォーム

- Linux
- Windows

## ダウンロード

- Artifacts from `master` pushes: GitHub Actions のワークフローアーティファクトをダウンロードしてください（例: `libretro-hook-release-linux`）。
- GitHub Releases: タグ付きリリース（例: `v0.1`）にプラットフォーム別 ZIP が添付されます。

## インストール

ダウンロードしたアーカイブを解凍し、RetroArch のディレクトリ構成に合わせてファイルを配置してください。

Windows

```text
<RETROARCH_HOME>\cores\hook_libretro.dll
<RETROARCH_HOME>\info\hook_libretro.info
<RETROARCH_HOME>\system\hook\before_load.bat
```

Linux

```text
<RETROARCH_HOME>/cores/hook_libretro.so
<RETROARCH_HOME>/info/hook_libretro.info
<RETROARCH_HOME>/system/hook/before_load.sh
```

配置後は RetroArch を再起動するか、コアを再読み込みして動作を確認してください。

検証のヒント:
- RetroArch のログ（`~/.config/retroarch/retroarch.log` など）でスクリプトの出力やエラーを確認する。
- スクリプトはローカルで引数を与えて実行し、期待した動作を確認してから配置する。

## ビルド方法

ビルドスクリプトは `main/build` にあります。代表的なターゲット:

- Linux
  - `cd main/build && make linux`
- Windows
  - `cd main/build && make windows64`

ビルド成果物は `main/generated/` 配下に出力されます。

## 使い方

1. RetroArch で本コアをロードし、ROM を指定します。
2. コアは `before_load.sh`（Unix）または `before_load.bat`（Windows）を実行します。スクリプトには RetroArch の System ディレクトリと ROM のフルパスが引数として渡されます。
3. スクリプトが標準出力に次の形式でコア名を出力した場合、libretro-hook は `retroarch` を子プロセスとして起動します。

```text
<core:mame_libretro.so>
```

4. 上記が出力されない場合、スクリプトの標準出力/標準エラーはゲーム画面に表示されます。



