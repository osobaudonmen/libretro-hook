#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif

#include "libretro.h"
#include "core_loader.h"
#include "core_discovery.h"
#include "core_options.h"
#include "platform_utils.h"
#include "hook_constants.h"

#include <compat/strl.h>
#include <retro_assert.h>

static uint8_t *frame_buf;
static struct retro_log_callback logging;
retro_log_printf_t log_cb;
retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;

/* Global paths for debugging/logging */
static char retro_base_directory[MAX_PATH_SIZE];
static char retro_game_path[MAX_PATH_SIZE];
static char retro_core_path[MAX_PATH_SIZE];

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

void retro_init(void)
{
   frame_buf = (uint8_t*)malloc(VIDEO_PIXELS * sizeof(uint32_t));
   const char *dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
   {
      snprintf(retro_base_directory, sizeof(retro_base_directory), "SYSTEM_DIRECTORY: %s", dir);
   }

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
   info->library_name     = "hook";
   info->library_version  = "0.1";
   info->need_fullpath    = true;
   info->block_extract    = true;
   info->valid_extensions = "7z|zip";
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

		/* Find matching core for this path */
		const char* selected_core = find_matching_core(retro_game_path);
		if (selected_core) {
			log_cb(RETRO_LOG_INFO, "Selected core for game: %s\n", selected_core);
         call_script_before_load(selected_core, retro_game_path);
         load_core(selected_core, retro_game_path);
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

