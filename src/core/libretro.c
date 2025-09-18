#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif
#include "libretro.h"

#include <retro_dirent.h>
#include <file/file_path.h>
#include <streams/file_stream.h>
#include <compat/strl.h>

#define VIDEO_WIDTH 256
#define VIDEO_HEIGHT 192
#define VIDEO_PIXELS VIDEO_WIDTH * VIDEO_HEIGHT

static uint8_t *frame_buf;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static char available_cores_list[8192] = "none";
static char available_patterns_list[1024] = "";
char retro_base_directory[4096];
char retro_game_path[4096];
char retro_core_path[4096];

void init_core_options(void);
static void load_path_patterns(void);

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}


static retro_environment_t environ_cb;

// Function to find matching core for given path
const char* find_matching_core(const char* game_path)
{
   if (!game_path) return NULL;
   
   log_cb(RETRO_LOG_INFO, "Finding core for path: %s\n", game_path);
   
   // Check each pattern and return matching core
   for (int i = 1; i <= 10; i++) {
       char pattern_key[64], core_key[64];
       snprintf(pattern_key, sizeof(pattern_key), "libretro_hook_path_pattern_%d", i);
       snprintf(core_key, sizeof(core_key), "libretro_hook_core_select_%d", i);
       
       struct retro_variable var_pattern = { pattern_key, NULL };
       struct retro_variable var_core = { core_key, NULL };
       
       if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var_pattern) && var_pattern.value &&
           environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var_core) && var_core.value) {
           
           // Check if pattern is set and matches
           if (strlen(var_pattern.value) > 0 && strstr(game_path, var_pattern.value)) {
               log_cb(RETRO_LOG_INFO, "Pattern '%s' matches, selected core: %s\n", 
                      var_pattern.value, var_core.value);
               
               // Return core if not "none"
               if (strcmp(var_core.value, "none") != 0) {
                   return var_core.value;
               }
           }
       }
   }
   
   return NULL; // No matching pattern found
}

void retro_init(void)
{
   frame_buf = (uint8_t*)malloc(VIDEO_PIXELS * sizeof(uint32_t));
   const char *dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
   {
      snprintf(retro_base_directory, sizeof(retro_base_directory), "SYSMTE_DIRECTORY: %s", dir);
   }

   load_path_patterns();
   init_core_options();
}

void retro_deinit(void)
{
   free(frame_buf);
   frame_buf = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   (void)port;
   (void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "skeleton";
   info->library_version  = "0.1";
   info->need_fullpath    = true;
   info->valid_extensions = "";
}

static retro_video_refresh_t video_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width   = VIDEO_WIDTH;
   info->geometry.base_height  = VIDEO_HEIGHT;
   info->geometry.max_width    = VIDEO_WIDTH;
   info->geometry.max_height   = VIDEO_HEIGHT;
   info->geometry.aspect_ratio = 0.0f;

   info->timing.fps            = 60.0;
   info->timing.sample_rate    = 0.0;
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
      log_cb = logging.log;
   else
      log_cb = fallback_log;



   bool no_content = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);

    log_cb(RETRO_LOG_INFO, "retro_set_environment done.\n");
}

void init_core_options(void)
{
   // Reset and build available cores list
   strcpy(available_cores_list, "none");
   // Ensure patterns list has defaults if empty
   if (available_patterns_list[0] == '\0')
      strcpy(available_patterns_list, "/mame/|/fbneo/|/arcade/");
   
   // Discover available cores and build selection list
   const char *path = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_LIBRETRO_PATH, &path) && path)
   {
      snprintf(retro_core_path, sizeof(retro_core_path), "CORE_PATH: %s", path);
      log_cb(RETRO_LOG_INFO, "Core path: %s\n", retro_core_path);
      char cores_dir[4096];
      strcpy(cores_dir, path);
      path_basedir(cores_dir);
      struct RDIR *rdir = retro_opendir(cores_dir);
      if (rdir) {
            while (retro_readdir(rdir)) {
                if (!retro_dirent_is_dir(rdir, NULL)) {
                    const char *name = retro_dirent_get_name(rdir);
                    const char *ext = path_get_extension(name);
                    if (strcmp(ext, "so") == 0 || strcmp(ext, "dll") == 0 || strcmp(ext, "dylib") == 0) {
                        log_cb(RETRO_LOG_INFO, "Found core: %s\n", name);
                        // Add to available cores list
                        if (strlen(available_cores_list) + strlen(name) + 2 < sizeof(available_cores_list)) {
                            strcat(available_cores_list, "|");
                            strcat(available_cores_list, name);
                        }
                    }
                }
            }
            retro_closedir(rdir);
      }
   }


   // Create core option variables with discovered cores
   static struct retro_variable vars[21]; // 10 pairs + 1 NULL terminator
   
   // Build the variables dynamically
   for (int i = 0; i < 10; i++) {
       static char pattern_keys[10][64];
       static char core_keys[10][64];
   static char pattern_desc[10][128];
   static char core_desc[10][8192];
       
      snprintf(pattern_keys[i], sizeof(pattern_keys[i]), "libretro_hook_path_pattern_%d", i + 1);
      snprintf(core_keys[i], sizeof(core_keys[i]), "libretro_hook_core_select_%d", i + 1);
      snprintf(pattern_desc[i], sizeof(pattern_desc[i]), "Path Pattern %d; %s", i + 1, available_patterns_list);
      snprintf(core_desc[i], sizeof(core_desc[i]), "Core Select %d; %s", i + 1, available_cores_list);
       
       vars[i * 2].key = pattern_keys[i];
       vars[i * 2].value = pattern_desc[i];
       vars[i * 2 + 1].key = core_keys[i];
       vars[i * 2 + 1].value = core_desc[i];
   }
   
   // NULL terminator
   vars[20].key = NULL;
   vars[20].value = NULL;
   
   environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

static void append_choice(char *dst, size_t dst_size, const char *choice)
{
   if (!choice || !*choice) return;
   if (dst[0] == '\0') {
      strlcpy(dst, choice, dst_size);
   } else {
      size_t need = strlen(dst) + 1 + strlen(choice) + 1;
      if (need <= dst_size) {
         strcat(dst, "|");
         strcat(dst, choice);
      }
   }
}

static void load_path_patterns(void)
{
   available_patterns_list[0] = '\0';
   const char *sysdir = NULL;
   if (!(environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &sysdir) && sysdir && *sysdir)) {
      strcpy(available_patterns_list, "/mame/|/fbneo/|/arcade/");
      return;
   }

   char patterns_path[4096];
   snprintf(patterns_path, sizeof(patterns_path), "%s%shook%spath_patterns.txt", sysdir, PATH_DEFAULT_SLASH(), PATH_DEFAULT_SLASH());

   RFILE *f = filestream_open(patterns_path, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
   if (!f) {
      strcpy(available_patterns_list, "/mame/|/fbneo/|/arcade/");
      return;
   }
   log_cb(RETRO_LOG_INFO, "Loading path patterns from: %s\n", patterns_path);

   char line[512];
   while (filestream_gets(f, line, sizeof(line))) {
      // trim whitespace and newlines
      size_t len = strlen(line);
      while (len && (line[len-1] == '\n' || line[len-1] == '\r' || line[len-1] == ' ' || line[len-1] == '\t')) {
         line[--len] = '\0';
      }
      char *start = line;
      while (*start == ' ' || *start == '\t') start++;
      if (*start == '\0' || *start == '#')
         continue;
      append_choice(available_patterns_list, sizeof(available_patterns_list), start);
   }
   filestream_close(f);

   if (available_patterns_list[0] == '\0')
      strcpy(available_patterns_list, "/mame/|/fbneo/|/arcade/");
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   (void)cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   (void)cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   (void)cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   (void)cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_reset(void)
{

}

static void check_variables(void)
{
   log_cb(RETRO_LOG_INFO, "Checking variables...\n");
   
   // Log current path patterns and core selections
   for (int i = 1; i <= 10; i++) {
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

void retro_run(void)
{
	video_cb(frame_buf, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * sizeof(uint32_t));

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();
}

bool retro_load_game(const struct retro_game_info *info)
{
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
      return false;
   }

	if (info != NULL) {
		snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);
		log_cb(RETRO_LOG_INFO, "Loading game: %s\n", retro_game_path);
		
		// Find matching core for this path
		const char* selected_core = find_matching_core(retro_game_path);
		if (selected_core) {
			log_cb(RETRO_LOG_INFO, "Selected core for game: %s\n", selected_core);
			
			// TODO: Load the selected core dynamically
			// This would require core_loader implementation
			log_cb(RETRO_LOG_INFO, "Core loading not yet implemented\n");
		} else {
			log_cb(RETRO_LOG_INFO, "No matching core pattern found, using default behavior\n");
		}
	}

   check_variables();

   (void)info;
   return true;
}

void retro_unload_game(void)
{

}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

