# libretro-hookプロジェクトに関するGitHub Copilotの指示

## 指示の基本ルール

- 会話は日本語でしてください。
- gitのコミットログは英語で書いてください。

## プロジェクトの概要

- libretro-hookは、libretroコアの実装であり、他のlibretroコアをロードして実行するためのローダー機能を提供します。
- Windows、Linux、Androidに向けたビルドをサポートしています。

## 開発指針

- libretro-commonを積極的に使用してポータビリティを確保してください。

## ツール

- ndk-build : PATHが通っていない場合は`/opt/android-ndk-{VER}/ndk-build`を使用してください。
  `{VER}`はNDKのバージョンで、デフォルトで`r27c`を使用してください。

## ビルド方法

- Windows用のビルドは`src/build/`ディレクトリで`make platform=wincross64`を実行してください。
- Android ARMv7アーキテクチャ用のビルドは`src/build/`ディレクトリで`make platform=armeabi-v7a`を実行してください。
- Android ARMv8アーキテクチャ用のビルドは`src/build/`ディレクトリで`make platform=arm64-v8a`を実行してください。

