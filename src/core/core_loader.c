#include "core_loader.h"
#include "platform_utils.h"
#include "platform_loader.h"
#include "hook_constants.h"
#include <stdio.h>

void load_core(const char *core_path, const char *rom_path)
{
    platform_load_core(core_path, rom_path);
}

void call_script_before_load(const char *core_file, const char *core_path, const char *rom_path)
{
    const char *system_dir = get_system_directory();
    if (!system_dir) {
        /* システムディレクトリが取得できない場合は何もしない */
        return;
    }

    #ifdef __unix__
        /* UNIX/Linux: <system>/hook/before_load.sh */
        char script_path[MAX_SCRIPT_PATH_SIZE];
        snprintf(script_path, sizeof(script_path), "%s/hook/before_load.sh", system_dir);
        platform_run_script(script_path, core_file, core_path, rom_path);
    #elif defined(_WIN32)
        /* Windows: <system>\hook\before_load.bat */
        char script_path[MAX_SCRIPT_PATH_SIZE];
        snprintf(script_path, sizeof(script_path), "%s\\hook\\before_load.bat", system_dir);
        platform_run_script(script_path, core_file, core_path, rom_path);
    #elif defined(__ANDROID__)
        /* Android: <system>/hook/before_load.sh */
        char script_path[MAX_SCRIPT_PATH_SIZE];
        snprintf(script_path, sizeof(script_path), "%s/hook/before_load.sh", system_dir);
        platform_run_script(script_path, core_file, core_path, rom_path);
    #else
        /* Unsupported platform */
        (void)core_file;
        (void)core_path;
        (void)rom_path;
    #endif
}
