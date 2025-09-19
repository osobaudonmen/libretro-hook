# libretro-hookプロジェクトに関するGitHub Copilotの指示

## 指示の基本ルール

- 会話は日本語でしてください。
- gitのコミットログは英語で書いてください。

## プロジェクトの概要

- libretro-hookは、libretroコアの実装であり、他のlibretroコアをロードして実行するためのローダー機能を提供します。
- Windows、Linux、Androidに向けたビルドをサポートしています。

## 開発指針

- libretroのAPIは`libretro.h`に定義されているので参照ください。
- libretro-commonを積極的に使用してポータビリティを確保してください。
- 重複するコードの記述を避け、共通化できる部分は共通化してください。
- コードのinclude関係で循環を避けるようにしてください。
- 関数の記述ファイルは、分かりやすいように再配置してください。
- 自明なコメントは記述しないでください。
- ファイル名、定数名、変数名、関数名などは一貫性を持たせてください。

## ツール

- ndk-build : PATHが通っていない場合は`/opt/android-ndk-{VER}/ndk-build`を使用してください。
  `{VER}`はNDKのバージョンで、デフォルトで`r27c`を使用してください。

## ビルド方法

- Windows用のビルドは`src/build/`ディレクトリで`make platform=wincross64`を実行してください。
- Android ARMv7アーキテクチャ用のビルドは`src/build/`ディレクトリで`make platform=armeabi-v7a`を実行してください。
- Android ARMv8アーキテクチャ用のビルドは`src/build/`ディレクトリで`make platform=arm64-v8a`を実行してください。

## 問題調査方法

- ビルドに失敗した場合は、`make clean`を実行してから再度ビルドを試みてください。

## 配布パッケージについて

- dist/release以下に配布用のバイナリと必要なファイルが配置されます。
- `packages.sh`で配布用のファイル一式を生成します。
- 生成時に`release/`は、一旦クリアしてください
- Linux, Windows, Androidのコアファイルはrelease向けでソースを再ビルドしてください
- `dist/info/`をコアのメタ情報として含めてください
- `dist/system/hook/`をコアが使用する外部ファイルとして含めてください
