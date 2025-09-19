#ifndef PLATFORM_LOADER_H
#define PLATFORM_LOADER_H

/* Platform-specific core loading */
void platform_load_core(const char *core_path, const char *rom_path);
void platform_run_script(const char *script_path, const char *core_file,
                         const char *core_path, const char *rom_path);

#endif /* PLATFORM_LOADER_H */
