#ifndef CORE_LOADER_H
#define CORE_LOADER_H

/* Core loading functionality */
void load_core(const char *core_path, const char *rom_path);
void call_script_before_load(const char *core_path, const char *rom_path);

#endif /* CORE_LOADER_H */
