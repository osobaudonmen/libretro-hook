# Libretro Hook

A libretro core implementation with loader functionality.

## Project Structure

```
libretro-hook/
├── src/                     # Main source code
│   ├── core/               # Core libretro implementation
│   │   ├── libretro.c      # Main libretro core implementation
│   │   ├── hook_constants.h # Project constants
│   │   ├── hook_globals.h  # Global variable declarations
│   │   ├── core_loader.c   # Core loader functionality
│   │   ├── core_loader.h   # Core loader header
│   │   ├── core_discovery.c # Core discovery functionality
│   │   ├── core_discovery.h # Core discovery header
│   │   ├── core_options.c  # Core options management
│   │   ├── core_options.h  # Core options header
│   │   ├── platform_loader.c # Platform-specific core loading
│   │   ├── platform_loader.h # Platform loader header
│   │   ├── platform_utils.c # Platform utilities
│   │   ├── platform_utils.h # Platform utilities header
│   │   └── path_patterns.txt # Path pattern configuration
│   ├── external/           # External libraries
│   │   └── libretro-common/ # Common libretro utilities
│   ├── build/              # Build system
│   │   ├── Makefile        # Main makefile
│   │   ├── Makefile.common # Common build definitions
│   │   ├── link.T          # Linker script
│   │   └── jni/            # Android build support
│   └── generated/          # Generated files (build artifacts)
├── examples/               # Example implementations and tests
│   ├── audio/              # Audio examples
│   ├── video/              # Video examples
│   ├── input/              # Input examples
│   ├── midi/               # MIDI examples
│   └── tests/              # Test implementations
├── docs/                   # Documentation
└── scripts/                # Utility scripts and language samples
    └── lang/               # Language-specific implementations
        ├── freebasic/      # FreeBASIC samples
        └── pascal/         # Pascal samples
```

## Building

To build the project:

```bash
cd src/build
make
```

This will create `hook_libretro.so` in the build directory.

### Build Options

- `DEBUG=1` - Build with debug symbols
- `platform=<platform>` - Target specific platform (unix, osx, windows, etc.)

### Supported Platforms

- **Linux/Unix**: `make platform=unix`
- **Windows**: `make platform=wincross64`
- **Android ARMv7**: `make platform=armeabi-v7a`
- **Android ARMv8**: `make platform=arm64-v8a`

## Features

- Full libretro core implementation with hook functionality
- Dynamic core loader for launching other libretro cores
- Cross-platform support (Linux, Windows, Android)
- Platform-specific core loading (RetroArch, custom commands)
- Core discovery with path pattern matching
- Comprehensive build system supporting multiple architectures
- Modular architecture with separate components for different functionalities

## Configuration

- Path pattern options for core selection are provided as choices.
- Choices are loaded from a UTF-8 text file named `path_patterns.txt`.
- Each line defines one choice. Leading/trailing spaces are trimmed. `#` starts a comment line.
- Default choices when file is missing/empty: `/mame/`, `/fbneo/`, `/arcade/`.

### Location of `path_patterns.txt`

- RetroArch System Directory under `hook/`:
    - `<system>/hook/path_patterns.txt`
    - RetroArch: Settings > Directory > System で `system` を確認

## Examples

The `examples/` directory contains various sample implementations:

- **audio/**: Audio processing examples
- **video/**: Video rendering examples (OpenGL, software, Vulkan)
- **input/**: Input handling examples
- **midi/**: MIDI functionality examples
- **tests/**: Test cores and debugging utilities

## License

See `LICENSE` for details.
