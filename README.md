# Libretro Hook

A libretro core implementation with loader functionality.

## Project Structure

```
libretro-hook/
├── src/                     # Main source code (previously build/make/)
│   ├── core/               # Core libretro implementation
│   │   ├── libretro.c      # Main libretro core
│   │   ├── libretro.h      # Core header
│   │   ├── core_loader.c   # Core loader functionality
│   │   ├── core_loader.h   # Core loader header
│   │   ├── settings.c      # Settings management
│   │   └── settings.h      # Settings header
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

This will create `sample_libretro.so` in the build directory.

### Build Options

- `DEBUG=1` - Build with debug symbols
- `platform=<platform>` - Target specific platform (unix, osx, windows, etc.)

## Features

- Full libretro core implementation
- Core loader functionality for launching other libretro cores
- Cross-platform support
- Comprehensive build system supporting multiple platforms
- Example implementations for various libretro features

## Examples

The `examples/` directory contains various sample implementations:

- **audio/**: Audio processing examples
- **video/**: Video rendering examples (OpenGL, software, Vulkan)
- **input/**: Input handling examples
- **midi/**: MIDI functionality examples
- **tests/**: Test cores and debugging utilities

## License

See `license` file for details.