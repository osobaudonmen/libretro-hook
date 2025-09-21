#ifndef GRAPHICS_UTILS_H
#define GRAPHICS_UTILS_H

#include <stdint.h>
#include "hook_constants.h"

/* Graphics utility functions */
void hook_set_pixel(uint8_t *buf, int x, int y, uint32_t color);
void hook_draw_char(uint8_t *buf, int x, int y, char c, uint32_t color);
void hook_draw_text(uint8_t *buf, int x, int y, const char *text, uint32_t color);
void hook_render_framebuffer(void);

#endif /* GRAPHICS_UTILS_H */
