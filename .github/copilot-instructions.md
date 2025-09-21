# libretro-hookプロジェクトに関するGitHub Copilotの指示

## 指示の基本ルール

- 会話は日本語でしてください。
- gitのコミットログは英語で書いてください。

## プロジェクトの概要

- libretro-hookは、libretroコアの実装であり、他のlibretroコアをロードして実行するためのローダー機能を提供します。
- Windows(wincross64)、Linux(unix)、Android(arm64-v8a、armeabi-v7a)に向けたビルドをサポートしています。
- 現状Androidは子プロセスやインテントを用いたRetroArchの起動ができないため正常に動作しません。

## 開発指針

- libretroのAPIは`libretro.h`に定義されているので参照ください。
- libretro-commonを積極的に使用してポータビリティを確保してください。
- 重複するコードの記述を避け、共通化できる部分は共通化してください。
- コードのinclude関係で循環を避けるようにしてください。
- 関数の記述ファイルは、分かりやすいように再配置してください。
- 自明なコメントは記述しないでください。
- ファイル名、定数名、変数名、関数名などは一貫性を持たせてください。

## 設計

- 本コアはlibretroコアとして実装されており、RetroArchなどのlibretroフロントエンドからロードされます。
- 本コアはROMがロードされた際に、`before_load.sh`または`before_load.bat`を実行します。
- `before_load.sh`または`before_load.bat`では、ゲーム開始前に設定の変更など任意の処理を追加できます。
- `before_load.sh`または`before_load.bat`の標準出力で、使用するコアファイル名を`<core:mame_libretro.so>`のように出力すると、そのコアで選択したROMを`retroarch`コマンドを子プロセスで起動します。
- `retroarch`コマンドを子プロセスで起動を起動後は`exit(0)`で終了します。
- `<core:mame_libretro.so>`が出力されない場合は、ゲーム画面にスクリプトに渡した引数と標準出力と標準エラーが表示されます。

## ツール

- ndk-build : PATHが通っていない場合は`/opt/android-ndk-{VER}/ndk-build`を使用してください。
  `{VER}`はNDKのバージョンで、デフォルトで`r27c`を使用してください。
- バッチファイル(.bat)の動作検証は、まずWindowsの`cmd.exe`を使用して、無理ならば`wine`を使用してください。

## ビルド方法

- Windows用のビルドは`main/build/`ディレクトリで`make windows64`を実行してください。
- Android ARMv7アーキテクチャ用のビルドは`main/build/`ディレクトリで`make android32`を実行してください。
- Android ARMv8アーキテクチャ用のビルドは`main/build/`ディレクトリで`make android64`を実行してください。
- Linux用のビルドは`main/build/`ディレクトリで`make linux`を実行してください。
- ビルドの成果物は`main/generated/`ディレクトリにプラットフォーム別のディレクトリ配下に生成されます。

## テスト方法・問題調査方法

- `make`のビルドに失敗した場合は、`make clean`を実行してから再度ビルドを試みてください。
- コマンドを実行した場合、コマンドの戻り値を確認してください。

## 配布パッケージについて

- dist/release以下に配布用のバイナリと必要なファイルが配置されます。
- `packages.sh`で配布用のファイル一式を生成します。
