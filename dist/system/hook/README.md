# `system/hook/`について

## 概要

本コアが使用する設定ファイルやスクリプトファイルが設置されています。

- `path_patterns.txt` :  使用するコアを判定するためのROMパスのフィルターリスト
- `before_load.sh` : MAME/FBNeoで麻雀ゲームごとの専用のオーバーレイファイルをロードするためにゲーム優先設定ファイルを生成するBashスクリプト
- `before_load.bat` : `before_load.sh`と同じ処理をするWindows用のバッチファイル

## `before_load.sh`の設計

- スクリプトの大雑把な責務を処理順に列挙します。
  - ROMファイルパスから対応するコアとゲームを判別する。
  - ゲームに対応するオーバーレイを設定ファイルを生成する
  - 対応コア情報を標準出力に出力する
- スクリプトは第一引数にRetroArchのSystemディレクトリのフルパス、第二引数にROMファイルのフルパスを受け取ります。
- ROMファイル名から拡張子を除いたらゲーム名になります。
- Systemディレクトリは、ゲーム別の設定ファイルを保存する場所を決定するために使用します。多くの場合、`<Systemディレクトリ>/../config/<コア名>/<ゲーム名>.cfg`がゲーム別の設定ファイルパスになります。
- 使用するコアが決定した場合には、標準出力に`<core:コアファイル>`を出力します。
- 使用するコアを判定する方法はROMファイルパスに含まれる文字列で判定します。次に例を示します。
  - `/mame/`や`\mame\`を含む場合、MAMEを使用する
  - `/fbneo/`や`\fbneo\`を含む場合、FBNeoを使用する
  - パスから判定できなかった場合はあきらめて、`<core:コアファイル>`を出力しない。
- コアのファイルパスやコア名は`https://github.com/libretro/libretro-core-info`を参考にします。
  - `.info`ファイルのパスからコアのファイルパスを決定します。例: `mame_libretro.info`ならば`mame_libretro.so`になります。
  - `.info`ファイルの内容からコア名を決定します。`.info`ファイル内の`corename`がコア名になります。
- スクリプトの先頭にハードコーディングするか環境変数で、`RETROARCH_HOME_DIR`, `RETROARCH_CONFIG_DIR`, `RETROARCH_OVERLAY_DIR`が指定できます。
  - `RETROARCH_HOME_DIR`は`RETROARCH_CONFIG_DIR`と`RETROARCH_OVERLAY_DIR`が未指定の場合に使用します。
    - RETROARCH_CONFIG_DIR = RETROARCH_HOME_DIR/config
    - RETROARCH_OVERLAY_DIR = RETROARCH_HOME_DIR/overlays
  - `RETROARCH_CONFIG_DIR`はゲーム別の設定ファイルの格納先のベースディレクトリになります。
    - ゲーム別の設定ファイルパスは`RETROARCH_CONFIG_DIR/<コア名>/<ゲーム名>.cfg`になります。
  - `RETROARCH_OVERLAY_DIR`はオーバーレイのベースディレクトリになります。
    - ゲーム別の麻雀ファイルのオーバーレイパスは`RETROARCH_OVERLAY_DIR/mahjong/mahjong_<ゲーム名>.cfg`になります。

## テストについて

### 概要

- 対象スクリプトは`before_load.sh`と`before_load.bat`です。
- テストは`/tmp/`の下に、RetroArchのディレクトリ構造を模倣してテスト用ディレクトリを作成して実行してください。
- スクリプトによって生成されるファイルの中身を確認してください。
- テストして問題がある場合は修正してください。
- テストが終わったらテスト用のディレクトリを削除してください。ただし、問題の解析に利用できそうなファイルがあれば削除せずに残してください。

### 確認するポイント

- `before_load.sh /home/user/.config/retroarch/system /mnt/d/files/roms/rebuild/fbneo/1942.7z`のように、第一引数はRetroArchのsystemディレクトリ、第二引数はROMパスの絶対パスで渡される。
- スクリプトは最後に`<core:mame_libretro.so>`のような形式でコア名を出力する必要がある。
- `<RETRO_ARCH_HOME>/system/hook/before_load.(sh|bat)`に設置される想定で`<RETRO_ARCH_HOME>/config/<CORE_NAME>/<GAME_NAME>.cfg`で出力される。
- `<RETRO_ARCH_HOME>/config/<CORE_NAME>/<GAME_NAME>.cfg`内の`input_overlay`は`<RETRO_ARCH_HOME>/overlays/mahjong/mahjong_<GAME_NAME>.cfg`のように絶対パスになる。
