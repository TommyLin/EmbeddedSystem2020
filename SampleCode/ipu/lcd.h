#ifndef _LCD_H_
#define _LCD_H_

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

#include <linux/mxcfb.h>
extern int fb_dbg_level;

#define FB_DPRINTF(level,fmt,arg...) if(fb_dbg_level >= level)\
    printf("[DEBUG]\t%s:%d " fmt, __FILE__, __LINE__, ## arg)

#define FB_ERR(fmt, arg...) do { if (fb_dbg_level >= 1)		\
        printf("[FB_ERR]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;\
        } while (0)
#define FB_INFO(fmt, arg...) do { if (fb_dbg_level >= 3)		\
        printf("[FB_INFO]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
         ;\
        } while (0)
#define FB_WARN(fmt, arg...) do { if (fb_dbg_level >= 2)		\
        printf("[FB_WARN]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;\
        } while (0)

#pragma pack(1)
typedef struct fb_dev_info{
    int fb_node;
    char fb_dev[100];
    int fb_fix_bbp;
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    struct mxcfb_color_key color_key;
    struct mxcfb_gbl_alpha g_alpha;
    struct mxcfb_loc_alpha l_alpha;
    char *fb_buf;
    char *fbuf_bak;
    int fb_bf_len;
}fb_dev_st;
#pragma pack()

//int fb_devinfo_init(struct fb_dev_info *fb_devinfo);
int fb_open(char *fb_dev);
int fb_close(int fb_dev);
//static int fb_get_vscreeninfo(int fb_dev,struct fb_var_screeninfo *fb_var);
//static int fb_get_fscreeninfo(int fb_dev,struct fb_fix_screeninfo *fb_fix);
int open_fb_dev(struct fb_dev_info *fb_devinfo);
int fb_apply_framebuffer(struct fb_dev_info *fb_devinfo);
int fb_check_fix_id(struct fb_dev_info *fb_devinfo);
int fb_set_fbl_alpha(struct fb_dev_info *fb_devinfo,unsigned char alpha,unsigned char color);



#endif
