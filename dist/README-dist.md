# Distribution Package Layout

生成される配布物は `dist/release` 以下に配置されます。

```
dist/
  info/
    hook_libretro.info
  patterns/
    path_patterns.sample.txt
  release/
    unix/
      hook_libretro.so
    windows-x64/
      hook_libretro.dll
    android-armeabi-v7a/
      hook_libretro.so
    android-arm64-v8a/
      hook_libretro.so
    info/
      hook_libretro.info
    docs/
      path_patterns.sample.txt
```

## 使用手順
1. 各プラットフォーム用バイナリをビルド
2. `./dist/package.sh` を実行
3. `dist/release` 以下を配布用アーカイブにまとめる（例: `zip -r hook_core_release.zip dist/release/*`）

## path_patterns.txt の設置
RetroArch の system ディレクトリに `hook` フォルダを作成し、
`system/hook/path_patterns.txt` に以下のような内容を配置:

```
/mame/
/fbneo/
/arcade/
```

## 備考
- Android用は `libs/<abi>/libhook.so` から名称 `hook_libretro.so` に統一
- `.info` は配布アーカイブにも同梱されます
