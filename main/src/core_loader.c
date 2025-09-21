#include "core_loader.h"
#include "platform_utils.h"
#include "platform_loader.h"
#include "hook_constants.h"
#include "hook_globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* These functions have been removed as they are unused */

void hook_execute_before_load_script(const char *rom_path)
{
    const char* script_path = hook_get_script_path("before_load");
    if (script_path) {
        /* Log the fact we're executing the script and the arguments passed */
        const char *system_dir = hook_get_system_directory();
        log_cb(RETRO_LOG_INFO, "CoreLoader: Executing script: %s\n", script_path);
        log_cb(RETRO_LOG_INFO, "CoreLoader: Script args: system_dir='%s' rom_path='%s'\n",
               system_dir ? system_dir : "(null)", rom_path ? rom_path : "(null)");

        char *script_output = NULL;
        char *script_error = NULL;
        int exit_code = platform_run_script_with_output(script_path, rom_path, &script_output, &script_error);

        /* Always forward script stdout/stderr to RetroArch log */
        if (script_output && strlen(script_output) > 0) {
            log_cb(RETRO_LOG_INFO, "CoreLoader: Script stdout:\n%s", script_output);
        } else {
            log_cb(RETRO_LOG_INFO, "CoreLoader: Script stdout: (empty)\n");
        }

        if (script_error && strlen(script_error) > 0) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Script stderr:\n%s", script_error);
        } else {
            log_cb(RETRO_LOG_INFO, "CoreLoader: Script stderr: (empty)\n");
        }

        /* Parse output for core specification */
        char *core_filename = platform_parse_core_from_output(script_output);

        if (core_filename && strlen(core_filename) > 0) {
            /* Core specified, launch retroarch with that core */
            log_cb(RETRO_LOG_INFO, "Core specified: %s\n", core_filename);
            platform_launch_retroarch_and_exit(core_filename, rom_path);
        } else {
            /* No core specified, only display on-screen info (game screen) */
            log_cb(RETRO_LOG_INFO, "No core specified, will display script information on-screen\n");
            platform_display_script_info(rom_path, script_output, script_error, exit_code);
        }

        /* Cleanup */
        if (script_output) free(script_output);
        if (script_error) free(script_error);
        if (core_filename) free(core_filename);
    } else {
        log_cb(RETRO_LOG_ERROR, "No before_load script found\n");
        snprintf(retro_display_messages, sizeof(retro_display_messages),
                "ERROR: No before_load script found\nROM: %s", rom_path);
    }
}
