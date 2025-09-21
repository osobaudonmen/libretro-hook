#include "graphics_utils.h"
#include "graphics_font.h"
#include "hook_globals.h"
#include <string.h>

void hook_set_pixel(uint8_t *buf, int x, int y, uint32_t color)
{
    if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT)
        return;
    ((uint32_t*)buf)[y * VIDEO_WIDTH + x] = color;
}

void hook_draw_char(uint8_t *buf, int x, int y, char c, uint32_t color)
{
    if (c < FONT_ASCII_OFFSET || c > (FONT_ASCII_OFFSET + FONT_CHAR_COUNT - 1))
        return;

    const uint8_t *bitmap = font_8x8[c - FONT_ASCII_OFFSET];
    for (int dy = 0; dy < FONT_HEIGHT; dy++) {
        for (int dx = 0; dx < FONT_WIDTH; dx++) {
            if (bitmap[dy] & (1 << (7 - dx))) {
                hook_set_pixel(buf, x + dx, y + dy, color);
            }
        }
    }
}

void hook_draw_text(uint8_t *buf, int x, int y, const char *text, uint32_t color)
{
    int start_x = x;
    while (*text) {
        if (*text == '\n') {
            y += 10;
            x = start_x;
        } else {
            hook_draw_char(buf, x, y, *text, color);
            x += FONT_WIDTH;
        }
        text++;
    }
}

void hook_render_framebuffer(void)
{
    memset(frame_buf, 0, VIDEO_PIXELS * sizeof(uint32_t));

    if (retro_display_messages[0]) {
        hook_draw_text(frame_buf, 10, 10, retro_display_messages, 0xFFFFFFFF);
    }

    video_cb(frame_buf, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * sizeof(uint32_t));
}
