# libretro-hookプロジェクトに関するGitHub Copilotの指示

## 基本ルール

- 会話は日本語でしてください。

## ツール

- ndk-build : PATHが通っていない場合は`/opt/android-ndk-{VER}/ndk-build`を使用してください。
  `{VER}`はNDKのバージョンで、デフォルトで`r27c`を使用してください。

## ビルド方法

- Windows用のビルドは`src/build/`ディレクトリで`make platform=wincross64`を実行してください。
- Android ARMv7アーキテクチャ用のビルドは`src/build/`ディレクトリで`make platform=armeabi-v7a`を実行してください。
- Android ARMv8アーキテクチャ用のビルドは`src/build/`ディレクトリで`make platform=arm64-v8a`を実行してください。

