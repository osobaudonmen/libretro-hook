#ifndef PLATFORM_LOADER_H
#define PLATFORM_LOADER_H

/* Enhanced script execution with output capture */
int platform_run_script_with_output(const char *script_path, const char *rom_path, char **output, char **error);
char *platform_parse_core_from_output(const char *output);
void platform_launch_retroarch_and_exit(const char *core_filename, const char *rom_path);
void platform_display_script_info(const char *rom_path, const char *output, const char *error, int exit_code);

#endif /* PLATFORM_LOADER_H */
