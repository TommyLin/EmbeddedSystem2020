#ifndef _TQ_IPU_H_
#define _TQ_IPU_H_

#ifdef __cplusplus
extern "C"{
#endif

//#include "../linux/ipu.h"
#include "ipu.h"
#include <linux/fb.h>

extern int ipu_dbg_level;//ipu调试信息，已经编译成库，所以没有用

#define IPU_DPRINTF(level,fmt,arg...) if (ipu_dbg_level >= level)\
    printf("[IPU_DPRINTF]\t%s:%d " fmt, __FILE__, __LINE__, ## arg)

#define IPU_ERR(fmt, arg...) do { if (ipu_dbg_level >= 1)		\
        printf("[IPU_ERR]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;	\
        } while (0)
#define IPU_INFO(fmt, arg...) do { if (ipu_dbg_level >= 3)		\
        printf("[IPU_INFO]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;	\
        } while (0)
#define IPU_WARN(fmt, arg...) do { if (ipu_dbg_level >= 2)		\
        printf("[IPU_INFO]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
        ;	\
        } while (0)


typedef struct {
        struct ipu_task task;
        int show_to_fb;//屏幕显示标志
        char fb_name[128];//输出显示屏类型
        char bs_dev;//
} ipu_test_handle_t;

typedef struct ipu_devinfo{
    char ipu_dev[50];//ipu设备名称，默认/dev/mxc_ipu
    ipu_test_handle_t ipu_handle;//ipu句柄
    int dump_printf;//打开ipu task一些信息的打印功能
    int fd_ipu;//ipu打开的fb节点
    int fd_fb;//lcd设备节点
    int isize;//ipu/屏幕的大小
    int ovsize;//采用ovelay功能时，需要申请的缓存大小
    int alpsize;//Alpha 缓存大小
    int osize;//ipu处理后需要输出的数据的大小
    void *inbuf;//ipu输入缓存
    void *vdibuf;//ipu 去交织缓存
    void *ovbuf;//ipu overlay buf缓存--暂不支持
    void *alpbuf;//ipu alphablending buf缓存
    void *outbuf;//ipu 输出缓存
    struct fb_var_screeninfo fb_var;//显示屏信息
    struct fb_fix_screeninfo fb_fix;//显示屏信息

}ipu_devinfo_st;
/* **************************************************************************************
 * funcation: ipu_handle_init 初始化ipu句柄信息，此函数只是示例，使用时请在客户程序空间重新编写
 * 初始化信息，其中包含输入、输出等一些信息，具体详见注释、及ipu.h文件（官方）
 * input: ipu_test_handle_t *handle 指向目标句柄
 *        struct device_info v4l2_camera 传递来自v4l2摄像头的输入源信息
 * output: 无
 * return: 无
 * other:关于该库的API使用，如果已经初始化了当次的输入输出信息，若想更改为其信息的，必须先关闭ipu后再次重新操作
 * 操作流程：开始->ipu_handle_init->init_ipu->start_ipu[可不断循环]->close_ipu(结束)
 */
//void ipu_handle_init(ipu_test_handle_t *handle,struct device_info v4l2_camera)
//{
//    /*default settings*/
//    char pformat[5]={0};
//    sprintf(pformat,"%c%c%c%c",(v4l2_camera.sensor_format&0xff),((v4l2_camera.sensor_format>>8)&0xff),
//            ((v4l2_camera.sensor_format>>16)&0xff),((v4l2_camera.sensor_format>>24)&0xff));//获取摄像头格式
//    handle->task.priority = 0;
//    handle->task.task_id = 0;
//    handle->task.timeout = 150;
      /*注:format: 支持的数据格式，详见ipu.h第94~164行
       * width/height: 设置的像素必须与数据源一致,不然无法打开或段错误
       * crop: 裁剪框体或叫虚拟框体，其大小crop.x/y（+裁剪的起始位置crop.pos.x/y）必须在总的像素范围内，不然会出现段错误
       * deinterlace.motion：去交织的速度，如果定义为HIGH_MOTION，则去交织将无效果，或者效果不佳
       * 其它参数请保持例程中ipu_handle_init内的默认参数，谢谢。
       */
//    handle->task.input.width = v4l2_camera.sensor_width;//指定输入源像素
//    handle->task.input.height = v4l2_camera.sensor_height;
//    handle->task.input.format =  v4l2_fourcc(pformat[0],pformat[1],pformat[2],pformat[3]);//指定输入源格式eg：v4l2_fourcc('Y', 'U','Y', 'V');//v4l2_fourcc('R', 'G','B', 'P');//
//    handle->task.input.crop.pos.x = 0;//裁剪x起始位置
//    handle->task.input.crop.pos.y = 0;//裁剪y起始位置
//    handle->task.input.crop.w = v4l2_camera.sensor_width;//输入数据源裁剪宽度
//    handle->task.input.crop.h = v4l2_camera.sensor_height;//输入数据源裁剪高度
//    handle->task.input.deinterlace.enable = 1;//去交织使能
//    handle->task.input.deinterlace.motion = MED_MOTION;//去交织处理，详见ipu_motion_sel
//    handle->task.input.deinterlace.field_fmt = 0;//缺省

//    handle->task.output.width = v4l2_camera.display_width;//当前支持1920
//    handle->task.output.height = v4l2_camera.display_height;//当前支持1080
//    handle->task.output.format = IPU_PIX_FMT_RGB565;//v4l2_fourcc(pformat[0],pformat[1],pformat[2],pformat[3]);
//    handle->task.output.rotate = IPU_ROTATE_NONE;//旋转处理，详见：ipu_rotate_mode_t
//    handle->task.output.crop.pos.x = 0;//(v4l2_camera.display_width-v4l2_camera.sensor_width)/2;
//    handle->task.output.crop.pos.y = 0;//(v4l2_camera.display_height-v4l2_camera.sensor_height)/2;
//    handle->task.output.crop.w = 0;//输出数据裁剪的虚拟框宽度
//    handle->task.output.crop.h = 0;//输出数据裁剪的虚拟框高度

//    handle->show_to_fb = 1;//显示使能开关 如果开启显示，就不会执行转码输出
//    /*指定输出的显示屏  仅有五种，根据自己显示屏选择：DISP3 FG,DISP3 BG,DISP3 BG - DI1,DISP4 FG,DISP4 BG - DI1*/
//    /*memcpy(handle->fb_name,"DISP3 BG",sizeof("DISP3 BG"));//全屏覆盖界面的方式 应该是：fb0*/
//    memcpy(handle->fb_name,"DISP3 FG",sizeof("DISP3 FG"));//具有图层功能，为fb1设备

//    printf("\t input.width: %d,\n input.height: %d,\n input.format: %d,
//           \n output.width: %d,\n output.height: %d,\n output.format: %d r\n",
//           handle->task.input.width,handle->task.input.height,handle->task.input.format,
//           handle->task.output.width,handle->task.output.height,handle->task.output.format);
//}
/* **************************************************************************************
 * funcation: start_ipu 开始ipu图像信息处理，数据以一帧为处理单元
 * 函数内部自动识别ipu_handle->show_to_fb：
 *    为：1，帧数据处理后在LCD上显示屏；
 *    为：0，且file_out !=NULL 帧数据处理后写入到 file_out
 *          若file_out ==NULL ipu_info->outbuf 将是处理后的帧数据，ipu_info->osize为帧数据大小
 * input: u8 *image_target 指向需要被处理的目标缓存，可以是摄像头数据、文件内容，格式详见：ipu.h
 *        struct ipu_devinfo *ipu_info ipu信息的结构体
 * output: FILE *file_out ipu处理帧数据后输出到的文件目标
 * return: 0 成功
 * other:关于该库的API使用，如果已经初始化了当次的输入输出信息，若想更改为其信息的，必须先关闭ipu后再次重新操作
 * 操作流程：开始->ipu_handle_init->init_ipu->start_ipu[可不断循环]->close_ipu(结束)
 */
int start_ipu(unsigned char *image_target, FILE * file_out, struct ipu_devinfo *ipu_info);
/* **************************************************************************************
 * funcation: init_ipu 初始化ipu，内部打开ipu设备节点：/dev/mxc_ipu，显示屏节点/dev/fbx[x:0~4]
 * 使用前必须先初始化ipu_info结构体，其中包含输入格式、输入像素、去交织等，输出像素、
 * 输出到文件或者显示屏（二选一），图像旋转和其它的一些图像处理的参数，具体参见：ipu_handle_init函数
 * input: struct ipu_devinfo *ipu_info ipu设备信息
 * output: FILE *file_ipuout ipu处理图像后输出到的目标文件
 * return: 0 成功
 * other:关于该库的API使用，如果已经初始化了当次的输入输出信息，若想更改为其信息的，必须先关闭ipu后再次重新操作
 * 操作流程：开始->ipu_handle_init->init_ipu->start_ipu[可不断循环]->close_ipu(结束)
 */
int init_ipu(struct ipu_devinfo *ipu_info,FILE *file_ipuout);
/* **************************************************************************************
 * funcation: close_ipu 关闭ipu，释放所有申请的存储区及映射
 * input: ipu_info 被释放的ipu设备信息
 * output:
 * return: 0 成功
 * other:
 */
int close_ipu(struct ipu_devinfo *ipu_info);
#endif
