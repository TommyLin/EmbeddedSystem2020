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

#include "soc_check.h"
#include "v4l2_camera.h"
#include <linux/mxcfb.h>

#include "lcd.h"
#include "include/ipu.h"
#include "include/tq_ipu.h"

#define TFAIL -1
#define TPASS 0

int camera_dbg_level = 1;
#define CAMERA_DPRINTF(level,fmt,arg...) if(camera_dbg_level >= level)\
    printf("[DEBUG]\t%s:%d " fmt, __FILE__, __LINE__, ## arg)

#define CAMERA_ERR(fmt, arg...) do { if (camera_dbg_level >= 1)		\
        printf("[CAMERA_ERR]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;	\
        } while (0)
#define CAMERA_INFO(fmt, arg...) do { if (camera_dbg_level >= 3)		\
        printf("[CAMERA_INFO]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;	\
        } while (0)
#define CAMERA_WARN(fmt, arg...) do { if (camera_dbg_level >= 2)		\
        printf("[CAMERA_WARN]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;	\
        } while (0)

#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

//#define IPU_PIX_FMT_RGB332  ipu_fourcc('R','G','B','1') /*!<  8  RGB-3-3-2     */
//#define IPU_PIX_FMT_RGB555  ipu_fourcc('R','G','B','O') /*!< 16  RGB-5-5-5     */
//#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P') /*!< 16  RGB-5-6-5     */
//#define IPU_PIX_FMT_RGB666  ipu_fourcc('R','G','B','6') /*!< 18  RGB-6-6-6     */
//#define IPU_PIX_FMT_BGR24   ipu_fourcc('B','G','R','3') /*!< 24  BGR-8-8-8     */
//#define IPU_PIX_FMT_RGB24   ipu_fourcc('R','G','B','3') /*!< 24  RGB-8-8-8     */
//#define IPU_PIX_FMT_BGR32   ipu_fourcc('B','G','R','4') /*!< 32  BGR-8-8-8-8   */
//#define IPU_PIX_FMT_BGRA32  ipu_fourcc('B','G','R','A') /*!< 32  BGR-8-8-8-8   */
//#define IPU_PIX_FMT_RGB32   ipu_fourcc('R','G','B','4') /*!< 32  RGB-8-8-8-8   */
//#define IPU_PIX_FMT_RGBA32  ipu_fourcc('R','G','B','A') /*!< 32  RGB-8-8-8-8   */
//#define IPU_PIX_FMT_ABGR32  ipu_fourcc('A','B','G','R') /*!< 32  ABGR-8-8-8-8  */

char v4l_device[100] = "/dev/video0";
int fd_v4l = 0;



device_info_st v4l2_camera;
struct fb_dev_info fb_info;
#define VIDEO_CLEAR(x) memset(&(x), 0, sizeof(x))
static void print_pixelformat(char *prefix, int val)
{
        printf("==== %s: %c%c%c%c\n", prefix ? prefix : "pixelformat",
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
//#ifdef BUILD_FOR_ANDROID
//        char fb_device_0[100] = "/dev/graphics/fb0";
//#else
//        char fb_device_0[100] = "/dev/fb0";
//#endif
        int fd_graphic_fb = 0;


        if (ioctl(fd_v4l, VIDIOC_OVERLAY, &overlay) < 0) {
                printf("VIDIOC_OVERLAY start failed\n");
                retval = TFAIL;
                goto out1;
        }

        CAMERA_INFO("\n[funtion- %s] is run...\r\n",__FUNCTION__);

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

//out0:

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
                v4l2_camera.sensor_format = ffmt.pixelformat;
                printf("==== %s: %c%c%c%c\n", "+++++++++++pixelformat",
                                                v4l2_camera.sensor_format & 0xff,
                                                (v4l2_camera.sensor_format >> 8) & 0xff,
                                                (v4l2_camera.sensor_format >> 16) & 0xff,
                                                (v4l2_camera.sensor_format >> 24) & 0xff);
                ffmt.index++;
        }
//        v4l2_camera.sensor_format = ffmt.pixelformat;

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
            ctl.id = V4L2_CID_PRIVATE_BASE + 2;
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
                return TFAIL;
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
                v4l2_camera.sensor_format = fmt->fmt.pix.pixelformat;
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
    unsigned char* in=(unsigned char*)addr;
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
            fb_info.fb_buf[ location + 0] = clip(b, 0, 255);
            fb_info.fb_buf[ location + 1] = clip(g, 0, 255);
            fb_info.fb_buf[ location + 2] = clip(r, 0, 255);

            r = (298 * y1 + 409 * v + 128) >> 8;
            g = (298 * y1 - 100 * u - 208 * v + 128) >> 8;
            b = (298 * y1 + 516 * u + 128) >> 8;
            fb_info.fb_buf[ location + 3] = clip(b, 0, 255);
            fb_info.fb_buf[ location + 4] = clip(g, 0, 255);
            fb_info.fb_buf[ location + 5] = clip(r, 0, 255);
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
        CAMERA_INFO("\n[funtion- %s] is run...\r\n",__FUNCTION__);
        if (ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0)
        {
                printf("v4l_capture_setup: VIDIOC_REQBUFS failed\n");
                return TFAIL;
        }
        CAMERA_INFO("VIDIOC_REQBUFS is ok \r\n");
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
        CAMERA_INFO("TEST_BUFFER_NUM 1 is ok \r\n");
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
        CAMERA_INFO("TEST_BUFFER_NUM 2 is ok \r\n");
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

void init_v4l2_paramet(void)
{
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
    v4l2_camera.privew_timeout = 50;
    v4l2_camera.privew_count = 50;
}
int vidioc_querycap(int v4l2_dev)
{
    struct v4l2_capability cap;

    if(ioctl(fd_v4l, VIDIOC_QUERYCAP, &cap) < 0){
        printf("VIDIOC_QUERYCAP is error  \n");
        return TFAIL;
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
    return TPASS;
}

struct ipu_devinfo ipu_info;
void ipu_handle_init(ipu_test_handle_t *handle,struct device_info v4l2_camera)
{
    /*default settings*/
    char pformat[5]={0};
    sprintf(pformat,"%c%c%c%c",(v4l2_camera.sensor_format&0xff),((v4l2_camera.sensor_format>>8)&0xff),
            ((v4l2_camera.sensor_format>>16)&0xff),((v4l2_camera.sensor_format>>24)&0xff));//获取摄像头格式
    handle->task.priority = 0;
    handle->task.task_id = 0;
    handle->task.timeout = 150;
      /*注:format: 支持的数据格式，详见ipu.h第94~164行
       * width/height: 设置的像素必须与数据源一致,不然无法打开或段错误
       * crop: 裁剪框体或叫虚拟框体，其大小crop.x/y（+裁剪的起始位置crop.pos.x/y）必须在总的像素范围内，不然会出现段错误
       * deinterlace.motion：去交织的速度，如果定义为HIGH_MOTION，则去交织将无效果，或者效果不佳
       * 其它参数请保持例程中ipu_handle_init内的默认参数，谢谢。
       */
    handle->task.input.width = v4l2_camera.sensor_width;//指定输入源像素
    handle->task.input.height = v4l2_camera.sensor_height;
    handle->task.input.format =  v4l2_fourcc(pformat[0],pformat[1],pformat[2],pformat[3]);//指定输入源格式eg：v4l2_fourcc('Y', 'U','Y', 'V');//v4l2_fourcc('R', 'G','B', 'P');//
    handle->task.input.crop.pos.x = 0;//裁剪x起始位置
    handle->task.input.crop.pos.y = 0;//裁剪y起始位置
    handle->task.input.crop.w = v4l2_camera.sensor_width;//输入数据源裁剪宽度
    handle->task.input.crop.h = v4l2_camera.sensor_height;//输入数据源裁剪高度
    handle->task.input.deinterlace.enable = 1;//去交织使能
    handle->task.input.deinterlace.motion = MED_MOTION;//去交织处理，详见ipu_motion_sel
    handle->task.input.deinterlace.field_fmt = 0;//缺省

    handle->task.output.width = v4l2_camera.display_width;//当前支持1920
    handle->task.output.height = v4l2_camera.display_height;//当前支持1080
    handle->task.output.format = IPU_PIX_FMT_RGB565;//v4l2_fourcc(pformat[0],pformat[1],pformat[2],pformat[3]);
    handle->task.output.rotate = IPU_ROTATE_NONE;//旋转处理，详见：ipu_rotate_mode_t
    handle->task.output.crop.pos.x = 0;//(v4l2_camera.display_width-v4l2_camera.sensor_width)/2;
    handle->task.output.crop.pos.y = 0;//(v4l2_camera.display_height-v4l2_camera.sensor_height)/2;
    handle->task.output.crop.w = 0;//输出数据裁剪的虚拟框宽度
    handle->task.output.crop.h = 0;//输出数据裁剪的虚拟框高度

    handle->show_to_fb = 1;//显示使能开关 如果开启显示，就不会执行转码输出
    /*指定输出的显示屏  仅有五种，根据自己显示屏选择：DISP3 FG,DISP3 BG,DISP3 BG - DI1,DISP4 FG,DISP4 BG - DI1*/
    /*memcpy(handle->fb_name,"DISP3 BG",sizeof("DISP3 BG"));//全屏覆盖界面的方式 应该是：fb0*/
    memcpy(handle->fb_name,"DISP3 FG",sizeof("DISP3 FG"));//具有图层功能，为fb1设备

    printf("\t input.width: %d,\n input.height: %d,\n input.format: %d,\
           \n output.width: %d,\n output.height: %d,\n output.format: %d r\n",
           handle->task.input.width,handle->task.input.height,handle->task.input.format,
           handle->task.output.width,handle->task.output.height,handle->task.output.format);
}

int
main(int argc, char **argv)
{
        struct v4l2_format fmt;
        struct v4l2_framebuffer fb_v4l2;
        int ret = -1;
        struct sigaction act;
        static struct v4l2_dbg_chip_ident v_chip;
        FILE * fd_y_file = 0;
        FILE *file_ipuout=NULL;
        char *file = "/v4l2_test.yuv";
        struct v4l2_buffer buf;
#ifdef BUILD_FOR_ANDROID
        memcpy(fb_info.fb_dev,"/dev/graphics/fb0",sizeof("/dev/graphics/fb0"));
#else
        memcpy(fb_info.fb_dev,"/dev/fb0",sizeof("/dev/fb0"));
#endif
        init_v4l2_paramet();
        /* for ctrl-c */
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = ctrl_c_handler;
        if((ret = sigaction(SIGINT, &act, NULL)) < 0) {
                CAMERA_ERR("install sigal error\n");
                return TFAIL;
        }
        if (process_cmdline(argc, argv) < 0) {
                return TFAIL;
        }
        if ((fd_v4l = open(v4l_device, O_RDWR, 0)) < 0) {
                CAMERA_ERR("Unable to open %s\n", v4l_device);
                return TFAIL;
        }
        vidioc_querycap(fd_v4l);
        if(open_fb_dev(&fb_info)<0)
        {
            CAMERA_ERR("open fb dev is failure\r\n");
            goto err1;
        }
        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (ioctl(fd_v4l, VIDIOC_DBG_G_CHIP_IDENT, &v_chip))
            {
                    printf("VIDIOC_DBG_G_CHIP_IDENT failed.\n");
                    goto err2;
            }
            printf("sensor v_chip is %s\n", v_chip.match.name);
        }else
        {
            if(fb_apply_framebuffer(&fb_info) < 0)
            {
                CAMERA_ERR("fb apply framebuffer is failure \r\n");
                goto err3;
            }
        }
        fb_check_fix_id(&fb_info);
        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._output) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (ioctl(fd_v4l, VIDIOC_S_OUTPUT, &v4l2_camera.screen_fb) < 0)
            {
                CAMERA_ERR("VIDIOC_S_OUTPUT failed\n");
            }
        }
        fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
        fmt.fmt.win.w.top=  v4l2_camera.display_top ;
        fmt.fmt.win.w.left= v4l2_camera.display_left;
        fmt.fmt.win.w.width=v4l2_camera.display_width;
        fmt.fmt.win.w.height=v4l2_camera.display_height;
        if (mxc_v4l_overlay_setup(&fmt) < 0) {
            CAMERA_ERR("Setup overlay failed.\n");
            goto err4;
        }
        memset(&fb_v4l2, 0, sizeof(fb_v4l2));
        if((v4l2_camera.v4l2_dev._bit._overlay) || (v4l2_camera.v4l2_dev._bit._output) || (v4l2_camera.v4l2_dev._bit._read_write))
        {
            if (!v4l2_camera.g_overlay) {

                fb_set_fbl_alpha(&fb_info,255,0);

                fb_v4l2.fmt.width = fb_info.fb_var.xres;
                fb_v4l2.fmt.height = fb_info.fb_var.yres;

                if (fb_info.fb_var.bits_per_pixel == 32) {
                    fb_v4l2.fmt.pixelformat = IPU_PIX_FMT_BGR32;
                    fb_v4l2.fmt.bytesperline = 4 * fb_v4l2.fmt.width;
                }
                else if (fb_info.fb_var.bits_per_pixel == 24) {
                    fb_v4l2.fmt.pixelformat = IPU_PIX_FMT_BGR24;
                    fb_v4l2.fmt.bytesperline = 3 * fb_v4l2.fmt.width;
                }
                else if (fb_info.fb_var.bits_per_pixel == 16) {
                    fb_v4l2.fmt.pixelformat = IPU_PIX_FMT_RGB565;
                    fb_v4l2.fmt.bytesperline = 2 * fb_v4l2.fmt.width;
                }

                fb_v4l2.flags = V4L2_FBUF_FLAG_PRIMARY;
                fb_v4l2.base = (void *) fb_info.fb_fix.smem_start +
                        fb_info.fb_fix.line_length*fb_info.fb_var.yoffset;
            } else {
                fb_set_fbl_alpha(&fb_info,0,1);

                if (ioctl(fd_v4l, VIDIOC_G_FBUF, &fb_v4l2) < 0) {
                    CAMERA_ERR("Get framebuffer failed\n");
                    goto err4;
                }
                fb_v4l2.flags = V4L2_FBUF_FLAG_OVERLAY;
            }
            if (ioctl(fd_v4l, VIDIOC_S_FBUF, &fb_v4l2) < 0)
            {
                CAMERA_ERR("set framebuffer failed\n");
                goto err4;
            }
            if (ioctl(fd_v4l, VIDIOC_G_FBUF, &fb_v4l2) < 0) {
                CAMERA_ERR("set framebuffer failed\n");
                goto err4;
            }
            printf("\n frame buffer width %d, height %d, bytesperline %d\n",
                    fb_v4l2.fmt.width, fb_v4l2.fmt.height, fb_v4l2.fmt.bytesperline);
            ret = mxc_v4l_overlay_test(v4l2_camera.privew_timeout);
        }else
        {
            printf("\n======== ipu_handle_init is start========\r\n");
            ipu_handle_init(&ipu_info.ipu_handle,v4l2_camera);
            ipu_info.dump_printf = 1;
            if ((fd_y_file = fopen(file, "wb")) ==NULL)
            {
                CAMERA_ERR("Unable to create y frame recording file\n");
                goto err5;
            }
            if ((file_ipuout = fopen("/ipu-test.rgb", "wb")) ==NULL)
            {
                CAMERA_ERR("Unable to create y frame recording file_ipuout\n");
                goto err5;
            }

            if (start_capturing(fd_v4l) < 0)
            {
                CAMERA_ERR("start_capturing failed\n");
                goto err6;
            }
            if(init_ipu(&ipu_info,file_ipuout) < 0)
            {
                CAMERA_WARN("ipu init is failed\n");
            }
            while((v4l2_camera.privew_count)&& (v4l2_camera.g_ctrl_c_rev == 0))
            {

                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                if (ioctl (fd_v4l, VIDIOC_DQBUF, &buf) < 0)	{
                    CAMERA_WARN("VIDIOC_DQBUF failed.\n");
                }
                start_ipu(buffers[buf.index].start,file_ipuout,&ipu_info);
                if (v4l2_camera.privew_count >= TEST_BUFFER_NUM) {
                    if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0) {
                        CAMERA_ERR("VIDIOC_QBUF failed\n");
                        break;
                    }
                }
                else
                    printf("buf.index %d\n", buf.index);
            }
            CAMERA_INFO("v4l2_camera.privew_count is ok \r\n");
            if (stop_capturing(fd_v4l) < 0)
            {
                    CAMERA_ERR("stop_capturing failed\n");
                    goto err6;
            }
        }
        if(close_ipu(&ipu_info)<0)
        {
            CAMERA_WARN("ipu close is failed\n");
        }
err6 :
err5 :
        for(int i = 0;i < TEST_BUFFER_NUM;i ++)
        {
            if(buffers[i].start)
                if(-1 == munmap(buffers[i].start, buffers[i].length))
                {
                    CAMERA_WARN("munmap - buffers is error \r\n");
                }
        }
        if(fd_y_file)
        {
            fclose(fd_y_file);
        }
err4 :
err3 :
        if(fb_info.fbuf_bak)
        {
            memcpy(fb_info.fb_buf,fb_info.fbuf_bak,fb_info.fb_bf_len);
            free(fb_info.fbuf_bak);
        }
        if(fb_info.fb_buf)
        {
            munmap(fb_info.fb_buf, fb_info.fb_bf_len);
        }

err2 :
        if(fb_info.fb_node)
        {
            close(fb_info.fb_node);            
        }
err1 :
        if(fd_v4l)
        {
            close(fd_v4l);
        }
        if(ret == 0)
            return ret;
        else
            return TFAIL;
}

