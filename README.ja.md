
# Libretro Hook

[English README](README.md)

## 概要

libretro-hook は、ゲーム開始前にスクリプトを実行できるRetroArch用のコアです。
付属のスクリプトのサンプルは MAME/FBNeo の麻雀タイトル向けオーバーレイの自動読み込みを行います。

## ダウンロード

- Artifacts from `master` pushes:
  - `master` へ push すると CI が実行され、ワークフローアーティファクトがアップロードされます。
  - GitHub Actions の該当ワークフロー実行ページから `libretro-hook-release-*` をダウンロードしてください。

- GitHub Releases (タグ付き):
  - タグ（例: `v0.1`）を push するとリリースが作成され、プラットフォーム別 ZIP が添付されます。

## インストール

ダウンロードしたアーカイブを解凍し、以下のように RetroArch のディレクトリへ配置してください。

Windows の例

```
<RETROARCH_HOME>\cores\hook_libretro.dll
<RETROARCH_HOME>\info\hook_libretro.info
<RETROARCH_HOME>\system\hook\before_load.bat
```

Linux の例

```
<RETROARCH_HOME>/cores/hook_libretro.so
<RETROARCH_HOME>/info/hook_libretro.info
<RETROARCH_HOME>/system/hook/before_load.sh
```

配置後、RetroArch を再起動するか、コアのリロードを行ってから動作を確認してください。

## ビルド方法

ビルドスクリプトは `main/build` にあります。代表的なターゲット:

- Linux
  - `cd main/build && make linux`
- Windows
  - `cd main/build && make windows64`

成果物は `main/generated/` 配下に作成されます。

## 使い方

1. RetroArch から本コアをロードし、ROM を指定します。
2. コアはスクリプト `before_load.sh`（Unix）または `before_load.bat`（Windows）を実行します。スクリプトには RetroArch の System ディレクトリと ROM のフルパスが引数として渡されます。
3. スクリプトの標準出力に次の形式でコア名を出力した場合、libretro-hook は `retroarch` を子プロセスとして起動します。
`<core:mame_libretro.so>`
4. 上記が出力されない場合、スクリプトの標準出力/標準エラーはゲーム画面に表示されます。
