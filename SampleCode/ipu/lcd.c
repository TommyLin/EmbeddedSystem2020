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

#include <asm/types.h>
//#include <linux/videodev2.h>
#include "videodev2.h"
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
//#include "mxc_ipudev_test.h"


#include <linux/mxcfb.h>
#include "lcd.h"

#define TFAIL -1
#define TPASS 0

int fb_dbg_level = 1;

struct fb_dev_info *fb_info;
//int fb_devinfo_init(struct fb_dev_info *fb_devinfo)
//{

//    fb_info =(struct fb_dev_info*)fb_devinfo;

//}
int fb_open(char *fb_dev)
{
    return open(fb_dev, O_RDWR );
}
int fb_close(int fb_dev)
{
    if(fb_dev)
    {
        close(fb_dev);
    }
    return TPASS;
}
static int fb_get_vscreeninfo(int fb_dev,struct fb_var_screeninfo *fb_var)
{
    if (ioctl(fb_dev, FBIOGET_VSCREENINFO, fb_var) < 0) {
        return TFAIL;
    }
    return TPASS;
}
static int fb_get_fscreeninfo(int fb_dev,struct fb_fix_screeninfo *fb_fix)
{
    if (ioctl(fb_dev, FBIOGET_FSCREENINFO, fb_fix) < 0) {
        return TFAIL;
    }
    return TPASS;
}

int open_fb_dev(struct fb_dev_info *fb_devinfo)
{
    int ret = 0;
    fb_devinfo->fb_node = fb_open(fb_devinfo->fb_dev);
    if(fb_devinfo->fb_node<0)
    {
        FB_ERR("fb open is failure \r\n");
    }
    FB_INFO("fb_open ：%d is ok\r\n",fb_devinfo->fb_node);
    ret = fb_get_vscreeninfo(fb_devinfo->fb_node,(struct fb_var_screeninfo*)&(fb_devinfo->fb_var));
    if(ret < 0)
    {
        close(fb_devinfo->fb_node);
        FB_ERR("fb get vscreeninfo is failure \r\n");
    }
    FB_INFO("fb_get_vscreeninfo is ok\r\n");
    ret = fb_get_fscreeninfo(fb_devinfo->fb_node,&fb_devinfo->fb_fix);
    if(ret < 0)
    {
        close(fb_devinfo->fb_node);
        FB_ERR("fb get fscreeninfo is failure \r\n");
    }
    FB_INFO("fb_get_fscreeninfo is ok\r\n");
    return ret;
}

int fb_apply_framebuffer(struct fb_dev_info *fb_devinfo)
{
    fb_devinfo->fb_bf_len = fb_devinfo->fb_var.xres * fb_devinfo->fb_var.yres *fb_devinfo->fb_var.bits_per_pixel / 8;
    fb_devinfo->fbuf_bak = malloc(sizeof(char) * fb_devinfo->fb_bf_len);
    if(!fb_devinfo->fbuf_bak)
    {
        FB_ERR("fb apply framebuffer is failure \r\n");
        return TFAIL;
    }
    fb_devinfo->fb_buf = (char*)mmap(0,fb_devinfo->fb_bf_len,PROT_READ | PROT_WRITE,MAP_SHARED, fb_devinfo->fb_node, 0);
    if(!fb_devinfo->fb_buf)
    {
        free(fb_devinfo->fb_buf);
        FB_ERR("fb mmap framebuffer is failure \r\n");
        return TFAIL;
    }
    memcpy(fb_devinfo->fbuf_bak,fb_devinfo->fb_buf,fb_devinfo->fb_bf_len);
    memset(fb_devinfo->fb_buf,0,fb_devinfo->fb_bf_len);
    return TPASS;
}

int fb_check_fix_id(struct fb_dev_info *fb_devinfo)
{
    if (strcmp(fb_devinfo->fb_fix.id, "DISP3 BG - DI1") == 0)
            fb_devinfo->fb_fix_bbp = 1;
    else if (strcmp(fb_devinfo->fb_fix.id, "DISP3 BG") == 0)
            fb_devinfo->fb_fix_bbp = 0;
    else if (strcmp(fb_devinfo->fb_fix.id, "DISP4 BG") == 0)
            fb_devinfo->fb_fix_bbp = 3;
    else if (strcmp(fb_devinfo->fb_fix.id, "DISP4 BG - DI1") == 0)
            fb_devinfo->fb_fix_bbp = 4;
    FB_INFO("fbx fix id: %s \n bbp: %d \r\n",fb_devinfo->fb_fix.id,fb_devinfo->fb_fix_bbp);
    return fb_devinfo->fb_fix_bbp;
}

int fb_set_fbl_alpha(struct fb_dev_info *fb_devinfo,unsigned char alpha,unsigned char color)
{
    fb_devinfo->g_alpha.alpha = alpha;
    fb_devinfo->g_alpha.enable = 1;
    if (ioctl(fb_devinfo->fb_node, MXCFB_SET_GBL_ALPHA, &fb_devinfo->g_alpha) < 0) {
        FB_ERR("set global alpha failed\n");
        return TFAIL;
    }
    if(color)
    {
        fb_devinfo->color_key.color_key = 0x00080808;
        fb_devinfo->color_key.enable = 0;
        if (ioctl(fb_devinfo->fb_node, MXCFB_SET_CLR_KEY, &fb_devinfo->color_key) < 0) {
            FB_ERR("set color key failed\n");
            return TFAIL;
        }
    }
    return TPASS;
}











