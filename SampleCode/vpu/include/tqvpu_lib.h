/*
 * Copyright 2004-2014 Freescale Semiconductor, Inc.
 *
 * Copyright (c) 2006, Chips & Media.  All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _TQVPU_LIB_H
#define _TQVPU_LIB_H

#include <linux/videodev2.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <semaphore.h>
#include "../include/mxc_ipu_hl_lib.h"
#include "../include/vpu_lib.h"
#include "../include/vpu_io.h"
#ifdef BUILD_FOR_ANDROID
#include "g2d.h"
#endif


#define COMMON_INIT

extern int vpu_test_dbg_level;//调试信息级别开关

#define dprintf(level, fmt, arg...)     if (vpu_test_dbg_level >= level) \
        printf("[DEBUG]\t%s:%d " fmt, __FILE__, __LINE__, ## arg)

#define err_msg(fmt, arg...) do { if (vpu_test_dbg_level >= 1)		\
	printf("[ERR]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
	printf("[ERR]\t" fmt, ## arg);	\
	} while (0)
#define info_msg(fmt, arg...) do { if (vpu_test_dbg_level >= 1)		\
	printf("[INFO]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
	printf("[INFO]\t" fmt, ## arg);	\
	} while (0)
#define warn_msg(fmt, arg...) do { if (vpu_test_dbg_level >= 1)		\
	printf("[WARN]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
	printf("[WARN]\t" fmt, ## arg);	\
	} while (0)

#undef u32
#undef u16
#undef u8
#undef s32
#undef s16
#undef s8
typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long s32;
typedef short s16;
typedef char s8;

#define SZ_4K			(4 * 1024)

#define STREAM_BUF_SIZE		0x200000
#define STREAM_FILL_SIZE	0x40000
#define STREAM_READ_SIZE	(512 * 8)
#define STREAM_END_SIZE		0
#define PS_SAVE_SIZE		0x080000
#define VP8_MB_SAVE_SIZE	0x080000
#define MPEG4_SCRATCH_SIZE	0x080000
#define MJPG_FILL_SIZE		(8 * 1024)

#define STREAM_ENC_PIC_RESET 	1

#define PATH_V4L2	0
#define PATH_FILE	1
#define PATH_NET	2
#define PATH_IPU	3
#ifdef BUILD_FOR_ANDROID
#define PATH_G2D	4
#endif
#define PATH_BUFFER     5

/* Test operations */
#define ENCODE		1
#define DECODE		2
#define LOOPBACK	3
#define TRANSCODE	4

#define DEFAULT_PORT		5555
#define DEFAULT_PKT_SIZE	0x28000

#define SIZE_USER_BUF            0x1000
#define USER_DATA_INFO_OFFSET    8*17

enum {
    MODE420 = 0,
    MODE422 = 1,
    MODE224 = 2,
    MODE444 = 3,
    MODE400 = 4
};

struct frame_buf {//--内部数据，用户不需要考虑
	int addrY;
	int addrCb;
	int addrCr;
	int strideY;
	int strideC;
	int mvColBuf;
	vpu_mem_desc desc;
};

struct v4l_buf {//--内部数据，用户不需要考虑
	void *start;
	off_t offset;
	size_t length;
};

#define MAX_BUF_NUM	32
#define QUEUE_SIZE	(MAX_BUF_NUM + 1)
struct v4l_specific_data {
	struct v4l2_buffer buf;
	struct v4l_buf *buffers[MAX_BUF_NUM];
};

#ifdef BUILD_FOR_ANDROID
struct g2d_specific_data {
	struct g2d_buf *g2d_bufs[MAX_BUF_NUM];
};
#endif

struct buf_queue {//--内部数据，用户不需要考虑
	int list[MAX_BUF_NUM + 1];
	int head;
	int tail;
};

struct ipu_buf {//--内部数据，用户不需要考虑
	int ipu_paddr;
	void * ipu_vaddr;
	int field;
};

struct vpu_display {//--内部数据，用户不需要考虑
	int fd;
	int nframes;
	int ncount;
	time_t sec;
	int queued_count;
	int dequeued_count;
	suseconds_t usec;
	int frame_size;

	pthread_t disp_loop_thread;

	sem_t avaiable_decoding_frame;
	sem_t avaiable_dequeue_frame;

	struct buf_queue display_q;
	struct buf_queue released_q;
	int stopping;
	int deinterlaced;
	void *render_specific_data;

	/* ipu lib renderer */
        ipu_lib_handle_t ipu_handle;
        ipu_lib_input_param_t input;
        ipu_lib_output_param_t output;
	pthread_t ipu_disp_loop_thread;
	struct buf_queue ipu_q;
	struct ipu_buf ipu_bufs[MAX_BUF_NUM];
};

struct capture_testbuffer {//--内部数据，用户不需要考虑
	size_t offset;
	unsigned int length;
};

struct rot {
        int rot_en;//选转使能
        int ext_rot_en;//扩展选转使能
        int rot_angle;//旋转角度 0 90 180 270 360
};

#define MAX_PATH	256
struct cmd_line {
	char input[MAX_PATH];	/* Input file name */
	char output[MAX_PATH];  /* Output file name */
        int src_scheme;//输入源方案：PATH_FILE文件，PATH_NET网络
        int dst_scheme;//输出源方案：PATH_FILE，PATH_NET
        int video_node;//--内部数据，用户不需要考虑
        int video_node_capture;//--内部数据，用户不需要考虑
        int src_fd;//用于input file 打开的描述符
        int dst_fd;//用于output file 打开的描述符
        int width;//输入源宽度
        int height;//输入源高度
        int enc_width;//编码后指定宽度
        int enc_height;//编码后指定高度
        int loff;//显示屏左起始点
        int toff;//显示屏顶部起始点
        int format;//格式：详见CodStd
        int deblock_en;//mx31之后不用
        int dering_en;//跳转使能
	int rot_en; /* Use VPU to do rotation */
	int ext_rot_en; /* Use IPU/GPU to do rotation */
        int rot_angle;//旋转角度90，180，270 360
        int mirror;//与旋转功能类型，具体不再详列
        int chromaInterleave;//色度交错 与mapType 连用
        int bitrate;//比特率
        int gop;//--内部数据，用户不需要考虑
        int save_enc_hdr;//--内部数据，用户不需要考虑
        int count;//--内部数据，用户不需要考虑
        int prescan;//预处理使能
        int bs_mode;//bitstream mode
	char *nbuf; /* network buffer */
	int nlen; /* remaining data in network buffer */
	int noffset; /* offset into network buffer */
	int seq_no; /* seq numbering to detect skipped frames */
	u16 port; /* udp port number */
	u16 complete; /* wait for the requested buf to be filled completely */
	int iframe;
	int mp4_h264Class;
	char vdi_motion;	/* VDI motion algorithm */
        int fps;//帧率
        int mapType;//参数选择,详见：GDI_TILED_MAP_TYPE
        int quantParam;//量化参数
};

struct decode {
        DecHandle handle;//解码句柄信息，--内部数据，用户不需要考虑
        PhysicalAddress phy_bsbuf_addr;//--内部数据，用户不需要考虑
        PhysicalAddress phy_ps_buf;//--内部数据，用户不需要考虑
        PhysicalAddress phy_slice_buf;//--内部数据，用户不需要考虑
        PhysicalAddress phy_vp8_mbparam_buf;//--内部数据，用户不需要考虑

        int phy_slicebuf_size;//--内部数据，用户不需要考虑
        int phy_vp8_mbparam_size;//--内部数据，用户不需要考虑
        u32 virt_bsbuf_addr;//--内部数据，用户不需要考虑
        int picwidth;//--内部数据，用户不需要考虑
        int picheight;//--内部数据，用户不需要考虑
        int stride;//--内部数据，用户不需要考虑
        int mjpg_fmt;//--内部数据，用户不需要考虑
        int regfbcount;//--内部数据，用户不需要考虑
        int minfbcount;//--内部数据，用户不需要考虑
        int rot_buf_count;//--内部数据，用户不需要考虑
        int extrafb;//--内部数据，用户不需要考虑
        FrameBuffer *fb;//
        struct frame_buf **pfbpool;//--内部数据，用户不需要考虑
        struct vpu_display *disp;//--内部数据，用户不需要考虑
        vpu_mem_desc *mvcol_memdesc;//--内部数据，用户不需要考虑
        Rect picCropRect;//--内部数据，用户不需要考虑
        int reorderEnable;//--内部数据，用户不需要考虑
        int tiled2LinearEnable;//--内部数据，用户不需要考虑
        int post_processing;//--内部数据，用户不需要考虑

        DecReportInfo mbInfo;//--内部数据，用户不需要考虑
        DecReportInfo mvInfo;//--内部数据，用户不需要考虑
        DecReportInfo frameBufStat;//--内部数据，用户不需要考虑
        DecReportInfo userData;//--内部数据，用户不需要考虑

        struct cmd_line *cmdl;//参加cmd_line

        int decoded_field[32];//解码文件
        int lastPicWidth;//图片最后的宽度
        int lastPicHeight;//图片最后的高度

        int mjpgLineBufferMode;//mjpg线缓存模式
        u32 mjpg_wr_ptr;//mjpeg写指针，内部处理
        u32 mjpg_rd_ptr;//mjpeg读指针，内部处理
	int mjpg_sc_state; /* start code FSM state */
        int mjpg_eof;//与mjpeg写指针有关，内部处理
        u8 *mjpg_cached_bsbuf;//mjpeg 缓存
        int mjpegScaleDownRatioWidth;//mjpeg缩小比例宽度/* 0,1,2,3 */
        int mjpegScaleDownRatioHeight;//mjpeg缩小比例高度/* 0,1,2,3 */

	struct frame_buf fbpool[MAX_BUF_NUM];
};

struct encode {
	EncHandle handle;		/* Encoder handle */
	PhysicalAddress phy_bsbuf_addr; /* Physical bitstream buffer */
	u32 virt_bsbuf_addr;		/* Virtual bitstream buffer */
	int enc_picwidth;	/* Encoded Picture width */
	int enc_picheight;	/* Encoded Picture height */
	int src_picwidth;        /* Source Picture width */
	int src_picheight;       /* Source Picture height */
	int totalfb;	/* Total number of framebuffers allocated */
	int src_fbid;	/* Index of frame buffer that contains YUV image */
	FrameBuffer *fb; /* frame buffer base given to encoder */
	struct frame_buf **pfbpool; /* allocated fb pointers are stored here */
	ExtBufCfg scratchBuf;
	int mp4_dataPartitionEnable;
        int ringBufferEnable;//循环缓存使能
        int mjpg_fmt;//mjpg框架
        int mvc_paraset_refresh_en;//--内部数据，用户不需要考虑
        int mvc_extension;//--内部数据，用户不需要考虑
        int linear2TiledEnable;//--内部数据，用户不需要考虑
        int minFrameBufferCount;//--内部数据，用户不需要考虑
        int avc_vui_present_flag;//--内部数据，用户不需要考虑

        EncReportInfo mbInfo;//--内部数据，用户不需要考虑
        EncReportInfo mvInfo;//--内部数据，用户不需要考虑
        EncReportInfo sliceInfo;//--内部数据，用户不需要考虑

	struct cmd_line *cmdl; /* command line */
        u8 * huffTable;//--内部数据，用户不需要考虑
        u8 * qMatTable;//--内部数据，用户不需要考虑

        struct frame_buf fbpool[MAX_BUF_NUM];//--内部数据，用户不需要考虑
};

struct input_argument {
        int mode;//工作模式
        pthread_t tid;//线程id
        char line[256];//命令缓存，详见例程
        struct cmd_line cmd;//参加cmd_line
};
struct vpu_dec_handle{
    struct cmd_line *cmdl;//参加cmd_line
    vpu_mem_desc mem_desc;//vpu解码过程中的内存描述 -- 用户不需要理会
    vpu_mem_desc ps_mem_desc;//vpu解码过程中的PS功能内存描述 -- 用户不需要理会
    vpu_mem_desc slice_mem_desc;//vpu解码过程中的片内存描述 -- 用户不需要理会
    vpu_mem_desc vp8_mbparam_mem_desc;//vpu解码过程中的vp8内存描述 -- 用户不需要理会
    struct decode *dec;//解码器信息
};
struct vpu_enc_handle{
    struct cmd_line *cmdl;//参加cmd_line
    vpu_mem_desc mem_desc;//vpu编码过程中的内存描述 -- 用户不需要理会
    vpu_mem_desc scratch_mem_desc;//vpu编码过程中的scratch内存描述 -- 用户不需要理会
    struct encode *enc;//编码器信息
};

/* **************************************************************************************
 * funcation: vpu_open 打开vpu功能
 * input: int versionInfoEn 为是否使能打印vpu的版本信息，为0时打印权限最高
 * 具体参考：dprintf(level, fmt, arg...)   --> level = versionInfoEn;
 * return:返回 ≥ 0 成功 -1 失败
 */
int vpu_open(int versionInfoEn);
/* **************************************************************************************
 * funcation: vpu_close 关闭vpu功能
 * input: 无
 * return:返回 ≥ 0 成功 -1 失败
 */
int vpu_close(void);
/* **************************************************************************************
 * funcation: decoder_init 解码器初始化
 * input: struct vpu_dec_handle *handle 初始化解码器需要的句柄
 * return:返回 ≥ 0 成功 -1 失败
 */
int decoder_init(struct vpu_dec_handle *handle);
/* **************************************************************************************
 * funcation: decoder_uninit 解码器未初始化
 * input: 无
 * return:无
 */
void decoder_uninit(void);
/* **************************************************************************************
 * funcation: decoder_start 解码器解码开始函数，
 * 关于解码过程中的数据输入、输出参加：decoder_complete_1frame_data_outputCallBack和
 * decoder_get_1frame_data_inputCallBack两个函数。
 * input: 无
 * return:返回 ≥ 0 成功 -1 失败
 */
int decoder_start(void);
/* **************************************************************************************
 * funcation: decoder_init 编码器初始化
 * input: struct vpu_enc_handle *handle 初始化编码器需要的句柄
 * return:返回 ≥ 0 成功 -1 失败
 */
int encoder_init(struct vpu_enc_handle *handle);
/* **************************************************************************************
 * funcation: encoder_uninit 编码器未初始化
 * input: 无
 * return:无
 */
void encoder_uninit(void);
/* **************************************************************************************
 * funcation: encoder_start 编码器编码开始
 * 关于编码过程中的输入、输出数据参加：encoder_get_1frame_data_inputCallBack 和
 * encoder_deal_with_1frame_data_outputCallBack
 * input: 无
 * return:返回 ≥ 0 成功 -1 失败
 */
int encoder_start(void);

/* **************************************************************************************
 * funcation: decoder_complete_1frame_data_outputCallBack 解码回调函数，完成了一帧数据的解码，
 * 完成该函数的重写，并在此函数内完成对解码好的帧数据的处理。例如：写入到id为fd的文件中、或经过图形图像
 * 处理后显示到LCD上等。
 * input: int fd,被写入文件的id  [写入文件时用到]
 *        void *vptr 指向帧数据缓存的指针
 *        size_t n 该帧数据的大小
 * return:返回 帧数据的实际长度；≥ 0 成功 -1 失败
 */
extern int decoder_complete_1frame_data_outputCallBack(int fd, void *vptr, size_t n);
/* **************************************************************************************
 * funcation: encoder_deal_with_1frame_data_outputCallBack 解码回调函数，获取一帧需要被解码的数
 * 据传入VPU实现自动解码，完成该函数的重写，并在此函数内完成填充完整的帧数据。例如：从id为fd的文件中获
 * 取一帧数据，或从网络获取等。
 * input: int fd,被读取的文件的id  [从文件读取时用到]
 *        void *vptr 指向帧数据缓存的指针
 *        size_t n 该帧数据的大小
 * return:返回 帧数据的实际长度；≥ 0 成功 -1 失败
 * 注意：如果返回的数据小于帧数据的实际大小，系统将自动识别为解码完成，退出系统！
 */
extern int decoder_get_1frame_data_inputCallBack(int fd, void *vptr, size_t n);
//编码输入读回调函数，从文件获取一帧数据
/* **************************************************************************************
 * funcation: encoder_get_1frame_data_inputCallBack 编码回调函数，获取一帧需要被编码的数据传入
 * VPU实现自动编码，完成该函数的重写，并在此函数内完成填充完整的帧数据。例如：从id为fd的文件中获取一帧
 * 数据，或从摄像头获取等
 * input: int fd,被写入文件的id  [从文件读取用到]
 *        void *vptr 指向帧数据缓存的指针
 *        size_t n 该帧数据的大小
 * return:返回 帧数据的实际长度；≥ 0 成功 -1 失败
 * 注意：如果返回的数据小于帧数据的实际大小，系统将自动识别为解码完成，退出系统！
 */
extern int encoder_get_1frame_data_inputCallBack(int fd, void *vptr, size_t n);
/* **************************************************************************************
 * funcation: encoder_deal_with_1frame_data_outputCallBack 编码回调函数，完成了一帧数据的编码，
 * 完成该函数的重写，并在此函数内完成对编码的数据处理。例如：写入到id为fd的文件中、传送到其它地方。
 * input: int fd,被写入文件的id  [写入文件时用到]
 *        void *vptr 指向帧数据缓存的指针
 *        size_t n 该帧数据的大小
 * return:返回 帧数据的实际长度；≥ 0 成功 -1 失败
 */
extern int encoder_deal_with_1frame_data_outputCallBack(int fd, void *vptr, size_t n);







/*
 *       以下函数等的定义均为库内部调用函数，用户不需要考虑，也请不要做任何修改 **************************************************
 *
 */



int vpu_read(struct cmd_line *cmd, char *buf, int n);
int vpu_write(struct cmd_line *cmd, char *buf, int n);
void get_arg(char *buf, int *argc, char *argv[]);
int open_files(struct cmd_line *cmd);
void close_files(struct cmd_line *cmd);
int check_params(struct cmd_line *cmd, int op);
char*skip_unwanted(char *ptr);
int parse_options(char *buf, struct cmd_line *cmd, int *mode);

struct vpu_display *v4l_display_open(struct decode *dec, int nframes,
                                        struct rot rotation, Rect rotCrop);
int v4l_get_buf(struct decode *dec);
int v4l_put_data(struct decode *dec, int index, int field, int fps);
void v4l_display_close(struct vpu_display *disp);
struct frame_buf *framebuf_alloc(struct frame_buf *fb, int stdMode, int format, int strideY, int height, int mvCol);
int tiled_framebuf_base(FrameBuffer *fb, Uint32 frame_base, int strideY, int height, int mapType);
struct frame_buf *tiled_framebuf_alloc(struct frame_buf *fb, int stdMode, int format, int strideY, int height, int mvCol, int mapType);
void framebuf_free(struct frame_buf *fb);

struct vpu_display *
ipu_display_open(struct decode *dec, int nframes, struct rot rotation, Rect cropRect);
void ipu_display_close(struct vpu_display *disp);
int ipu_put_data(struct vpu_display *disp, int index, int field, int fps);

#ifdef BUILD_FOR_ANDROID
struct vpu_display *
android_display_open(struct decode *dec, int nframes, struct rot rotation, Rect cropRect);
void android_display_close(struct vpu_display *disp);
int android_get_buf(struct decode *dec);
int android_put_data(struct vpu_display *disp, int index, int field, int fps);
#endif

int v4l_start_capturing(void);
void v4l_stop_capturing(void);
int v4l_capture_setup(struct encode *enc, int width, int height, int fps);
int v4l_get_capture_data(struct v4l2_buffer *buf);
void v4l_put_capture_data(struct v4l2_buffer *buf);


int decoder_open(struct decode *dec);
void decoder_close(struct decode *dec);
int decoder_parse(struct decode *dec);
int decoder_allocate_framebuffer(struct decode *dec);
void decoder_free_framebuffer(struct decode *dec);

void SaveQpReport(Uint32 *qpReportAddr, int picWidth, int picHeight,
		  int frameIdx, char *fileName);

static inline int is_mx6x_mjpg(int fmt)
{
        if (cpu_is_mx6x() && (fmt == STD_MJPG))
                return true;
        else
                return false;
}

static __inline int queue_size(struct buf_queue * q)
{
        if (q->tail >= q->head)
                return (q->tail - q->head);
        else
                return ((q->tail + QUEUE_SIZE) - q->head);
}

static __inline int queue_buf(struct buf_queue * q, int idx)
{
        if (((q->tail + 1) % QUEUE_SIZE) == q->head)
                return -1;      /* queue full */
        q->list[q->tail] = idx;
        q->tail = (q->tail + 1) % QUEUE_SIZE;
        return 0;
}

static __inline int dequeue_buf(struct buf_queue * q)
{
        int ret;
        if (q->tail == q->head)
                return -1;      /* queue empty */
        ret = q->list[q->head];
        q->head = (q->head + 1) % QUEUE_SIZE;
        return ret;
}

static __inline int peek_next_buf(struct buf_queue * q)
{
        if (q->tail == q->head)
                return -1;      /* queue empty */
        return q->list[q->head];
}

#endif
