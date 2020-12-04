#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include "getfbi.hpp"

using namespace std;


struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path )
{
    struct framebuffer_info fb_info;
    struct fb_var_screeninfo screen_info;

    int fd = open(framebuffer_device_path, O_RDWR);

    ioctl(fd, FBIOGET_VSCREENINFO, &screen_info);

    fb_info.xres_virtual = screen_info.xres_virtual;
    fb_info.yres_virtual = screen_info.yres_virtual;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;

    return fb_info;
};
