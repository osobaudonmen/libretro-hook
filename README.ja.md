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


## インストール

ダウンロードしたアーティファクト（ZIP または `.so` / `.dll`）をプラットフォームに応じて配置します。以下は代表的な手順です。

- Linux (unix):
  - 配布 ZIP を解凍し、`hook_libretro.so` を任意のディレクトリに置きます。例えば:

    ```bash
    unzip libretro-hook-release-linux.zip
    sudo cp generated/unix/hook_libretro.so /usr/local/lib/libretro/hook_libretro.so
    ```

  - RetroArch のコア検索パス（通常は `~/.config/retroarch/cores/` や `/usr/local/lib/libretro/`）に配置するか、RetroArch 側で手動でコアを指定してください。

  - 検証: RetroArch から `hook_libretro.so` をロードし、ROM を起動して `before_load.sh` が実行されることを確認します。

- Windows (wincross64):
  - ZIP を解凍し、`hook_libretro.dll`（ファイル名は配布物に従う）を RetroArch のコアフォルダにコピーします。
  - `before_load.bat` を `scripts\` に置き、必要に応じて編集してください。
  - 検証: RetroArch を起動し、コアとして libretro-hook を選んで ROM をロードします。スクリプト出力が画面に表示されるか、指定のコア起動が行われることを確認します。

- Android (arm64/armv7) — 限定的サポート:
  - Android 用にビルドされた `.so` を対応する ABI ディレクトリ（例: `lib/arm64-v8a/`）に置き、APK に含めるか、RetroArch のコア格納場所に展開します。
  - 注意: Android では子プロセスの起動やインテントによる RetroArch 起動が制限される場合があり、期待通り動作しないことがあります。

### 検証手順（一般）

- ログ: RetroArch のログ（通常は `~/.config/retroarch/retroarch.log`）を確認して、`before_load.sh` / `before_load.bat` の出力やエラーを確認します。
- スクリプトの簡易チェック: 手元の端末でスクリプトを直接実行して返り値や標準出力を確認します。

```bash
bash scripts/before_load.sh /path/to/system /path/to/rom
```

上記が正しく動作すれば、RetroArch からの呼び出しでも同様の動作が期待できます。

