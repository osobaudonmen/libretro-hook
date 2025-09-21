#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <stdint.h>

/* Font constants */
#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define FONT_CHAR_COUNT 95
#define FONT_ASCII_OFFSET 32

/* Font data */
extern const uint8_t font_8x8[FONT_CHAR_COUNT][FONT_HEIGHT];

#endif /* GRAPHICS_FONT_H */
