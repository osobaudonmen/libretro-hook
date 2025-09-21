#include "platform_utils.h"
#include "hook_constants.h"
#include "hook_globals.h"
#include <stdio.h>
#include <compat/strl.h>
#include <file/file_path.h>

const char* hook_get_system_directory(void)
{
   static char system_dir[MAX_PATH_SIZE] = "";
   const char *dir = NULL;

   if (environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir) {
      strlcpy(system_dir, dir, sizeof(system_dir));
      return system_dir;
   }

   return NULL;
}

const char* hook_get_script_path(const char* script_name)
{
   static char script_path[MAX_SCRIPT_PATH_SIZE] = "";
   const char* system_dir = hook_get_system_directory();
   if (!system_dir) return NULL;

   char hook_dir[MAX_PATH_SIZE];
   fill_pathname_join(hook_dir, system_dir, "hook", sizeof(hook_dir));

#ifdef _WIN32
   char script_filename[256];
   snprintf(script_filename, sizeof(script_filename), "%s.bat", script_name);
   fill_pathname_join(script_path, hook_dir, script_filename, sizeof(script_path));
#else
   char script_filename[256];
   snprintf(script_filename, sizeof(script_filename), "%s.sh", script_name);
   fill_pathname_join(script_path, hook_dir, script_filename, sizeof(script_path));
#endif

   return script_path;
}
