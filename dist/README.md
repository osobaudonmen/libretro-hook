# Distribution Package Layout

`dist/package.sh`を実行すると`dist/release`以下に配布用のバイナリと必要なファイルが配置されます。

## 配布用のファイルの生成手順

- `dist/package.sh`で配布用のファイル一式を生成します。
- 生成時に`release/`は、一旦クリアしてください
- Linux, Windows, Androidのコアファイルはrelease向けでソースを再ビルドしてください
- `dist/release/cores/`以下にプラットフォーム別のディレクトリを作成し、各プラットフォーム用のバイナリを配置してください
- `main/info/`をコアのメタ情報として含めてください
- `dist/system/hook/`にコアから呼び出すスクリプト（`main/scripts/before_load.*`）をコピーしてください。

## `system/hook/`について

### 概要

本コアが使用する設定ファイルやスクリプトファイルが設置されています。

- `main/scripts/before_load.sh` : MAME/FBNeoで麻雀ゲームごとの専用のオーバーレイファイルをロードするためにゲーム優先設定ファイルを生成するBashスクリプト
- `main/scripts/before_load.bat` : `before_load.sh`と同じ処理をするWindows用のバッチファイル。

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
