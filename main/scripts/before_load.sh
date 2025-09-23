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

# Determine core based on ROM file path
if echo "$ROM_PATH" | grep -q "/mame/\|\\\\mame\\\\"; then
    CORE_FILE="mame_libretro.so"
    CORE_NAME="MAME"
elif echo "$ROM_PATH" | grep -q "/fbneo/\|\\\\fbneo\\\\"; then
    CORE_FILE="fbneo_libretro.so"
    CORE_NAME="FinalBurn Neo"
else
    # No core determined - exit without outputting core file
    exit 0
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
mkdir -p "$RETROARCH_CONFIG_DIR/$CORE_NAME"

# Create game-specific config file (absolute path)
CONFIG_FILE="$RETROARCH_CONFIG_DIR/$CORE_NAME/${GAME_NAME}.cfg"
OVERLAY_PATH="$RETROARCH_OVERLAY_DIR/mahjong/mahjong_${GAME_NAME}.cfg"

# Only create config file if it doesn't already exist
if [ ! -f "$CONFIG_FILE" ]; then
    cat > "$CONFIG_FILE" << EOF
# Auto-generated overlay configuration for $GAME_NAME
input_overlay = "$OVERLAY_PATH"
input_overlay_enable = "true"
EOF
    echo "Created config file: $CONFIG_FILE"
fi

# Output core information for libretro-hook
echo "<core:$CORE_FILE>"
