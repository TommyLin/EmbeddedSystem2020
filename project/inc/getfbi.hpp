#pragma once

#include <stdint.h>


struct framebuffer_info
{
    uint32_t bits_per_pixel;    // depth of framebuffer
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
    uint32_t yres_virtual;      // how many pixel in a col in virtual screen
};


struct framebuffer_info get_framebuffer_info (const char *framebuffer_device_path);
