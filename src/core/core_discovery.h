#ifndef CORE_DISCOVERY_H
#define CORE_DISCOVERY_H

#include "libretro.h"

/* Core discovery and matching functionality */
const char* find_matching_core(const char* game_path);
void discover_available_cores(char* cores_list, size_t cores_list_size);
void load_path_patterns(char* patterns_list, size_t patterns_list_size);

#endif /* CORE_DISCOVERY_H */
