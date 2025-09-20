#!/bin/sh

# Autoload Mahjong Overlays for MAME and FinalBurn Neo cores in RetroArch

# User configurable directories (uncomment and modify as needed)
# RETROARCH_HOME_DIR_OVERRIDE="/custom/path/to/retroarch"
# RETROARCH_CONFIG_DIR="/custom/path/to/config"
# RETROARCH_OVERLAY_DIR="/custom/path/to/overlays"

# Check arguments
if [ $# -ne 3 ]; then
    echo "Usage: $0 <retroarch_home_dir> <core_path> <rom_path>"
    exit 1
fi

RETROARCH_HOME_DIR="$1"
CORE_PATH="$2"
ROM_PATH="$3"

# Extract core name from core path
CORE_BASENAME=$(basename "$CORE_PATH")
CORE_NAME="${CORE_BASENAME%_libretro*}"

# Determine system name based on core name
case "$CORE_NAME" in
    "fbneo"|"fbneo2012")
        SYSTEM_NAME="FinalBurn Neo"
        ;;
    "mame"|"mame2003"|"mame2010")
        SYSTEM_NAME="MAME"
        ;;
    *)
        # Default system name based on core name
        SYSTEM_NAME="$CORE_NAME"
        ;;
esac

# Extract game name from ROM path (remove extension and path)
ROM_BASENAME=$(basename "$ROM_PATH")
GAME_NAME="${ROM_BASENAME%.*}"

# RetroArch home directory is now provided as argument
# But still allow override via environment variable
if [ -n "$RETROARCH_HOME_DIR_OVERRIDE" ]; then
    RETROARCH_HOME_DIR="$RETROARCH_HOME_DIR_OVERRIDE"
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
