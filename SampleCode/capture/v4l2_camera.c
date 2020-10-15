/*
 * Copyright 2004-2013 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*
 * @file v4l2_camera.c
 *
 * @brief Mxc Video For Linux 2 driver test application
 *
 */

#ifdef __cplusplus
extern "C"{
#endif

/*=======================================================================
                                        INCLUDE FILES
=======================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Verification Test Environment Include Files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
//#include <linux/videodev2.h>
#include "videodev2.h"
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>

//#include <linux/mxcfb.h>
#include "mxcfb.h"

#define TFAIL -1
#define TPASS 0

#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_RGB332  ipu_fourcc('R','G','B','1') /*!<  8  RGB-3-3-2     */
#define IPU_PIX_FMT_RGB555  ipu_fourcc('R','G','B','O') /*!< 16  RGB-5-5-5     */
#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P') /*!< 16  RGB-5-6-5     */
#define IPU_PIX_FMT_RGB666  ipu_fourcc('R','G','B','6') /*!< 18  RGB-6-6-6     */
#define IPU_PIX_FMT_BGR24   ipu_fourcc('B','G','R','3') /*!< 24  BGR-8-8-8     */
#define IPU_PIX_FMT_RGB24   ipu_fourcc('R','G','B','3') /*!< 24  RGB-8-8-8     */
#define IPU_PIX_FMT_BGR32   ipu_fourcc('B','G','R','4') /*!< 32  BGR-8-8-8-8   */
#define IPU_PIX_FMT_BGRA32  ipu_fourcc('B','G','R','A') /*!< 32  BGR-8-8-8-8   */
#define IPU_PIX_FMT_RGB32   ipu_fourcc('R','G','B','4') /*!< 32  RGB-8-8-8-8   */
#define IPU_PIX_FMT_RGBA32  ipu_fourcc('R','G','B','A') /*!< 32  RGB-8-8-8-8   */
#define IPU_PIX_FMT_ABGR32  ipu_fourcc('A','B','G','R') /*!< 32  ABGR-8-8-8-8  */

char v4l_device[100] = "/dev/video0";
int fd_v4l = 0;
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
char *screen_fbp = NULL;
device_info_st v4l2_camera;
#define VIDEO_CLEAR(x) memset(&(x), 0, sizeof(x))
static void print_pixelformat(char *prefix, int val)
{
        printf("%s: %c%c%c%c\n", prefix ? prefix : "pixelformat",
                                        val & 0xff,
                                        (val >> 8) & 0xff,
                                        (val >> 16) & 0xff,
                                        (val >> 24) & 0xff);
}

void ctrl_c_handler(int signum, siginfo_t *info, void *myact)
{
        v4l2_camera.g_ctrl_c_rev = 1;
        return;
}

int
mxc_v4l_overlay_test(int timeout)
{
        int i;
        int overlay = 1;
        int retval = 0;
        struct v4l2_control ctl;
#ifdef BUILD_FOR_ANDROID
        char fb_device_0[100] = "/dev/graphics/fb0";
#else
        char fb_device_0[100] = "/dev/fb0";
#endif
        int fd_graphic_fb = 0;
        struct fb_var_screeninfo fb0_var;
        struct mxcfb_loc_alpha l_alpha;

        if (ioctl(fd_v4l, VIDIOC_OVERLAY, &overlay) < 0) {
                printf("VIDIOC_OVERLAY start failed\n");
                retval = TFAIL;
                goto out1;
        }

        printf("\n[funtion- %s] is run...\r\n",__FUNCTION__);

        for (i = 0; i < 3 ; i++) {
                // flash a frame
                ctl.id = V4L2_CID_PRIVATE_BASE + 1;
                if (ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl) < 0)
                {
                        printf("set ctl failed\n");
                        retval = TFAIL;
                        goto out2;
                }
                    sleep(1);
        }

        if (v4l2_camera.g_camera_color == 1) {
                ctl.id = V4L2_CID_BRIGHTNESS;
                for (i = 0; i < 0xff; i+=0x20) {
                            ctl.value = i;
                        printf("change the brightness %d\n", i);
                    ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl);
                            sleep(1);
                }
                }
                else if (v4l2_camera.g_camera_color == 2) {
                ctl.id = V4L2_CID_SATURATION;
                for (i = 25; i < 150; i+= 25) {
                            ctl.value = i;
                        printf("change the color saturation %d\n", i);
                    ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl);
                            sleep(5);
                }
                }
                else if (v4l2_camera.g_camera_color == 3) {
                ctl.id = V4L2_CID_RED_BALANCE;
                for (i = 0; i < 0xff; i+=0x20) {
                            ctl.value = i;
                        printf("change the red balance %d\n", i);
                    ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl);
                            sleep(1);
                }
                }
                else if (v4l2_camera.g_camera_color == 4) {
                ctl.id = V4L2_CID_BLUE_BALANCE;
                for (i = 0; i < 0xff; i+=0x20) {
                            ctl.value = i;
                        printf("change the blue balance %d\n", i);
                    ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl);
                            sleep(1);
                }
                }
                else if (v4l2_camera.g_camera_color == 5) {
                ctl.id = V4L2_CID_BLACK_LEVEL;
                for (i = 0; i < 4; i++) {
                            ctl.value = i;
                        printf("change the black balance %d\n", i);
                    ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl);
                            sleep(5);
                }
        }
        else {
                        sleep(timeout);
        }

out2:
        overlay = 0;
        if (ioctl(fd_v4l, VIDIOC_OVERLAY, &overlay) < 0)
        {
                printf("VIDIOC_OVERLAY stop failed\n");
                retval = TFAIL;
                goto out1;
        }
out1:

out0:

        close(fd_graphic_fb);
        return retval;
}

int
mxc_v4l_overlay_setup(struct v4l2_format *fmt)
{
        struct v4l2_streamparm parm;
        v4l2_std_id id;
        struct v4l2_control ctl;
        struct v4l2_crop crop;
        struct v4l2_frmsizeenum fsize;
        struct v4l2_fmtdesc ffmt;

        printf("sensor supported frame size:\n");
        fsize.index = 0;
        while (ioctl(fd_v4l, VIDIOC_ENUM_FRAMESIZES, &fsize) >= 0) {
                printf(" %dx%d\n", fsize.discrete.width,
                                               fsize.discrete.height);
                fsize.index++;
        }

        ffmt.index = 0;
        while (ioctl(fd_v4l, VIDIOC_ENUM_FMT, &ffmt) >= 0) {
                print_pixelformat("sensor frame format", ffmt.pixelformat);
                ffmt.index++;
        }

        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parm.parm.capture.timeperframe.numerator = 1;
        parm.parm.capture.timeperframe.denominator = v4l2_camera.g_camera_framerate;
        parm.parm.capture.capturemode = v4l2_camera.g_capture_mode;

        if (ioctl(fd_v4l, VIDIOC_S_PARM, &parm) < 0)
        {
                printf("VIDIOC_S_PARM failed\n");
                return TFAIL;
        }

        parm.parm.capture.timeperframe.numerator = 0;
        parm.parm.capture.timeperframe.denominator = 0;

        if (ioctl(fd_v4l, VIDIOC_G_PARM, &parm) < 0)
        {
                printf("get frame rate failed\n");
                return TFAIL;
        }

        printf("frame_rate is %d\n",
               parm.parm.capture.timeperframe.denominator);


        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._output) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (ioctl(fd_v4l, VIDIOC_G_CROP, &crop) < 0)
            {
                printf("VIDIOC_G_CROP failed\n");
                return -1;
            }
            crop.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
            crop.c.left = v4l2_camera.sensor_left;
            crop.c.top = v4l2_camera.sensor_top;
            crop.c.width = v4l2_camera.sensor_width;
            crop.c.height = v4l2_camera.sensor_height;
            if (ioctl(fd_v4l, VIDIOC_S_CROP, &crop) < 0)
            {
                printf("set cropping failed\n");
                return TFAIL;
            }
            ctl.id = V4L2_CID_PRIVATE_BASE;// + 2;
            ctl.value = v4l2_camera.privew_rotate;
            if (ioctl(fd_v4l, VIDIOC_S_CTRL, &ctl) < 0)
            {
                printf("set control failed\n");
                return TFAIL;
            }
        }else
        {
            fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
            fmt->fmt.pix.width=v4l2_camera.display_width;
            fmt->fmt.pix.height=v4l2_camera.display_height;
        }
        if (ioctl(fd_v4l, VIDIOC_S_FMT, fmt) < 0)
        {
                printf("set format failed\n");
//                return TFAIL;
        }

        if (ioctl(fd_v4l, VIDIOC_G_FMT, fmt) < 0)
        {
                printf("get format failed\n");

        }else
        {
                printf("\t Width = %d", fmt->fmt.pix.width);
                printf("\t Height = %d", fmt->fmt.pix.height);
                printf("\t Image size = %d\n", fmt->fmt.pix.sizeimage);
                print_pixelformat(0, fmt->fmt.pix.pixelformat);
        }
        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._output) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (ioctl(fd_v4l, VIDIOC_G_STD, &id) < 0)
            {
                printf("VIDIOC_G_STD failed\n");
            }
        }

        return TPASS;
}
int process_cmdline(int argc, char **argv)
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-if") == 0) {
                        ++i;
                        if(strcmp(argv[i], "0") == 0)
                        {
                            v4l2_camera.sensor_width = 640;
                            v4l2_camera.sensor_height = 480;
                        }
                        else if(strcmp(argv[i], "1") == 0)
                        {
                           v4l2_camera.sensor_width = 720;
                           v4l2_camera.sensor_height = 576;
                        }
                }
                else if (strcmp(argv[i], "-of") == 0) {
                        ++i;
                        if(strcmp(argv[i], "1") == 0)
                        {
                            v4l2_camera.display_width = v4l2_camera.sensor_width;
                            v4l2_camera.display_height = v4l2_camera.sensor_height;
                        }
                        else if(strcmp(argv[i], "0") == 0)
                        {
                           v4l2_camera.display_width = 800;
                           v4l2_camera.display_height = 480;
                        }
                }
                else if (strcmp(argv[i], "-iw") == 0) {
                        v4l2_camera.sensor_width = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-ih") == 0) {
                        v4l2_camera.sensor_height = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-ow") == 0) {
                        v4l2_camera.display_width = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-oh") == 0) {
                        v4l2_camera.display_height = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-r") == 0) {
                        v4l2_camera.privew_rotate = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-t") == 0) {
                        v4l2_camera.privew_timeout = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-di") == 0) {
                        strcpy(v4l_device, argv[++i]);
                }
                else if (strcmp(argv[i], "-v") == 0) {
                        v4l2_camera.g_camera_color = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-fr") == 0) {
                        v4l2_camera.g_camera_framerate = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-fg") == 0) {
                        v4l2_camera.g_overlay = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "-m") == 0) {
                        v4l2_camera.g_capture_mode = atoi(argv[++i]);
                }

                else if (strcmp(argv[i], "-help") == 0) {
                        printf("MXC Video4Linux overlay Device Test\n\n" \
                               " -if <input frame> 0orNull-640*480,1-720*576\n"\
                               " -of <output frame> 0-800*480, 1-'=-if'\n"\
                               " -iw <input width>\n -ih <input height>\n" \
                               " -ow <display width>\n -oh <display height>\n" \
                               " -r <rotate mode>\n -t <timeout>\n" \
                               " -di <camera select, /dev/video0, /dev/video1,/dev/video2> \n" \
                               " -v <camera color> 1-brightness 2-saturation"\
                               " 3-red 4-blue 5-black balance\n"\
                               " -m <capture mode> 0-low resolution 1-high resolution\n" \
                               " -fr <frame rate> 30fps by default\n" \
                               " -fg foreground mode when -fg specified as 1,"\
                               " otherwise go to frame buffer\n");
                        return -1;
                }
        }
        printf("\n\n v4l2_device: %s\n",v4l_device);
        printf("sensor_width = %d, sensor_height = %d, display_width = %d, display_height = %d\n",\
               v4l2_camera.sensor_width, v4l2_camera.sensor_height,\
               v4l2_camera.display_width, v4l2_camera.display_height);
        if ((v4l2_camera.display_width == 0) || (v4l2_camera.display_height == 0)) {
                return TFAIL;
        }
        return 0;
}

int clip(int value, int min, int max)
{
    return (value > max ? max : value < min ? min : value);
}
int process_image(void *addr,int length,int fwidth,int fheight,struct fb_var_screeninfo fb0_var,struct fb_fix_screeninfo fb0_fix)
{
    unsigned char* in=(char*)addr;
    int width=fwidth;
    int height=fheight;
    int istride=fwidth *2;
    int x,y,j;
    int y0,u,y1,v,r,g,b;
    long location=0;
    if(height > 480)
    {
        height = 480;
    }
    if(width > 800)
    {
        width = 800;
    }
//    printf("fwidth:%d,fheight:%d,fb0_var.xoffset:%d,fb0_var.yoffset:%d,fb0_fix.line_length:%d,fb0_var.bits_per_pixel:%d\n",fwidth,fheight,fb0_var.xoffset,fb0_var.yoffset,fb0_fix.line_length,fb0_var.bits_per_pixel);
    for ( y = 0; y < height; ++y)
    {
        for (j = 0, x=0; j < width * 2 ; j += 4,x +=2)
        {
            location = (x+fb0_var.xoffset) * (fb0_var.bits_per_pixel/8) + (y+fb0_var.yoffset) * fb0_fix.line_length;
            y0 = in[j];
            u = in[j + 1] - 128;
            y1 = in[j + 2];
            v = in[j + 3] - 128;
            r = (298 * y0 + 409 * v + 128) >> 8;
            g = (298 * y0 - 100 * u - 208 * v + 128) >> 8;
            b = (298 * y0 + 516 * u + 128) >> 8;
            screen_fbp[ location + 0] = clip(b, 0, 255);
            screen_fbp[ location + 1] = clip(g, 0, 255);
            screen_fbp[ location + 2] = clip(r, 0, 255);

            r = (298 * y1 + 409 * v + 128) >> 8;
            g = (298 * y1 - 100 * u - 208 * v + 128) >> 8;
            b = (298 * y1 + 516 * u + 128) >> 8;
            screen_fbp[ location + 3] = clip(b, 0, 255);
            screen_fbp[ location + 4] = clip(g, 0, 255);
            screen_fbp[ location + 5] = clip(r, 0, 255);
        }        in +=istride;      }
  usleep(40);
    return 0;
}

int start_capturing(int fd_v4l)
{
        unsigned int i;
        struct v4l2_buffer buf;
        enum v4l2_buf_type type;
        struct v4l2_requestbuffers req;
        memset(&req, 0, sizeof (req));
        req.count = TEST_BUFFER_NUM;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        printf("\n[funtion- %s] is run...\r\n",__FUNCTION__);
        if (ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0)
        {
                printf("v4l_capture_setup: VIDIOC_REQBUFS failed\n");
                return TFAIL;
        }

        for (i = 0; i < TEST_BUFFER_NUM; i++)
        {
                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;
                if (ioctl(fd_v4l, VIDIOC_QUERYBUF, &buf) < 0)
                {
                        printf("VIDIOC_QUERYBUF error\n");
                        return TFAIL;
                }

                buffers[i].length = buf.length;
                buffers[i].offset = (size_t) buf.m.offset;
                buffers[i].start = mmap (NULL, buffers[i].length,
                    PROT_READ | PROT_WRITE, MAP_SHARED,
                    fd_v4l, buffers[i].offset);
                memset(buffers[i].start, 0xFF, buffers[i].length);
        }
        for (i = 0; i < TEST_BUFFER_NUM; i++)
        {
                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;
                buf.m.offset = buffers[i].offset;

                if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0) {
                        printf("VIDIOC_QBUF error\n");
                        return TFAIL;
                }

        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl (fd_v4l, VIDIOC_STREAMON, &type) < 0) {
                printf("VIDIOC_STREAMON error\n");
                return -1;
        }
        return 0;
}
int stop_capturing(int fd_v4l)
{
        enum v4l2_buf_type type;

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        return ioctl (fd_v4l, VIDIOC_STREAMOFF, &type);
}
int
main(int argc, char **argv)
{
        struct v4l2_format fmt;
        struct v4l2_framebuffer fb_v4l2;
#ifdef BUILD_FOR_ANDROID
        char fb_device_0[100] = "/dev/graphics/fb0";
#else
        char fb_device_0[100] = "/dev/fb0";
#endif
        char *fb_device_fg;
        int fd_fb_0 = 0;
        struct fb_fix_screeninfo fb0_fix;
        struct fb_var_screeninfo fb0_var;
        struct mxcfb_color_key color_key;
        struct mxcfb_gbl_alpha g_alpha;
        struct mxcfb_loc_alpha l_alpha;
        int ret = 0;
        struct sigaction act;
        static struct v4l2_dbg_chip_ident v_chip;
        struct v4l2_capability cap;
        char *screeninfo_tmp = NULL;

        v4l2_camera.v4l2_dev._cap = 0;
        v4l2_camera.sensor_width = 640;
        v4l2_camera.sensor_height = 480;
        v4l2_camera.sensor_top = 0;
        v4l2_camera.sensor_left = 0;
        v4l2_camera.screen_fb = 0;
        v4l2_camera.screen_size = 0;
        v4l2_camera.display_width = 800;
        v4l2_camera.display_height = 480;
        v4l2_camera.display_top = 0;
        v4l2_camera.display_left = 0;
        v4l2_camera.g_camera_color = 0;
        v4l2_camera.g_camera_framerate = 30;
        v4l2_camera.g_overlay = 1;
        v4l2_camera.g_capture_mode = 0;
        v4l2_camera.g_ctrl_c_rev = 0;
        v4l2_camera.privew_rotate = 1;
        v4l2_camera.privew_timeout = 10;
        v4l2_camera.privew_count = 100;

        /* for ctrl-c */
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = ctrl_c_handler;

        if((ret = sigaction(SIGINT, &act, NULL)) < 0) {
                printf("install sigal error\n");
                return TFAIL;
        }

        if (process_cmdline(argc, argv) < 0) {
                return TFAIL;
        }

        if ((fd_v4l = open(v4l_device, O_RDWR, 0)) < 0) {
                printf("Unable to open %s\n", v4l_device);
                return TFAIL;
        }
        VIDEO_CLEAR(cap);
        if(ioctl(fd_v4l, VIDIOC_QUERYCAP, &cap) < 0){
            printf("VIDIOC_QUERYCAP is error  \n");
            goto err1;
        }

        printf("\n sensor supported capability :\n");
        if((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
            printf("\t capture is support \r\n");
            v4l2_camera.v4l2_dev._bit._capture = 1;
        }
        else
        {
            printf("\t capture is not support \r\n");
        }
        if((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT )){
            printf("\t output is support \r\n");
            v4l2_camera.v4l2_dev._bit._output = 1;
        }
        else
        {
            printf("\t output is not support \r\n");
        }
        if((cap.capabilities & V4L2_CAP_VIDEO_OVERLAY  )){
            printf("\t overlay is support \r\n");
            v4l2_camera.v4l2_dev._bit._overlay = 1;
        }
        else
        {
            printf("\t overlay is not support \r\n");
        }
        if((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY  )){
            printf("\t output-overlay is support, but demo is not support \r\n");
            //v4l2_camera.v4l2_dev._bit._outpurt_overlay = 1;
        }
        else
        {
            printf("\t output-overlay is not support \r\n");
        }
        if((cap.capabilities & V4L2_CAP_READWRITE  )){
            printf("\t read-write is support \r\n");
            v4l2_camera.v4l2_dev._bit._read_write = 1 ;
        }
        else
        {
            printf("\t read-write is not support \r\n");
        }

        if ((fd_fb_0 = open(fb_device_0, O_RDWR )) < 0)	{
                printf("Unable to open frame buffer 0\n");
                goto err1;
        }

        if (ioctl(fd_fb_0, FBIOGET_VSCREENINFO, &fb0_var) < 0) {
            printf("ioctl FBIOGET_VSCREENINFO err \n");
                goto err0;
        }
        if (ioctl(fd_fb_0, FBIOGET_FSCREENINFO, &fb0_fix) < 0) {
            printf("ioctl FBIOGET_FSCREENINFO err \n");
                goto err0;
        }

        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (ioctl(fd_v4l, VIDIOC_DBG_G_CHIP_IDENT, &v_chip))
            {
                    printf("VIDIOC_DBG_G_CHIP_IDENT failed.\n");
                    goto err0;
            }
            printf("sensor v_chip is %s\n", v_chip.match.name);
        }else
        {
            v4l2_camera.screen_size = fb0_var.xres * fb0_var.yres *fb0_var.bits_per_pixel / 8;
            screeninfo_tmp = malloc(sizeof(char)*v4l2_camera.screen_size);
            screen_fbp=(char*)mmap(0,v4l2_camera.screen_size,PROT_READ | PROT_WRITE,MAP_SHARED, fd_fb_0, 0);
            if(screen_fbp == NULL)
            {
                printf("screen_fbp mmap error \r\n");
                goto err0;
            }else
                memcpy(screeninfo_tmp,screen_fbp,v4l2_camera.screen_size);
        }
        if (strcmp(fb0_fix.id, "DISP3 BG - DI1") == 0)
                v4l2_camera.screen_fb = 1;
        else if (strcmp(fb0_fix.id, "DISP3 BG") == 0)
                v4l2_camera.screen_fb = 0;
        else if (strcmp(fb0_fix.id, "DISP4 BG") == 0)
                v4l2_camera.screen_fb = 3;
        else if (strcmp(fb0_fix.id, "DISP4 BG - DI1") == 0)
                v4l2_camera.screen_fb = 4;
        printf("[] v4l2_camera.screen_fb:%d \r\n",v4l2_camera.screen_fb);
        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._output) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (ioctl(fd_v4l, VIDIOC_S_OUTPUT, &v4l2_camera.screen_fb) < 0)
            {
                printf("VIDIOC_S_OUTPUT failed\n");
            }
        }
        fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
        fmt.fmt.win.w.top=  v4l2_camera.display_top ;
        fmt.fmt.win.w.left= v4l2_camera.display_left;
        fmt.fmt.win.w.width=v4l2_camera.display_width;
        fmt.fmt.win.w.height=v4l2_camera.display_height;
        if (mxc_v4l_overlay_setup(&fmt) < 0) {
                printf("Setup overlay failed.\n");
                goto err0;
                }

        memset(&fb_v4l2, 0, sizeof(fb_v4l2));
        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._output) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (!v4l2_camera.g_overlay) {
                g_alpha.alpha = 255;
                g_alpha.enable = 1;
                if (ioctl(fd_fb_0, MXCFB_SET_GBL_ALPHA, &g_alpha) < 0) {
                    printf("Set global alpha failed\n");
                    goto err0;
                }



                fb_v4l2.fmt.width = fb0_var.xres;
                fb_v4l2.fmt.height = fb0_var.yres;

                if (fb0_var.bits_per_pixel == 32) {
                    fb_v4l2.fmt.pixelformat = IPU_PIX_FMT_BGR32;
                    fb_v4l2.fmt.bytesperline = 4 * fb_v4l2.fmt.width;
                }
                else if (fb0_var.bits_per_pixel == 24) {
                    fb_v4l2.fmt.pixelformat = IPU_PIX_FMT_BGR24;
                    fb_v4l2.fmt.bytesperline = 3 * fb_v4l2.fmt.width;
                }
                else if (fb0_var.bits_per_pixel == 16) {
                    fb_v4l2.fmt.pixelformat = IPU_PIX_FMT_RGB565;
                    fb_v4l2.fmt.bytesperline = 2 * fb_v4l2.fmt.width;
                }

                fb_v4l2.flags = V4L2_FBUF_FLAG_PRIMARY;
                fb_v4l2.base = (void *) fb0_fix.smem_start +
                        fb0_fix.line_length*fb0_var.yoffset;
            } else {
                g_alpha.alpha = 0;
                g_alpha.enable = 1;
                if (ioctl(fd_fb_0, MXCFB_SET_GBL_ALPHA, &g_alpha) < 0) {
                    printf("Set global alpha failed\n");
                    goto err0;
                }

                color_key.color_key = 0x00080808;
                color_key.enable = 0;
                if (ioctl(fd_fb_0, MXCFB_SET_CLR_KEY, &color_key) < 0) {
                    printf("Set color key failed\n");
                    goto err0;
                }



                if (ioctl(fd_v4l, VIDIOC_G_FBUF, &fb_v4l2) < 0) {
                    printf("Get framebuffer failed\n");
                    goto err0;
                }
                fb_v4l2.flags = V4L2_FBUF_FLAG_OVERLAY;
            }
            close(fd_fb_0);

            if (ioctl(fd_v4l, VIDIOC_S_FBUF, &fb_v4l2) < 0)
            {
                printf("set framebuffer failed\n");
                goto err0;
            }

            if (ioctl(fd_v4l, VIDIOC_G_FBUF, &fb_v4l2) < 0) {
                printf("set framebuffer failed\n");
                goto err0;
            }
            printf("\n frame buffer width %d, height %d, bytesperline %d\n",
                    fb_v4l2.fmt.width, fb_v4l2.fmt.height, fb_v4l2.fmt.bytesperline);
            ret = mxc_v4l_overlay_test(v4l2_camera.privew_timeout);
        }else
        {
            FILE * fd_y_file = 0;
            char *file = "/v4l2_test.yuv";
            struct v4l2_buffer buf;
            if ((fd_y_file = fopen(file, "wb")) ==NULL)
            {
                    printf("Unable to create y frame recording file\n");
                    goto err1;
            }
            if (start_capturing(fd_v4l) < 0)
            {
                    printf("start_capturing failed\n");
                    goto err2;
            }
            while(v4l2_camera.privew_count--)
            {

                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                if (ioctl (fd_v4l, VIDIOC_DQBUF, &buf) < 0)	{
                        printf("VIDIOC_DQBUF failed.\n");
                }

                fwrite(buffers[buf.index].start, fmt.fmt.pix.sizeimage, 1, fd_y_file);
                process_image(buffers[buf.index].start,fmt.fmt.pix.sizeimage,fmt.fmt.pix.width,fmt.fmt.pix.height,fb0_var,fb0_fix);
                if (v4l2_camera.privew_count >= TEST_BUFFER_NUM) {
                        if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0) {
                                printf("VIDIOC_QBUF failed\n");
                                        break;
                        }
                }
                else
                        printf("buf.index %d\n", buf.index);
            }
            if (stop_capturing(fd_v4l) < 0)
            {
                    printf("stop_capturing failed\n");
                    goto err2;
            }
            if(ret == -2)
            {
err2:           close(fd_fb_0);
//                memset(screen_fbp,0,v4l2_camera.screen_size);
                memcpy(screen_fbp,screeninfo_tmp,v4l2_camera.screen_size);
                free(screeninfo_tmp);
                for(int i = 0;i < TEST_BUFFER_NUM;i ++)
                {
                    if(-1 == munmap(buffers[i].start, buffers[i].length))
                    {
                        printf("munmap - buffers is error \r\n");
                    }
                }
                if(-1 == munmap(screen_fbp, v4l2_camera.screen_size))
                {
                    printf("munmap - screen_fbp is error \r\n");
                }
                fclose(fd_y_file);
                close(fd_v4l);
                return TFAIL;
            }
            printf("\nSave the photos to '%s'\n",file);
//            memset(screen_fbp,0,v4l2_camera.screen_size);
            memcpy(screen_fbp,screeninfo_tmp,v4l2_camera.screen_size);
            free(screeninfo_tmp);
            for(int i = 0;i < TEST_BUFFER_NUM;i ++)
            {
                if(-1 == munmap(buffers[i].start, buffers[i].length))
                {
                    printf("munmap - buffers is error \r\n");
                }
            }
            if(-1 == munmap(screen_fbp, v4l2_camera.screen_size))
            {
                printf("munmap - screen_fbp is error \r\n");
            }
            fclose(fd_y_file);
            close(fd_fb_0);
        }
        if(ret == -1)
        {
err0:       close(fd_fb_0);
err1:       if(fd_v4l != 0)
            {
                close(fd_v4l);
            }
            return TFAIL;
        }        
        close(fd_v4l);
        return ret;
}

