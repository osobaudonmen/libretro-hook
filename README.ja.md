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
