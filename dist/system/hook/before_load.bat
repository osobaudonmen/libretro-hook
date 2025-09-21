@echo off
REM Autoload Mahjong Overlays for MAME and FinalBurn Neo cores in RetroArch

REM User configurable directories (uncomment and modify as needed)
REM set RETROARCH_HOME_DIR=C:\custom\path\to
REM set RETROARCH_CONFIG_DIR=C:\custom\path\to\config
REM set RETROARCH_OVERLAY_DIR=C:\custom\path\to\overlays

REM Check arguments
if "%~2"=="" (
    echo Usage: %0 ^<system_dir^> ^<rom_path^>
    exit /b 1
)

set SYSTEM_DIR=%~1
set ROM_PATH=%~2

REM Extract game name from ROM path (remove extension and path)
for %%i in ("%ROM_PATH%") do set ROM_BASENAME=%%~ni
set GAME_NAME=%ROM_BASENAME%

REM Get file extension
for %%i in ("%ROM_PATH%") do set ROM_EXT=%%~xi
set ROM_EXT=%ROM_EXT:~1%

REM Default core selection logic - modify as needed
if "%ROM_EXT%"=="zip" (
    set CORE_NAME=mame_libretro.dll
    set SYSTEM_NAME=MAME
) else if "%ROM_EXT%"=="7z" (
    set CORE_NAME=mame_libretro.dll
    set SYSTEM_NAME=MAME
) else (
    REM Default to MAME
    set CORE_NAME=mame_libretro.dll
    set SYSTEM_NAME=MAME
)

REM System directory is provided as first argument
REM Allow override via environment variable if needed
if not "%SYSTEM_DIR_OVERRIDE%"=="" (
    set SYSTEM_DIR=%SYSTEM_DIR_OVERRIDE%
)

if "%RETROARCH_HOME_DIR%"=="" (
    set RETROARCH_HOME_DIR=%SYSTEM_DIR%\..
)

REM Set config and overlay directories if not set
if "%RETROARCH_CONFIG_DIR%"=="" (
    set RETROARCH_CONFIG_DIR=%RETROARCH_HOME_DIR%\config
)

if "%RETROARCH_OVERLAY_DIR%"=="" (
    set RETROARCH_OVERLAY_DIR=%RETROARCH_HOME_DIR%\overlays
)

REM Create config directory if it doesn't exist
if not exist "%RETROARCH_CONFIG_DIR%\%SYSTEM_NAME%" (
    mkdir "%RETROARCH_CONFIG_DIR%\%SYSTEM_NAME%"
)

REM Determine overlay path based on game name
REM Use game name directly as overlay name
set OVERLAY_NAME=%GAME_NAME%

REM Create game-specific config file
set CONFIG_FILE=%RETROARCH_CONFIG_DIR%\%SYSTEM_NAME%\%GAME_NAME%.cfg
REM Convert to absolute path for overlay (absolute path)
for %%i in ("%RETROARCH_OVERLAY_DIR%\mahjong\mahjong_%OVERLAY_NAME%.cfg") do set OVERLAY_PATH=%%~fi

echo # Auto-generated overlay configuration for %GAME_NAME% > "%CONFIG_FILE%"
echo input_overlay = "%OVERLAY_PATH%" >> "%CONFIG_FILE%"
echo input_overlay_enable = "true" >> "%CONFIG_FILE%"

echo Created overlay configuration: %CONFIG_FILE%
echo System: %SYSTEM_NAME%
echo Overlay path: %OVERLAY_PATH%

REM Output core information for libretro-hook
echo ^<core:%CORE_NAME%^>
