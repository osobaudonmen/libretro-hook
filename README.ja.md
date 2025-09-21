# Libretro Hook (日本語)

ゲーム開始前に任意のスクリプトを実行するための libretro コアです。

サンプルスクリプトは MAME/FBNeo 向けに麻雀ゲームのオーバーレイ設定を優先してロードするためのものです。

## ダウンロード

- master への push によるアーティファクト:
  - `master` への push で CI が実行され、ビルド成果物がワークフローアーティファクトとしてアップロードされます。
  - 最新のアーティファクトは GitHub Actions ページ内の該当ワークフロー実行から「Artifacts」を選んでダウンロードしてください（例: `libretro-hook-release-linux` や `libretro-hook-release-windows`）。

- GitHub Releases (タグ付き):
  - `v0.1` のようなタグを push すると、ワークフローが GitHub Release を作成し、プラットフォーム別 ZIP を添付します。
  - リポジトリの Releases ページで該当するリリースの添付ファイルをダウンロードしてください。

(注) CI による自動公開を行っています。README は英語版の `README.md` を参照してください。
