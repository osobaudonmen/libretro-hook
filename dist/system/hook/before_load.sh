#!/bin/sh

# Autoload Mahjong Overlays for MAME and FinalBurn Neo cores in RetroArch

# User configurable directories (uncomment and modify as needed)
# RETROARCH_HOME_DIR="/custom/path/to"
# RETROARCH_CONFIG_DIR="/custom/path/to/config"
# RETROARCH_OVERLAY_DIR="/custom/path/to/overlays"

# Check arguments
if [ $# -ne 2 ]; then
    echo "Usage: $0 <system_dir> <rom_path>"
    exit 1
fi

SYSTEM_DIR="$1"
ROM_PATH="$2"

# Extract game name from ROM path (remove extension and path)
ROM_BASENAME=$(basename "$ROM_PATH")
GAME_NAME="${ROM_BASENAME%.*}"

# Determine core based on ROM file extension or game name
ROM_EXT="${ROM_PATH##*.}"

# Default core selection logic - modify as needed
case "$ROM_EXT" in
    "zip"|"7z")
        # Assume MAME for compressed archives
        CORE_NAME="mame_libretro.so"
        SYSTEM_NAME="MAME"
        ;;
    *)
        # Default to MAME
        CORE_NAME="mame_libretro.so"
        SYSTEM_NAME="MAME"
        ;;
esac

# System directory is provided as first argument
# Allow override via environment variable if needed
if [ -n "$SYSTEM_DIR_OVERRIDE" ]; then
    SYSTEM_DIR="$SYSTEM_DIR_OVERRIDE"
fi

# Determine RetroArch home directory from system directory
if [ -z "$RETROARCH_HOME_DIR" ]; then
    RETROARCH_HOME_DIR="$(dirname "$SYSTEM_DIR")"
fi

# Set config and overlay directories if not set
if [ -z "$RETROARCH_CONFIG_DIR" ]; then
    RETROARCH_CONFIG_DIR="$RETROARCH_HOME_DIR/config"
fi

if [ -z "$RETROARCH_OVERLAY_DIR" ]; then
    RETROARCH_OVERLAY_DIR="$RETROARCH_HOME_DIR/overlays"
fi

# Create config directory if it doesn't exist
mkdir -p "$RETROARCH_CONFIG_DIR/$SYSTEM_NAME"

# Determine overlay path based on game name
# Use game name directly as overlay name
OVERLAY_NAME="$GAME_NAME"

# Create game-specific config file (absolute path)
CONFIG_FILE="$RETROARCH_CONFIG_DIR/$SYSTEM_NAME/${GAME_NAME}.cfg"
OVERLAY_PATH="$RETROARCH_OVERLAY_DIR/mahjong/mahjong_${OVERLAY_NAME}.cfg"

cat > "$CONFIG_FILE" << EOF
# Auto-generated overlay configuration for $GAME_NAME
input_overlay = "$OVERLAY_PATH"
input_overlay_enable = "true"
EOF

echo "Created overlay configuration: $CONFIG_FILE"
echo "System: $SYSTEM_NAME"
echo "Overlay path: $OVERLAY_PATH"

# Output core information for libretro-hook
echo "<core:$CORE_NAME>"
