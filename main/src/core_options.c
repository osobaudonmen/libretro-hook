#include "core_options.h"
#include "core_discovery.h"
#include "hook_globals.h"
#include <stdio.h>
#include <string.h>
#include <retro_assert.h>
#include <compat/strl.h>

static char available_cores_list[MAX_CORES_LIST_SIZE] = "none";
static char available_patterns_list[MAX_PATTERNS_LIST_SIZE] = "";

void init_core_options(void)
{
   /* Ensure patterns list has defaults if empty */
   if (available_patterns_list[0] == '\0')
      strlcpy(available_patterns_list, DEFAULT_PATH_PATTERNS, sizeof(available_patterns_list));

   /* Discover available cores and build selection list */
   discover_available_cores(available_cores_list, sizeof(available_cores_list));
   load_path_patterns(available_patterns_list, sizeof(available_patterns_list));

   /* Create core option variables with discovered cores */
   static struct retro_variable vars[21]; /* 10 pairs + 1 NULL terminator */

   /* Build the variables dynamically */
   for (int i = 0; i < MAX_CORE_OPTIONS; i++) {
       static char pattern_keys[MAX_CORE_OPTIONS][64];
       static char core_keys[MAX_CORE_OPTIONS][64];
       static char pattern_desc[MAX_CORE_OPTIONS][128];
       static char core_desc[MAX_CORE_OPTIONS][MAX_CORES_LIST_SIZE];

       snprintf(pattern_keys[i], sizeof(pattern_keys[i]), "libretro_hook_path_pattern_%d", i + 1);
       snprintf(core_keys[i], sizeof(core_keys[i]), "libretro_hook_core_select_%d", i + 1);
       snprintf(pattern_desc[i], sizeof(pattern_desc[i]), "Path Pattern %d; %s", i + 1, available_patterns_list);
       snprintf(core_desc[i], sizeof(core_desc[i]), "Core Select %d; %s", i + 1, available_cores_list);

       vars[i * 2].key = pattern_keys[i];
       vars[i * 2].value = pattern_desc[i];
       vars[i * 2 + 1].key = core_keys[i];
       vars[i * 2 + 1].value = core_desc[i];
   }

   /* NULL terminator */
   vars[20].key = NULL;
   vars[20].value = NULL;

   environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

void check_variables(void)
{
   log_cb(RETRO_LOG_INFO, "Checking variables...\n");

   /* Log current path patterns and core selections */
   for (int i = 1; i <= MAX_CORE_OPTIONS; i++) {
       char pattern_key[64], core_key[64];
       snprintf(pattern_key, sizeof(pattern_key), "libretro_hook_path_pattern_%d", i);
       snprintf(core_key, sizeof(core_key), "libretro_hook_core_select_%d", i);

       struct retro_variable var_pattern = { pattern_key, NULL };
       struct retro_variable var_core = { core_key, NULL };

       if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var_pattern) && var_pattern.value &&
           environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var_core) && var_core.value) {

           if (strlen(var_pattern.value) > 0) {
               log_cb(RETRO_LOG_INFO, "Pattern %d: '%s' -> Core: %s\n",
                      i, var_pattern.value, var_core.value);
           }
       }
   }
}
