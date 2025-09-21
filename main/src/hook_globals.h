#ifndef HOOK_GLOBALS_H
#define HOOK_GLOBALS_H

#include <stdint.h>
#include "libretro.h"

/* Global variables */
extern uint8_t *frame_buf;
extern struct retro_log_callback logging;
extern retro_log_printf_t log_cb;
extern retro_environment_t environ_cb;
extern retro_video_refresh_t video_cb;
extern char retro_display_messages[4096];

#endif /* HOOK_GLOBALS_H */
