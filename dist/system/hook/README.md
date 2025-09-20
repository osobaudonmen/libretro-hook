# `system/hook/`について

## 概要

本コアが使用する設定ファイルやスクリプトファイルが設置されています。

- `path_patterns.txt` :  使用するコアを判定するためのROMパスのフィルターリスト
- `before_load.sh` : MAME/FBNeoで麻雀ゲームのゲームごとの専用のオーバーレイファイルをロードするためにゲーム優先設定ファイルを生成するBashスクリプト
- `before_load.bat` : `before_load.sh`と同じ処理をするWindows用のバッチファイル

## テストについて

## 概要

- 対象スクリプトは`before_load.sh`と`before_load.bat`です。
- テストの実行は`/tmp/`の下にテスト用のディレクトリを作成して実行してください。
- スクリプトによって生成されるファイルの中身を確認してください。
- テストして問題がある場合は修正してください。
- テストが終わったらテスト用のディレクトリを削除してください。ただし、問題の解析に利用できそうなファイルがあれば削除せずに残してください。

## 確認するポイント

- `befre_load.sh fbneo_libretro.so fbneo_libretro.so /mnt/d/files/roms/rebuild/fbneo/1942.7z`のように第一引数のコアファイルはファイル名のみ、第二引数のROMパスは絶対パスで渡される。
- `<RETRO_ARCH_HOME>/system/hook/before_load.(sh|bat)`に設置される想定で`<RETRO_ARCH_HOME>/config/<CORE_NAME>/<GAME_NAME>.cfg`で出力される。
- `<RETRO_ARCH_HOME>/config/<CORE_NAME>/<GAME_NAME>.cfg`内の`input_overlay`は`<RETRO_ARCH_HOME>/overlays/mahjong/mahjong_<GAME_NAME>.cfg`のように絶対パスになる。
