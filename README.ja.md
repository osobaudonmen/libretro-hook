# Libretro Hook

[English](README.md)

## 概要

`libretro-hook` は、ゲーム開始前に任意のスクリプトを実行できる RetroArch 用のコアです。<br>
付属のサンプルスクリプトは、MAME/FBNeo 用の RetroArch 麻雀オーバーレイ
([retroarch_mahjong_overlays](https://github.com/osobaudonmen/retroarch_mahjong_overlays)) を自動で読み込みます。

## ダウンロード

GitHubのReleaseページからファイルをダウンロードしてください。

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

配置後は RetroArch を再起動するか、コアを再読み込みして動作を確認してください。

## ビルド方法

ビルドスクリプトは `main/build` にあります。代表的なターゲット:

- Linux
  - `cd main/build && make linux`
- Windows
  - `cd main/build && make windows64`

成果物は `main/generated/` 配下に作成されます。

## 使い方

1. RetroArch から本コアをロードし、ROM を指定します。
2. コアがスクリプト `before_load.sh`（Unix）または `before_load.bat`（Windows）を実行します。スクリプトには RetroArch の System ディレクトリと ROM のフルパスが引数として渡されます。
3. スクリプトの標準出力で `<core:mame_libretro.so>` の形式でコア名を出力すると、そのコアで選択した ROM を起動します。
4. 上記の形式が出力されない場合、スクリプトの標準出力・標準エラーはゲーム画面に表示されます。

## サンプルスクリプトについて

付属のサンプルスクリプト（`before_load.sh` / `before_load.bat`）は、MAME/FBNeo の麻雀ゲーム用オーバーレイを自動適用する機能を提供します。

### 主な機能

- ROM パスから使用するコア（MAME または FBNeo）を自動判別
- ゲーム名に対応した専用オーバーレイ設定ファイルを自動生成
- 既存の設定ファイルがある場合は上書きしない（設定を保護）

### 対応ゲーム判別方法

スクリプトは ROM ファイルパスに含まれる文字列で使用コアを判別します：

- パスに `/mame/` または `\mame\` を含む場合 → MAME コア
- パスに `/fbneo/` または `\fbneo\` を含む場合 → FBNeo コア
- 判別できない場合 → エラーメッセージを表示

### 設定ファイルの配置

生成されるゲーム別設定ファイル：
```
<RetroArch設定ディレクトリ>/<コア名>/<ゲーム名>.cfg
```

オーバーレイファイルの参照先：
```
<RetroArchホームディレクトリ>/overlays/mahjong/mahjong_<ゲーム名>.cfg
```

### 注意点

- オーバーレイファイル（`mahjong_*.cfg`）は別途 [retroarch_mahjong_overlays](https://github.com/osobaudonmen/retroarch_mahjong_overlays) から入手する必要があります
- スクリプトの先頭でディレクトリパスをハードコーディング設定可能（環境に応じて調整してください）
- 既存のゲーム別設定ファイルは上書きされません（手動設定を保護するため）
