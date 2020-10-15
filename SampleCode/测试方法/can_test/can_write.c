#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ioctl.h>
#include <getopt.h>
#include <sys/mman.h>
#include "ioctl.h"
#include "can.h"


int signalquit;

void sighandler(int sig)
{
	switch(sig){
		case SIGINT:
			signalquit =1;
			break;
	}
}


static void print_usage(char *prg)
{
    fprintf(stderr,
	    "Usage: %s <can-interface> [Options] \n"
	    "Options:\n"
	    " -h, --help               this help\n"
	    " -b, --baudrate=BPS       baudrate in bits/sec\n"
	    " -B, --bittime=BRP:PROP_SEG:PHASE_SEG1:PHASE_SEG2:SJW:SAM\n",
	    prg);
}

can_baudrate_t string_to_baudrate(char *str)
{
    can_baudrate_t baudrate;
    if (sscanf(str, "%i", &baudrate) != 1)
	return -1;
    return baudrate;
}

int main(int argc, char *argv[])
{
    	char    ifname[16];
    	int     can_fd = -1;
    	int     new_baudrate = -1;
    	int     bittime_count = 0, bittime_data[6];
    	struct  ifreq ifr;
	struct sockaddr_can addr;
	struct can_frame frame;
    	can_baudrate_t *baudrate;
    	struct can_bittime *bittime;
    	int opt, ret;
    	char* ptr;
	int nbytes,i;
        char p_buf[100]={0};
        char p_tmp[20]="100000";
    	
	struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "baudrate", required_argument, 0, 'b'},
		{ "bittime", required_argument, 0, 'B'},
		{ 0, 0, 0, 0},
    	};

    	while ((opt = getopt_long(argc, argv, "hb:B:",long_options, NULL)) != -1) {
                switch (opt) {
		case 'h':
	    		print_usage(argv[0]);
	    		exit(0);

		case 'b':
                        printf("--  b = %s  \r\n\r\n",optarg);
	    		new_baudrate = string_to_baudrate(optarg);
	    		if (new_baudrate == -1) {
				print_usage(argv[0]);
				exit(0);
	    		}
	    		break;

		case 'B':
	    		ptr = optarg;
	    		while (1) {
				bittime_data[bittime_count++] = strtoul(ptr, NULL, 0);
	        		if (!(ptr = strchr(ptr, ':')))
		    		break;
				ptr++;
	    		}
	    		if (bittime_count != 6) {
				print_usage(argv[0]);
				exit(0);
	    		}
	    		break;

		default:
	    		fprintf(stderr, "Unknown option %c\n", opt);
	    		break;
		}
    	}

    /* Get CAN interface name */
	if (optind != argc - 1 && optind != argc - 2) {
		print_usage(argv[0]);
		return 0;
    	}
        sprintf(p_buf,"ifconfig %s down",argv[optind]);
        printf("sys-cmd 1 %s \r\n",p_buf);

    	strncpy(ifname, argv[optind], IFNAMSIZ);
    	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    	can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    	if (can_fd < 0) {
		perror("socket");
		return can_fd;
    	}

    	ret = ioctl(can_fd, SIOCGIFINDEX, &ifr);
    	if (ret) {
		perror("ioctl:SIOCGIFINDEX");
		return ret;
    	}


	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

    	if (new_baudrate != -1) {
		printf("baudrate: %d\n", new_baudrate);
		baudrate = (can_baudrate_t *)&ifr.ifr_ifru;
		*baudrate = new_baudrate;
                printf("-- b is %d \r\n",ifr.ifr_ifru);
		ret = ioctl(can_fd, SIOCSCANBAUDRATE, &ifr);
		if (ret) {
			perror("ioctl:SIOCSCANBAUDRATE");
		    	goto abort;
		}
	    }

    	if (bittime_count) {
		bittime = (struct can_bittime *)&ifr.ifr_ifru;
	    	bittime->type = CAN_BITTIME_STD;
	    	bittime->std.brp = bittime_data[0];
	    	bittime->std.prop_seg = bittime_data[1];
	    	bittime->std.phase_seg1 = bittime_data[2];
	    	bittime->std.phase_seg2 = bittime_data[3];
	    	bittime->std.sjw = bittime_data[4];
	    	bittime->std.sam = bittime_data[5];
		printf("bit-time: brp=%d prop_seg=%d phase_seg1=%d "
		       "phase_seg2=%d sjw=%d sam=%d\n",
		       bittime->std.brp,
		       bittime->std.prop_seg,
		       bittime->std.phase_seg1,
		       bittime->std.phase_seg2,
		       bittime->std.sjw,
		       bittime->std.sam);

		ret = ioctl(can_fd, SIOCSCANCUSTOMBITTIME, &ifr);
		if (ret) {
			perror("ioctl:SIOCSCANCUSTOMBITTIME\n");
	    		goto abort;
		}

    	}
        sprintf(p_buf,"ip link set %s type can bitrate %s triple-sampling on",argv[optind],p_tmp);
        printf("sys-cmd 2 %s \r\n",p_buf);
        system(p_buf);
        sprintf(p_buf,"ifconfig %s up",argv[optind]);
        printf("sys-cmd 3 %s \r\n",p_buf);
        system(p_buf);

	if(bind(can_fd,(struct sockaddr *)&addr,sizeof(addr))){
		perror("bind\n");
		goto abort;
	}


	memset(&frame,0x0,sizeof(frame));

	frame.can_id = 0x123;
	frame.can_dlc =8;
	strncpy(frame.data,"12345678",8);
	while(!signalquit){
                nbytes = write(can_fd,&frame,sizeof(frame));
		if(nbytes < 0){
			perror("write");
			goto abort;
		}
		if(nbytes < sizeof(struct can_frame)){
			printf("write:incomplete can frame\n");
			goto abort;
		}
		printf("write a can frame:can_id=0x%x,can_dlc=%d,data=%s\n",frame.can_id,frame.can_dlc,frame.data);                
		for(i=0;i<10000;i++);
	}     
    	close(can_fd);
    	return 0;

 abort:
    	close(can_fd);
    	return ret;
}
