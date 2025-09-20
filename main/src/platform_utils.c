#include "platform_utils.h"
#include "hook_constants.h"
#include "hook_globals.h"
#include <stdio.h>
#include <compat/strl.h>

const char* get_system_directory(void)
{
   static char system_dir[MAX_PATH_SIZE] = "";
   const char *dir = NULL;

   if (environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir) {
      strlcpy(system_dir, dir, sizeof(system_dir));
      return system_dir;
   }

   return NULL;
}

const char* get_script_path(const char* script_name)
{
   static char script_path[MAX_SCRIPT_PATH_SIZE] = "";
   const char* system_dir = get_system_directory();
   if (!system_dir) return NULL;

#ifdef _WIN32
   snprintf(script_path, sizeof(script_path), "%s\\hook\\%s.bat", system_dir, script_name);
#else
   snprintf(script_path, sizeof(script_path), "%s/hook/%s.sh", system_dir, script_name);
#endif

   return script_path;
}
