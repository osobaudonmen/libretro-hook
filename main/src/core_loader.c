#include "core_loader.h"
#include "platform_utils.h"
#include "platform_loader.h"
#include "hook_constants.h"
#include <stdio.h>

void load_core(const char *core_path, const char *rom_path)
{
    platform_load_core(core_path, rom_path);
}

void call_script_before_load(const char *core_path, const char *rom_path)
{
    const char* script_path = get_script_path("before_load");
    if (script_path) {
        platform_run_script(script_path, core_path, rom_path);
    }
}
