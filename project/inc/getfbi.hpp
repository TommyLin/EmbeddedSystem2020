#pragma once

#include <fstream>
#include <opencv2/imgproc/imgproc.hpp>


struct framebuffer_info
{
    uint32_t bits_per_pixel;    // depth of framebuffer
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
    uint32_t yres_virtual;      // how many pixel in a col in virtual screen
};


struct framebuffer_info get_framebuffer_info (const char *framebuffer_device_path);
void set_framebuffer(std::ofstream *, cv::Mat *, cv::Size2f, framebuffer_info);
