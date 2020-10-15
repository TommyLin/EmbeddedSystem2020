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

//#include "v4l2_camera.h"
#include <linux/mxcfb.h>
#include "../include/tq_ipu.h"

#include "lcd.h"
#include "ipu_test.h"


#define TFAIL -1
#define TPASS 0
int decoder_get_1frame_data_inputCallBack(int fd, void *vptr, size_t n);
int decoder_complete_1frame_data_outputCallBack(int fd, void *vptr, size_t n);
int encoder_deal_with_1frame_data_outputCallBack(int fd, void *vptr, size_t n);
int encoder_get_1frame_data_inputCallBack(int fd, void *vptr, size_t n);

void ipu_handle_init(ipu_test_handle_t *handle,struct cmd_line cmdl);
int ipu_test_init(struct input_argument input_argc);
int ipu_test_close(void);

int camera_dbg_level = 3;
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

struct ipu_devinfo ipu_info;
void ipu_handle_init(ipu_test_handle_t *handle,struct cmd_line cmdl)
{
    /*default settings*/

    handle->task.priority = 0;
    handle->task.task_id = 0;
    handle->task.timeout = 150;
    /*注:format: 支持的数据格式，详见ipu.h第94~164行
       * width/height: 设置的像素必须与数据源一致,不然无法打开或段错误
       * crop: 裁剪框体或叫虚拟框体，其大小crop.x/y（+裁剪的起始位置crop.pos.x/y）必须在总的像素范围内，不然会出现段错误
       * deinterlace.motion：去交织的速度，如果定义为HIGH_MOTION，则去交织将无效果，或者效果不佳
       * 其它参数请保持例程中ipu_handle_init内的默认参数，谢谢。
       */

    handle->task.input.width = cmdl.width;//指定输入源像素
    handle->task.input.height = cmdl.height;
    handle->task.input.format =  IPU_PIX_FMT_YUV420P;//IPU_PIX_FMT_YUV420P;//v4l2_fourcc(pformat[0],pformat[1],pformat[2],pformat[3]);//指定输入源格式eg：v4l2_fourcc('Y', 'U','Y', 'V');//v4l2_fourcc('R', 'G','B', 'P');//
    handle->task.input.crop.pos.x = 0;//裁剪x起始位置
    handle->task.input.crop.pos.y = 0;//裁剪y起始位置
    handle->task.input.crop.w = cmdl.width;//输入数据源裁剪宽度
    handle->task.input.crop.h = cmdl.height;//输入数据源裁剪高度
    handle->task.input.deinterlace.enable = 1;//去交织使能
    handle->task.input.deinterlace.motion = MED_MOTION;//去交织处理，详见ipu_motion_sel
    handle->task.input.deinterlace.field_fmt = 0;//缺省

    handle->task.output.width = 800;//当前支持1920
    handle->task.output.height = 480;//当前支持1080
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

    printf("\n input.width: %d,\n input.height: %d,\n input.format: %d,\
           \n output.width: %d,\n output.height: %d,\n output.format: %d r\n",
           handle->task.input.width,handle->task.input.height,handle->task.input.format,
           handle->task.output.width,handle->task.output.height,handle->task.output.format);
}
enum work_mode_en{
    wEncode = 0x40,
    wEncode_4file,//从文件中编码数据
    wEncode_4fdata,//编码帧数据
    wEncode_2file,//编码到文件
    wDecode = 0x80,//
    wDecode_2lcd,//解码到lcd
    wDecode_2file,//解码到文件
};
enum work_mode_en work_mode;
int 	/* write n bytes to a file descriptor */
decoder_complete_1frame_data_outputCallBack(int fd, void *vptr, size_t n)
{
    int nleft;
    int nwrite;
    char  *ptr;

    if(work_mode == wDecode_2lcd)
    {
        start_ipu((unsigned char *)vptr,NULL,&ipu_info);
    }
    else
    {
        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
            if ( (nwrite = write(fd, ptr, nleft)) <= 0) {
                perror("fwrite: ");
                return (-1);			/* error */
            }

            nleft -= nwrite;
            ptr   += nwrite;
        }
    }
    return (n);
} /* end decoder_complete_1frame_data_outputCallBack */
int	/* Read n bytes from a file descriptor */
decoder_get_1frame_data_inputCallBack(int fd, void *vptr, size_t n)
{
        int nleft = 0;
        int nread = 0;
        char  *ptr;

        ptr = vptr;
        nleft = n;
        while (nleft > 0) {//做了数据实际读取与预期要读数据的大小处理，返回实际读取的数据的长度，如果为0，说明完成了，会自动关闭vpu
                if ( (nread = read(fd, ptr, nleft)) <= 0) {
                        if (nread == 0)
                                return (n - nleft);

                        perror("read");
                        return (-1);			/* error EINTR XXX */
                }

                nleft -= nread;
                ptr   += nread;
        }

        return (n - nleft);
} /* end decoder_get_1frame_data_inputCallBack */

int 	/* write n bytes to a file descriptor */
encoder_deal_with_1frame_data_outputCallBack(int fd, void *vptr, size_t n)
{
    int nleft;
    int nwrite;
    char  *ptr;

    if(work_mode == wDecode)
    {
        start_ipu((unsigned char *)vptr,NULL,&ipu_info);
    }
    else
    {
        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
            if ( (nwrite = write(fd, ptr, nleft)) <= 0) {
                perror("fwrite: ");
                return (-1);			/* error */
            }

            nleft -= nwrite;
            ptr   += nwrite;
        }
    }
    return (n);
} /* end encoder_deal_with_1frame_data_outputCallBack */
int	/* Read n bytes from a file descriptor */
encoder_get_1frame_data_inputCallBack(int fd, void *vptr, size_t n)
{
        int nleft = 0;
        int nread = 0;
        char  *ptr;

        ptr = vptr;
        nleft = n;
        while (nleft > 0) {//做了数据实际读取与预期要读数据的大小处理，返回实际读取的数据的长度，如果为0，说明完成了，会自动关闭vpu
                if ( (nread = read(fd, ptr, nleft)) <= 0) {
                        if (nread == 0)
                                return (n - nleft);

                        perror("read");
                        return (-1);			/* error EINTR XXX */
                }

                nleft -= nread;
                ptr   += nread;
        }

        return (n - nleft);
} /* end encoder_get_1frame_data_inputCallBack */

struct fb_dev_info fb_info={
    .fb_dev = "/dev/fb0",
    .fb_buf = NULL,
};
int ipu_test_init(struct input_argument input_argc)
{
    FILE *file_ipuout=NULL;
    int err = -1;
    work_mode = wDecode_2lcd;
    if(open_fb_dev(&fb_info)<0)
    {
        CAMERA_ERR("open fb dev is failure\r\n");
        return err;
    }
    if(fb_apply_framebuffer(&fb_info) >= 0)
    {
        fb_check_fix_id(&fb_info);
        ipu_handle_init(&ipu_info.ipu_handle,input_argc.cmd);
        if(init_ipu(&ipu_info,file_ipuout) < 0)
        {
            CAMERA_WARN("ipu init is failed\n");
        }
        err = 0;
    }else
    {
        CAMERA_ERR("fb apply framebuffer is failure \r\n");
        if(fb_info.fbuf_bak)
        {
            memcpy(fb_info.fb_buf,fb_info.fbuf_bak,fb_info.fb_bf_len);
            free(fb_info.fbuf_bak);
        }
        if(fb_info.fb_buf)
        {
            munmap(fb_info.fb_buf, fb_info.fb_bf_len);
        }

        if(fb_info.fb_node)
        {
            close(fb_info.fb_node);
        }
    }
    return err;
}

int ipu_test_close(void)
{
    work_mode = wEncode;
    if(close_ipu(&ipu_info)<0)
    {
        CAMERA_WARN("ipu close is failed\n");
    }
    if(fb_info.fbuf_bak)
    {
        memcpy(fb_info.fb_buf,fb_info.fbuf_bak,fb_info.fb_bf_len);
        free(fb_info.fbuf_bak);
    }
    if(fb_info.fb_buf)
    {
        munmap(fb_info.fb_buf, fb_info.fb_bf_len);
    }
    if(fb_info.fb_node)
    {
        close(fb_info.fb_node);
    }
    return 0;
}
