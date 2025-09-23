# libretro-hookプロジェクトに関するGitHub Copilotの指示

## 指示の基本ルール

- 日本語で会話してください。
- gitのコミットログは英語で書いてください。

## プロジェクトの概要

- libretro-hookは、libretroコアの実装であり、他のlibretroコアをロードして実行するためのローダー機能を提供します。
- Windows(wincross64)、Linux(unix)、Android(arm64-v8a、armeabi-v7a)に向けたビルドをサポートしています。
- 現状Androidは子プロセスやインテントを用いたRetroArchの起動ができないため正常に動作しません。配布用のバイナリやアセットやダウンロードからは除外してください。

## 開発指針

- libretroのAPIは`libretro.h`に定義されているので参照ください。
- libretro-commonを積極的に使用してポータビリティを確保してください。
- 重複するコードの記述を避け、共通化できる部分は共通化してください。
- コードのinclude関係で循環を避けるようにしてください。
- 関数の記述ファイルは、分かりやすいように再配置してください。
- 自明なコメントは記述しないでください。
- コードの変更や削除の記録としてのコメントは記述しないでください。
- ファイル名、定数名、変数名、関数名などは一貫性を持たせてください。
- ログ出力の書式と文体は一貫性を持たせてください。
- 重複する無駄なログ出力は削除してください。
- 意味のないログ出力は削除してください。
- 未使用かつ今後の再利用性の低いコードは削除してください。
- 未使用かつ今後の再利用性の低いファイルは削除してください。

## コアの設計

- 本コアはlibretroコアとして実装されており、RetroArchなどのlibretroフロントエンドからロードされます。
- 本コアはROMがロードされた際に、`before_load.sh`または`before_load.bat`を実行します。
- `before_load.sh`または`before_load.bat`では、ゲーム開始前に設定の変更など任意の処理を追加できます。
- `before_load.sh`または`before_load.bat`の標準出力で、使用するコアファイル名を`<core:mame_libretro.so>`のように出力すると、そのコアで選択したROMを`retroarch`コマンドを子プロセスで起動します。
- `retroarch`コマンドを子プロセスで起動を起動後は`exit(0)`で終了します。
- スクリプトに渡した引数と標準出力と標準エラーはRetroArchのログに出力されます。
- `<core:mame_libretro.so>`が出力されない場合は、スクリプトに渡した引数と標準出力と標準エラーが、ゲーム画面にも表示されます。

## 開発ツール

- ndk-build : PATHが通っていない場合は`/opt/android-ndk-{VER}/ndk-build`を使用してください。
  `{VER}`はNDKのバージョンで、デフォルトで`r27c`を使用してください。
- バッチファイル(.bat)の動作検証は、`cmd.exe`または`wine`を使用してください。

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
- `dist/package.sh`を実行すると`dist/release`以下に配布用のバイナリと必要なファイルが配置されます。

### 配布用のファイルの生成手順

- `dist/package.sh`で配布用のファイル一式を生成します。
- 生成時に`release/`は、一旦クリアしてください
- Linux, Windows, Androidのコアファイルはrelease向けでソースを再ビルドしてください
- `dist/release/cores/`以下にプラットフォーム別のディレクトリを作成し、各プラットフォーム用のバイナリを配置してください
- `main/info/`をコアのメタ情報として含めてください
- `dist/system/hook/`にコアから呼び出すスクリプト（`main/scripts/before_load.*`）をコピーしてください。

## サンプルスクリプトについて

ゲームが開始される前に実行されるスクリプトのサンプルを提供しています。
サンプルでは、MAME/FBNeo用のRetroArch麻雀オーバーレイを自動でロードするために、ゲーム優先の設定ファイルを自動生成します。

- `main/scripts/before_load.sh` : MAME/FBNeoで麻雀ゲームごとの専用のオーバーレイファイルをロードするためにゲーム優先設定ファイルを生成するBashスクリプト
- `main/scripts/before_load.bat` : `before_load.sh`と同じ処理をするWindows用のバッチファイル。
- `<RETROARCH_HOME_DIR>/system/hook/`に設置します。

### `before_load.sh`の設計

- スクリプトの大雑把な責務を処理順に列挙します。
  - ROMファイルパスから対応するコアとゲームを判別する。
  - ゲームに対応するオーバーレイを設定ファイルを生成する
  - 対応コア情報を標準出力に出力する
- スクリプトは第一引数にRetroArchのSystemディレクトリのフルパス、第二引数にROMファイルのフルパスを受け取ります。
- ROMファイル名から拡張子を除いたらゲーム名になります。
- コアファイルや`CORE_NAME`は`main/external/libretro-core-info/*.info`を参考にして決定します。
  - `.info`ファイルのパスからコアのファイルパスを決定します。例: `mame_libretro.info`ならば`mame_libretro.so`になります。
  - `.info`ファイルの内容から`CORE_NAME`を決定します。`.info`ファイル内の`corename`が`CORE_NAME`になります。
- Systemディレクトリは、ゲーム別の設定ファイルを保存する場所を決定するために使用します。多くの場合、`<Systemディレクトリ>/../config/<CORE_NAME>/<ゲーム名>.cfg`がゲーム別の設定ファイルパスになります。
- 使用するコアが決定した場合には、標準出力に`<core:コアファイル>`の形式で出力します。
- 使用するコアを判定する方法はROMファイルパスに含まれる文字列で判定します。次に例を示します。
  - `/mame/`や`\mame\`を含む場合、MAMEを使用する
  - `/fbneo/`や`\fbneo\`を含む場合、FBNeoを使用する
  - パスから判定できなかった場合はあきらめて、`<core:コアファイル>`を出力しない。
- スクリプトの先頭にハードコーディングで、`RETROARCH_HOME_DIR`, `RETROARCH_CONFIG_DIR`, `RETROARCH_OVERLAY_DIR`が指定できます。
  - `RETROARCH_HOME_DIR`は`RETROARCH_CONFIG_DIR`と`RETROARCH_OVERLAY_DIR`が未指定の場合に使用します。
    - RETROARCH_CONFIG_DIR = RETROARCH_HOME_DIR/config
    - RETROARCH_OVERLAY_DIR = RETROARCH_HOME_DIR/overlays
  - `RETROARCH_CONFIG_DIR`はゲーム別の設定ファイルの格納先のベースディレクトリになります。
    - ゲーム別の設定ファイルパスは`RETROARCH_CONFIG_DIR/<CORE_NAME>/<ゲーム名>.cfg`になります。
  - `RETROARCH_OVERLAY_DIR`はオーバーレイのベースディレクトリになります。
    - ゲーム別のオーバーレイパスは`RETROARCH_OVERLAY_DIR/mahjong/mahjong_<ゲーム名>.cfg`（今回は麻雀用）になります。

### テストについて

#### 概要

- 対象スクリプトは`before_load.sh`と`before_load.bat`です。
- テストは`/tmp/`の下に、RetroArchのディレクトリ構造を模倣してテスト用ディレクトリを作成して実行してください。
- スクリプトによって生成されるファイルの中身を確認してください。
- テストして問題がある場合は修正してください。
- テストが終わったらテスト用のディレクトリを削除してください。ただし、問題の解析に利用できそうなファイルがあれば削除せずに残してください。

#### 確認するポイント

- `before_load.sh /home/user/.config/retroarch/system /mnt/d/files/roms/rebuild/fbneo/1942.7z`のように、第一引数はRetroArchのsystemディレクトリ、第二引数はROMパスの絶対パスで渡される。
- スクリプトは最後に`<core:fbneo_libretro.so>`のような形式でコアファイルを出力する必要がある。
- スクリプトは`<RETROARCH_HOME_DIR>/system/hook/`に設置され、`<RETROARCH_HOME_DIR>/config/<CORE_NAME>/<GAME_NAME>.cfg`を生成する
- `<RETROARCH_HOME_DIR>/config/<CORE_NAME>/<GAME_NAME>.cfg`内の`input_overlay`は`<RETROARCH_HOME_DIR>/overlays/mahjong/mahjong_<GAME_NAME>.cfg`のように絶対パスになる。

## READMEについて

### 更新方針

- 自動で章を追加しないでください。
- 記載されてる内容を改善・最新化してください。
- 正確なMarkdownの構文で記述してください。

### 多言語対応

- 英語版は`README.md`、日本語版は`README.ja.md`です。
- 日本語版をそのまま翻訳して英語版を作成してください。章を増やしたりしないでください。
- 先頭部分に各言語へのリンクを設置してください。
