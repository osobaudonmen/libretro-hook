# Libretro Hook

[English README](README.md)

## 目次

- [概要](#概要)
- [サポートプラットフォーム](#サポートプラットフォーム)
- [ダウンロード](#ダウンロード)
- [ビルド方法（ローカル）](#ビルド方法ローカル)
- [使い方](#使い方)
- [スクリプト例](#スクリプト例)
- [開発方針](#開発方針)
- [貢献](#貢献)
- [参照](#参照)

## 概要

libretro-hook は、ゲーム開始前に任意のスクリプト（`before_load.sh` / `before_load.bat`）を実行できる libretro コアです。

主な用途:
- RetroArch や他の libretro フロントエンドからロードされる際に、ROM ごとに追加処理を行う（設定の切り替え、別プロセスの起動など）。
- サンプルスクリプトは MAME/FBNeo の麻雀タイトル向けに、ゲームごとのオーバーレイ設定を優先的に読み込むための処理を行います。

## サポートプラットフォーム

- Linux (unix)
- Windows (wincross64)

## ダウンロード

- Artifacts from `master` pushes:
  - `master` へ push すると CI が実行され、ビルド成果物がワークフローアーティファクトとしてアップロードされます。
  - GitHub Actions の該当ワークフロー実行ページから「Artifacts」を選び、`libretro-hook-release-linux` や `libretro-hook-release-windows` をダウンロードしてください。

- GitHub Releases (タグ付き):
  - `v0.1` のようなタグを push すると、ワークフローが GitHub Release を作成しプラットフォーム別の ZIP を添付します。
  - リポジトリの Releases ページからダウンロードしてください。

## ビルド方法

ビルドスクリプトは `main/build` にあります。代表的なターゲットは次のとおりです。

- Linux:
  - `cd main/build && make linux`
- Windows:
  - `cd main/build && make windows64`

成果物は `main/generated/` 配下のプラットフォーム別ディレクトリに作成されます。

## 使い方

1. RetroArch などのフロントエンドから本コアをロードし、ROM が指定されると、`before_load.sh`（Unix）または `before_load.bat`（Windows）がコアによって実行されます。
2. スクリプトはコマンドライン引数でRetroArchのSystemディレクトリとROMのフルパスが渡されます。
3. スクリプトの標準出力に以下の形式でコア名を出力すると、そのコアで選択した ROM を `retroarch` コマンドで子プロセスとして起動します。

```
<core:mame_libretro.so>
```

4. 上記の形式が出力された場合、libretro-hook は `retroarch` を子プロセスで起動します。
5. スクリプトが上記の出力を行わなかった場合、スクリプトに渡した引数や標準出力/標準エラーはゲーム画面上に表示されます。

