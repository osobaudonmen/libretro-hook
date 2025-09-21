@echo off
REM Autoload Mahjong Overlays for MAME and FinalBurn Neo cores in RetroArch

REM Enable delayed expansion to allow safe substring operations inside blocks
setlocal enabledelayedexpansion

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

REM Determine core based on ROM file path
if not "%ROM_PATH:\mame\=%"=="%ROM_PATH%" (
    set CORE_FILE=mame_libretro.dll
    set CORE_NAME=MAME
    goto core_determined
)

if not "%ROM_PATH:/mame/=%"=="%ROM_PATH%" (
    set CORE_FILE=mame_libretro.dll
    set CORE_NAME=MAME
    goto core_determined
)

if not "%ROM_PATH:\fbneo\=%"=="%ROM_PATH%" (
    set CORE_FILE=fbneo_libretro.dll
    set CORE_NAME=FinalBurn Neo
    goto core_determined
)

if not "%ROM_PATH:/fbneo/=%"=="%ROM_PATH%" (
    set CORE_FILE=fbneo_libretro.dll
    set CORE_NAME=FinalBurn Neo
    goto core_determined
)

REM No core determined - exit without outputting core file
exit /b 0

:core_determined

if "%RETROARCH_HOME_DIR%"=="" (
    for %%i in ("%SYSTEM_DIR%") do set RETROARCH_HOME_DIR=%%~dpi
    set "RETROARCH_HOME_DIR=!RETROARCH_HOME_DIR:~0,-1!"
)

REM Set config and overlay directories if not set
if "%RETROARCH_CONFIG_DIR%"=="" (
    set "RETROARCH_CONFIG_DIR=!RETROARCH_HOME_DIR!\config"
)

if "%RETROARCH_OVERLAY_DIR%"=="" (
    set "RETROARCH_OVERLAY_DIR=!RETROARCH_HOME_DIR!\overlays"
)

REM Create config directory if it doesn't exist
if not exist "!RETROARCH_CONFIG_DIR!\!CORE_NAME!" (
    mkdir "!RETROARCH_CONFIG_DIR!\!CORE_NAME!"
)

REM Create game-specific config file
set "CONFIG_FILE=!RETROARCH_CONFIG_DIR!\!CORE_NAME!\!GAME_NAME!.cfg"
REM Convert to absolute path for overlay (absolute path)
for %%i in ("!RETROARCH_OVERLAY_DIR!\mahjong\mahjong_!GAME_NAME!.cfg") do set OVERLAY_PATH=%%~fi

echo # Auto-generated overlay configuration for !GAME_NAME! > "!CONFIG_FILE!"
echo input_overlay = "!OVERLAY_PATH!" >> "!CONFIG_FILE!"
echo input_overlay_enable = "true" >> "!CONFIG_FILE!"

REM Output core information for libretro-hook
echo ^<core:%CORE_FILE%^>
