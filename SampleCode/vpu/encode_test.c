/*
 * Copyright 2004-2013, 2016 Freescale Semiconductor, Inc.
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

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include "include/tqvpu_lib.h"

#include "include/soc_check.h"

#define ONE_FRAME_INTERV 100000 // 100 ms

char *usage = "Usage: ./mxc_vpu_test.out -D \"<decode options>\" "\
	       "-E \"<encode options>\" "\
	       "-L \"<loopback options>\" -C <config file> "\
	       "-T \"<transcode options>\" "\
	       "-H display this help \n "
	       "\n"\
	       "decode options \n "\
	       "  -i <input file> Read input from file \n "\
	       "	If no input file is specified, default is network \n "\
	       "  -o <output file> Write output to file \n "\
	       "	If no output is specified, default is LCD \n "\
	       "  -x <output method> output mode V4l2(0) or IPU lib(1) \n "\
	       "        0 - V4L2 of FG device, 1 - IPU lib path \n "\
	       "        2 - G2D (available for Android only) \n "\
	       "        Other value means V4L2 with other video node\n "\
	       "        16 - /dev/video16, 17 - /dev/video17, and so on \n "\
	       "  -f <format> 0 - MPEG4, 1 - H.263, 2 - H.264, 3 - VC1, \n "\
	       "	4 - MPEG2, 5 - DIV3, 6 - RV, 7 - MJPG, \n "\
	       "        8 - AVS, 9 - VP8\n "\
	       "	If no format specified, default is 0 (MPEG4) \n "\
	       "  -l <mp4Class / h264 type> \n "\
	       "        When 'f' flag is 0 (MPEG4), it is mp4 class type. \n "\
	       "        0 - MPEG4, 1 - DIVX 5.0 or higher, 2 - XVID, 5 - DIVX4.0 \n "\
	       "        When 'f' flag is 2 (H.264), it is h264 type. \n "\
	       "        0 - normal H.264(AVC), 1 - MVC \n "\
	       "  -p <port number> UDP port number to bind \n "\
	       "	If no port number is secified, 5555 is used \n "\
	       "  -c <count> Number of frames to decode \n "\
	       "  -d <deblocking> Enable deblock - 1. enabled \n "\
	       "	default deblock is disabled (0). \n "\
	       "  -e <dering> Enable dering - 1. enabled \n "\
	       "	default dering is disabled (0). \n "\
	       "  -r <rotation angle> 0, 90, 180, 270 \n "\
	       "	default rotation is disabled (0) \n "\
	       "  -m <mirror direction> 0, 1, 2, 3 \n "\
	       "	default no mirroring (0) \n "\
	       "  -u <ipu/gpu rotation> Using IPU/GPU rotation for display - 1. IPU/GPU rotation \n "\
	       "        default is VPU rotation(0).\n "\
	       "        This flag is effective when 'r' flag is specified.\n "\
	       "  -v <vdi motion> set IPU VDI motion algorithm l, m, h.\n "\
	       "	default is m-medium. \n "\
	       "  -w <width> display picture width \n "\
	       "	default is source picture width. \n "\
	       "  -h <height> display picture height \n "\
	       "	default is source picture height \n "\
	       "  -j <left offset> display picture left offset \n "\
	       "	default is 0. \n "\
	       "  -k <top offset> display picture top offset \n "\
	       "	default is 0 \n "\
	       "  -a <frame rate> display framerate \n "\
	       "	default is 30 \n "\
	       "  -t <chromaInterleave> CbCr interleaved \n "\
	       "        default is interleave(1). \n "\
	       "  -s <prescan/bs_mode> Enable prescan in decoding on i.mx5x - 1. enabled \n "\
	       "        default is disabled. Bitstream mode in decoding on i.mx6  \n "\
	       "        0. Normal mode, 1. Rollback mode \n "\
	       "        default is enabled. \n "\
	       "  -y <maptype> Map type for GDI interface \n "\
	       "        0 - Linear frame map, 1 - frame MB map, 2 - field MB map \n "\
	       "        default is 0. \n "\
	       "\n"\
	       "encode options \n "\
	       "  -i <input file> Read input from file (yuv) \n "\
	       "	If no input file specified, default is camera \n "\
	       "  -x <input method> input mode V4L2 with video node \n "\
	       "        0 - /dev/video0, 1 - /dev/video1, and so on \n "\
	       "  -o <output file> Write output to file \n "\
	       "	This option will be ignored if 'n' is specified \n "\
	       "	If no output is specified, def files are created \n "\
	       "  -n <ip address> Send output to this IP address \n "\
	       "  -p <port number> UDP port number at server \n "\
	       "	If no port number is secified, 5555 is used \n "\
	       "  -f <format> 0 - MPEG4, 1 - H.263, 2 - H.264, 7 - MJPG \n "\
	       "	If no format specified, default is 0 (MPEG4) \n "\
	       "  -l <h264 type> 0 - normal H.264(AVC), 1 - MVC\n "\
	       "  -c <count> Number of frames to encode \n "\
	       "  -r <rotation angle> 0, 90, 180, 270 \n "\
	       "	default rotation is disabled (0) \n "\
	       "  -m <mirror direction> 0, 1, 2, 3 \n "\
	       "	default no mirroring (0) \n "\
	       "  -w <width> capture image width \n "\
	       "	default is 176. \n "\
	       "  -h <height>capture image height \n "\
	       "	default is 144 \n "\
	       "  -b <bitrate in kbps> \n "\
	       "	default is auto (0) \n "\
	       "  -g <gop size> \n "\
	       "	default is 0 \n "\
	       "  -t <chromaInterleave> CbCr interleaved \n "\
	       "        default is interleave(1). \n "\
	       "  -q <quantization parameter> \n "\
	       "	default is 20 \n "\
	       "  -a <frame rate> capture/encode framerate \n "\
	       "	default is 30 \n "\
	       "\n"\
	       "loopback options \n "\
	       "  -x <input method> input mode V4L2 with video node \n "\
	       "        0 - /dev/video0, 1 - /dev/video1, and so on \n "\
	       "  -f <format> 0 - MPEG4, 1 - H.263, 2 - H.264, 7 - MJPG \n "\
	       "	If no format specified, default is 0 (MPEG4) \n "\
	       "  -w <width> capture image width \n "\
	       "	default is 176. \n "\
	       "  -h <height>capture image height \n "\
	       "	default is 144 \n "\
	       "  -t <chromaInterleave> CbCr interleaved \n "\
               "        default is interleave(1). \n "\
	       "  -a <frame rate> capture/encode/display framerate \n "\
	       "	default is 30 \n "\
	       "\n"\
	       "transcode options, encoder set to h264 720p now \n "\
	       "  -i <input file> Read input from file \n "\
	       "        If no input file is specified, default is network \n "\
	       "  -o <output file> Write output to file \n "\
	       "        If no output is specified, default is LCD \n "\
	       "  -x <output method> V4l2(0) or IPU lib(1) \n "\
	       "  -f <format> 0 - MPEG4, 1 - H.263, 2 - H.264, 3 - VC1, \n "\
	       "        4 - MPEG2, 5 - DIV3, 6 - RV, 7 - MJPG, \n "\
	       "        8 - AVS, 9 - VP8\n "\
	       "        If no format specified, default is 0 (MPEG4) \n "\
	       "  -l <mp4Class / h264 type> \n "\
	       "        When 'f' flag is 0 (MPEG4), it is mp4 class type. \n "\
	       "        0 - MPEG4, 1 - DIVX 5.0 or higher, 2 - XVID, 5 - DIVX4.0 \n "\
	       "        When 'f' flag is 2 (H.264), it is h264 type. \n "\
	       "        0 - normal H.264(AVC), 1 - MVC \n "\
	       "  -p <port number> UDP port number to bind \n "\
	       "        If no port number is secified, 5555 is used \n "\
	       "  -c <count> Number of frames to decode \n "\
	       "  -d <deblocking> Enable deblock - 1. enabled \n "\
	       "        default deblock is disabled (0). \n "\
	       "  -e <dering> Enable dering - 1. enabled \n "\
	       "        default dering is disabled (0). \n "\
	       "  -r <rotation angle> 0, 90, 180, 270 \n "\
	       "        default rotation is disabled (0) \n "\
	       "  -m <mirror direction> 0, 1, 2, 3 \n "\
	       "        default no mirroring (0) \n "\
	       "  -u <ipu rotation> Using IPU rotation for display - 1. IPU rotation \n "\
	       "        default is VPU rotation(0).\n "\
	       "        This flag is effective when 'r' flag is specified.\n "\
	       "  -v <vdi motion> set IPU VDI motion algorithm l, m, h.\n "\
	       "        default is m-medium. \n "\
	       "  -w <width> display picture width \n "\
	       "        default is source picture width. \n "\
	       "  -h <height> display picture height \n "\
	       "        default is source picture height \n "\
	       "  -j <left offset> display picture left offset \n "\
	       "        default is 0. \n "\
	       "  -k <top offset> display picture top offset \n "\
	       "        default is 0 \n "\
	       "  -a <frame rate> display framerate \n "\
	       "        default is 30 \n "\
	       "  -t <chromaInterleave> CbCr interleaved \n "\
	       "        default is interleave(1). \n "\
	       "  -s <prescan/bs_mode> Enable prescan in decoding on i.mx5x - 1. enabled \n "\
	       "        default is disabled. Bitstream mode in decoding on i.mx6  \n "\
	       "        0. Normal mode, 1. Rollback mode \n "\
	       "        default is enabled. \n "\
	       "  -y <maptype> Map type for GDI interface \n "\
	       "        0 - Linear frame map, 1 - frame MB map, 2 - field MB map \n "\
	       "  -q <quantization parameter> \n "\
	       "	default is 20 \n "\
	       "\n"\
	       "config file - Use config file for specifying options \n";

sigset_t sigset;
int quitflag;

static struct input_argument input_arg[MAX_NUM_INSTANCE];
static int instance;
static int using_config_file;

int vpu_test_dbg_level;

int decode_test(void *arg);

int encdec_test(void *arg);
int transcode_test(void *arg);

/* Encode or Decode or Loopback */
static char *mainopts = "HE:D:";

/* Options for encode and decode */
static char *options = "i:o:x:n:p:r:f:c:w:h:g:b:d:e:m:u:t:s:l:j:k:a:v:y:q:";

int
parse_config_file(char *file_name)
{
	FILE *fp;
	char line[MAX_PATH];
	char *ptr;
	int end;

	fp = fopen(file_name, "r");
	if (fp == NULL) {
		err_msg("Failed to open config file\n");
		return -1;
	}

	while (fgets(line, MAX_PATH, fp) != NULL) {
		if (instance > MAX_NUM_INSTANCE) {
			err_msg("No more instances!!\n");
			break;
		}

		ptr = skip_unwanted(line);
		end = parse_options(ptr, &input_arg[instance].cmd,
					&input_arg[instance].mode);
		if (end == 100) {
			instance++;
		}
	}

	fclose(fp);
	return 0;
}

int
parse_main_args(int argc, char *argv[])
{
	int status = 0, opt;

	do {
		opt = getopt(argc, argv, mainopts);
		switch (opt)
		{
		case 'E':
			input_arg[instance].mode = ENCODE;
			strncpy(input_arg[instance].line, argv[0], 26);
                        printf("*****\t input_arg[instance].line 1:%s \r\n",input_arg[instance].line);
			strncat(input_arg[instance].line, " ", 2);
                        printf("*****\t input_arg[instance].line 2:%s \r\n",input_arg[instance].line);
			strncat(input_arg[instance].line, optarg, 200);
                        printf("*****\t input_arg[instance].line 3:%s \r\n",input_arg[instance].line);
			instance++;
			break;
		case -1:
			break;
		case 'H':
                    info_msg("\n%s", usage);
                    break;
		default:
			status = -1;
			break;
		}
	} while ((opt != -1) && (status == 0) && (instance < MAX_NUM_INSTANCE));

	optind = 1;
	return status;
}

int
parse_args(int argc, char *argv[], int i)
{
	int status = 0, opt, val;

	input_arg[i].cmd.chromaInterleave = 1;
	if (cpu_is_mx6x())
		input_arg[i].cmd.bs_mode = 1;

	do {
		opt = getopt(argc, argv, options);
		switch (opt)
		{
		case 'i':
			strncpy(input_arg[i].cmd.input, optarg, MAX_PATH);
			input_arg[i].cmd.src_scheme = PATH_FILE;
			break;
		case 'o':
			if (input_arg[i].cmd.dst_scheme == PATH_NET) {
				warn_msg("-o ignored because of -n\n");
				break;
			}
			strncpy(input_arg[i].cmd.output, optarg, MAX_PATH);
			input_arg[i].cmd.dst_scheme = PATH_FILE;
			break;
		case 'x':
			val = atoi(optarg);
			if ((input_arg[i].mode == ENCODE) || (input_arg[i].mode == LOOPBACK))
				input_arg[i].cmd.video_node_capture = val;
			else {
				if (val == 1) {
					input_arg[i].cmd.dst_scheme = PATH_IPU;
					info_msg("Display through IPU LIB\n");
					if (cpu_is_mx6x())
						warn_msg("IPU lib is OBSOLETE, please try other renderer\n");
#ifdef BUILD_FOR_ANDROID
				} else if (val == 2) {
					input_arg[i].cmd.dst_scheme = PATH_G2D;
					info_msg("Display through G2D\n");
#endif
				} else {
					input_arg[i].cmd.dst_scheme = PATH_V4L2;
					info_msg("Display through V4L2\n");
					input_arg[i].cmd.video_node = val;
				}
				if (cpu_is_mx27() &&
					(input_arg[i].cmd.dst_scheme == PATH_IPU)) {
					input_arg[i].cmd.dst_scheme = PATH_V4L2;
					warn_msg("ipu lib disp only support in ipuv3\n");
				}
			}
			break;
		case 'n':
			if (input_arg[i].mode == ENCODE) {
				/* contains the ip address */
				strncpy(input_arg[i].cmd.output, optarg, 64);
				input_arg[i].cmd.dst_scheme = PATH_NET;
			} else {
				warn_msg("-n option used only for encode\n");
			}
			break;
		case 'p':
			input_arg[i].cmd.port = atoi(optarg);
			break;
		case 'r':
			input_arg[i].cmd.rot_angle = atoi(optarg);
			if (input_arg[i].cmd.rot_angle)
				input_arg[i].cmd.rot_en = 1;
			break;
		case 'u':
			input_arg[i].cmd.ext_rot_en = atoi(optarg);
			/* ipu/gpu rotation will override vpu rotation */
			if (input_arg[i].cmd.ext_rot_en)
				input_arg[i].cmd.rot_en = 0;
			break;
		case 'f':
			input_arg[i].cmd.format = atoi(optarg);
			break;
		case 'c':
			input_arg[i].cmd.count = atoi(optarg);
			break;
		case 'v':
			input_arg[i].cmd.vdi_motion = optarg[0];
			break;
		case 'w':
			input_arg[i].cmd.width = atoi(optarg);
			break;
		case 'h':
			input_arg[i].cmd.height = atoi(optarg);
			break;
		case 'j':
			input_arg[i].cmd.loff = atoi(optarg);
			break;
		case 'k':
			input_arg[i].cmd.toff = atoi(optarg);
			break;
		case 'g':
			input_arg[i].cmd.gop = atoi(optarg);
			break;
		case 's':
			if (cpu_is_mx6x())
				input_arg[i].cmd.bs_mode = atoi(optarg);
			else
				input_arg[i].cmd.prescan = atoi(optarg);
			break;
		case 'b':
			input_arg[i].cmd.bitrate = atoi(optarg);
			break;
		case 'd':
			input_arg[i].cmd.deblock_en = atoi(optarg);
			break;
		case 'e':
			input_arg[i].cmd.dering_en = atoi(optarg);
			break;
		case 'm':
			input_arg[i].cmd.mirror = atoi(optarg);
			if (input_arg[i].cmd.mirror)
				input_arg[i].cmd.rot_en = 1;
			break;
		case 't':
			input_arg[i].cmd.chromaInterleave = atoi(optarg);
			break;
		case 'l':
			input_arg[i].cmd.mp4_h264Class = atoi(optarg);
			break;
		case 'a':
			input_arg[i].cmd.fps = atoi(optarg);
			break;
		case 'y':
			input_arg[i].cmd.mapType = atoi(optarg);
			break;
		case 'q':
			input_arg[i].cmd.quantParam = atoi(optarg);
			break;
		case -1:
			break;
		default:
			status = -1;
			break;
		}
	} while ((opt != -1) && (status == 0));

	optind = 1;
	return status;
}
int
main(int argc, char *argv[])
{
        int err, nargc, ret;
	char *pargv[32] = {0}, *dbg_env;
        struct vpu_enc_handle handle;

	char *soc_list[] = {"i.MX6Q", "i.MX6QP", "i.MX6DL", " "};

	ret = soc_version_check(soc_list);
	if (ret == 0) {
		printf("mxc_vpu_test.out not supported on current soc\n");
		return 0;
	}

	ret = 0;

	dbg_env=getenv("VPU_TEST_DBG");
	if (dbg_env)
		vpu_test_dbg_level = atoi(dbg_env);
	else
                vpu_test_dbg_level = 1;
	err = parse_main_args(argc, argv);

	info_msg("VPU test program built on %s %s\n", __DATE__, __TIME__);

        vpu_open(1);

        if (instance == 0)  {
            input_arg[instance].mode = ENCODE;
            strncat(input_arg[instance].line, " ", 2);
            strncat(input_arg[instance].line, "-o enc.264 -w 1280 -h 720 -f 2", 200);
        }
        if (using_config_file == 0) {
            get_arg(input_arg[0].line, &nargc, pargv);
            err = parse_args(nargc, pargv, 0);
            if (err) {
                vpu_close();
                goto usage;
            }
        }
        if (check_params(&input_arg[0].cmd, input_arg[0].mode) == 0) {
            if (open_files(&input_arg[0].cmd) == 0) {
                if (input_arg[0].mode == ENCODE) {
                    handle.cmdl = &input_arg[0].cmd;
                    ret = encoder_init(&handle);
                    if(encoder_start()<0)
                    {
                        printf("The code failed \n");
                    }
                }
                encoder_uninit();
                close_files(&input_arg[0].cmd);
            } else {
                ret = -1;
            }
        } else {
            ret = -1;
        }

        vpu_close();

	return ret;

usage:
	info_msg("\n%s", usage);
	return -1;
}

