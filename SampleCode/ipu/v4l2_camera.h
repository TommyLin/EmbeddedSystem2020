#ifndef _V4L2_CAMERA_H_
#define _V4L2_CAMERA_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <asm/types.h>
#include <linux/fb.h>
#include <sys/mman.h>
//#include <math.h>
#include <string.h>
#include <malloc.h>
//#include <signal.h>

//#include <linux/mxcfb.h>
#include "mxcfb.h"

#define TEST_BUFFER_NUM 3
struct testbuffer
{
        unsigned char *start;
        size_t offset;
        unsigned int length;
};

struct testbuffer buffers[TEST_BUFFER_NUM];
#pragma pack(1)
typedef struct sensor_cap{
    struct{
        char _capture : 1;
        char _output : 1;
        char _overlay : 1;
        char _outpurt_overlay : 1;

        char _read_write : 1;
        char _other : 3;
    }_bit;
    char _cap;
}sensor_cap_uion;
#pragma pack()
#pragma pack(1)
typedef struct device_info{
    sensor_cap_uion v4l2_dev;
    int sensor_width;
    int sensor_height;
    int sensor_top;
    int sensor_left;
    int sensor_format;
    int display_width;
    int display_height;
    int display_top;
    int display_left;
    int screen_fb;
    int screen_size;
    int privew_rotate;
    int privew_timeout;
    int privew_count;
    int g_overlay;
    int g_camera_color;
    int g_camera_framerate;
    int g_capture_mode;
    int g_ctrl_c_rev;
}device_info_st;
#pragma pack()


#endif
