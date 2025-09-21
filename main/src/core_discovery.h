#ifndef CORE_DISCOVERY_H
#define CORE_DISCOVERY_H

#include "libretro.h"

/* Core discovery and matching functionality */
#define CORE_DISCOVERY_H

void hook_discover_available_cores(char* cores_list, size_t cores_list_size);
void hook_load_path_patterns(char* patterns_list, size_t patterns_list_size);

#endif /* CORE_DISCOVERY_H */
