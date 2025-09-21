#include "core_discovery.h"
#include "hook_constants.h"
#include "hook_globals.h"
#include <stdio.h>
#include <string.h>
#include <retro_dirent.h>
#include <file/file_path.h>
#include <streams/file_stream.h>
#include <compat/strl.h>
#include <retro_assert.h>

const char* hook_find_matching_core(const char* game_path)
{
   if (!game_path) return NULL;

   log_cb(RETRO_LOG_INFO, "Finding core for path: %s\n", game_path);

   /* Normalize path separators to forward slashes for consistent matching */
   char normalized_path[4096];
   strlcpy(normalized_path, game_path, sizeof(normalized_path));
   for (char *p = normalized_path; *p; p++) {
       if (*p == '\\') *p = '/';
   }

   /* Check each pattern and return matching core */
   for (int i = 1; i <= 10; i++) {
       char pattern_key[64], core_key[64];
       snprintf(pattern_key, sizeof(pattern_key), "libretro_hook_path_pattern_%d", i);
       snprintf(core_key, sizeof(core_key), "libretro_hook_core_select_%d", i);

       struct retro_variable var_pattern = { pattern_key, NULL };
       struct retro_variable var_core = { core_key, NULL };

       if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var_pattern) && var_pattern.value &&
           environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var_core) && var_core.value) {

           /* Check if pattern is set and matches */
           if (strlen(var_pattern.value) > 0 && strstr(normalized_path, var_pattern.value)) {
               log_cb(RETRO_LOG_INFO, "Pattern '%s' matches, selected core: %s\n",
                      var_pattern.value, var_core.value);

               /* Return core if not "none" */
               if (strcmp(var_core.value, "none") != 0) {
                   return var_core.value;
               }
           }
       }
   }

   return NULL; /* No matching pattern found */
}

void hook_discover_available_cores(char* cores_list, size_t cores_list_size)
{
   if (!cores_list || cores_list_size == 0) return;

   strlcpy(cores_list, "none", cores_list_size);

   const char *path = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_LIBRETRO_PATH, &path) && path) {
      log_cb(RETRO_LOG_INFO, "Core path: %s\n", path);
      char cores_dir[4096];
      strlcpy(cores_dir, path, sizeof(cores_dir));
      path_basedir(cores_dir);

      struct RDIR *rdir = retro_opendir(cores_dir);
      if (rdir) {
            while (retro_readdir(rdir)) {
                if (!retro_dirent_is_dir(rdir, NULL)) {
                    const char *name = retro_dirent_get_name(rdir);
                    const char *ext = path_get_extension(name);
                    if (strcmp(ext, "so") == 0 || strcmp(ext, "dll") == 0 || strcmp(ext, "dylib") == 0) {
                        log_cb(RETRO_LOG_INFO, "Found core: %s\n", name);
                        if (strlen(cores_list) + strlen(name) + 2 < cores_list_size) {
                            strlcat(cores_list, "|", cores_list_size);
                            strlcat(cores_list, name, cores_list_size);
                        }
                    }
                }
            }
            retro_closedir(rdir);
      }
   }
}

static void append_choice(char *dst, size_t dst_size, const char *choice)
{
   if (!choice || !*choice) return;
   if (dst[0] == '\0') {
      strlcpy(dst, choice, dst_size);
   } else {
      size_t need = strlen(dst) + 1 + strlen(choice) + 1;
      if (need <= dst_size) {
         strlcat(dst, "|", dst_size);
         strlcat(dst, choice, dst_size);
      }
   }
}

void hook_load_path_patterns(char* patterns_list, size_t patterns_list_size)
{
   if (!patterns_list || patterns_list_size == 0) return;

   patterns_list[0] = '\0';
   const char *sysdir = NULL;
   if (!(environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &sysdir) && sysdir && *sysdir)) {
      strlcpy(patterns_list, DEFAULT_PATH_PATTERNS, patterns_list_size);
      return;
   }

   char patterns_path[4096];
   char hook_dir[MAX_PATH_SIZE];
   fill_pathname_join(hook_dir, sysdir, "hook", sizeof(hook_dir));
   fill_pathname_join(patterns_path, hook_dir, "path_patterns.txt", sizeof(patterns_path));

   RFILE *f = filestream_open(patterns_path, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
   if (!f) {
      strlcpy(patterns_list, DEFAULT_PATH_PATTERNS, patterns_list_size);
      return;
   }
   log_cb(RETRO_LOG_INFO, "Loading path patterns from: %s\n", patterns_path);

   char line[512];
   while (filestream_gets(f, line, sizeof(line))) {
      /* trim whitespace and newlines */
      size_t len = strlen(line);
      while (len && (line[len-1] == '\n' || line[len-1] == '\r' ||
                     line[len-1] == ' ' || line[len-1] == '\t')) {
         line[--len] = '\0';
      }
      char *start = line;
      while (*start == ' ' || *start == '\t') start++;
      if (*start == '\0' || *start == '#')
         continue;
      append_choice(patterns_list, patterns_list_size, start);
   }
   filestream_close(f);

   if (patterns_list[0] == '\0')
      strlcpy(patterns_list, DEFAULT_PATH_PATTERNS, patterns_list_size);
}
