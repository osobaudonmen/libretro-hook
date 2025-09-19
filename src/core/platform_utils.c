#include "platform_utils.h"
#include "hook_constants.h"
#include "libretro.h"
#include <compat/strl.h>

/* External dependencies */
extern retro_environment_t environ_cb;

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
