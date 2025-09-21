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

void hook_discover_available_cores(char* cores_list, size_t cores_list_size)
{
   if (!cores_list || cores_list_size == 0) return;

   strlcpy(cores_list, "none", cores_list_size);

   const char *path = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_LIBRETRO_PATH, &path) && path) {
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
