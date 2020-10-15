#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }

switch( nSpeed )
    {
    case 300:
        cfsetispeed(&newtio, B300);
        cfsetospeed(&newtio, B300);
        break;	
    case 600:
        cfsetispeed(&newtio, B600);
        cfsetospeed(&newtio, B600);
        break;		
    case 1200:
        cfsetispeed(&newtio, B1200);
        cfsetospeed(&newtio, B1200);
        break;			
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
     case 19200:
        cfsetispeed(&newtio, B19200);
        cfsetospeed(&newtio, B19200);
        break;       
     case 38400:
        cfsetispeed(&newtio, B38400);
        cfsetospeed(&newtio, B38400);
        break;  
     case 57600:
        cfsetispeed(&newtio, B57600);
        cfsetospeed(&newtio, B57600);
        break;      
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        printf("sorry! [%d bps] is not support ,Set default 9600bps now!\n",nSpeed);
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}

int open_port(int fd,char *dev)
{
    //char *dev[]={"/dev/ttyO1","/dev/ttyO2","/dev/ttyO3"};
    long  vdisable;
   fd = open( dev, O_RDWR|O_NOCTTY); //open("/dev/tttyS0",O_RDWR | O_NOCTTY);  //O_NDELAY
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open %s ok\n",dev);
        }
	
    if(fcntl(fd, F_SETFL, 0)<0)
    {
        printf("fcntl failed!\n");
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    }
    if(isatty(STDIN_FILENO)==0)
    {
        printf("standard input is not a terminal device\n");
    }
    else
    {
        printf("isatty success!\n");
    }
    printf("fd-open=%d\n",fd);
    return fd;
}

void msdelay(int val){
	while(val--)usleep(1000);
}

//#define DEBUG_STR  "ttySC1 554433"
#define Mode_W    1
#define Mode_R    2
#define Mode_RW   3
#define Version   "V1.1"
//const char *default_str = "hello serial test";
int main(int argc ,char **argv)
{
    int fd;
    int nread,i;
    char buff[1024];
	int nwrite;
	int setmode;
	char *wchr;
	int format = 0;
	int boudrate = 9600;
	int enter = 0;
	int loop = 0;
	int intervalMS = 1000;
	printf("\n==================================================\n");
	//printf("argc = %d\n",argc);
	if((argc < 2) || (!strcmp(argv[1],"-h"))||(!strcmp(argv[1],"help"))){
		 printf("Version %s\n",Version);
		 printf("[device] [option1] [option2] ....[optionN]\n");
		 printf("option:\n");
		 printf("       -asc: read to ascii\n");
		 printf("       -b: set buadrate  + [bps]\n");
		 printf("       [bps] 300 600 1200 2400 4800 9600 19200 38400 57600 115200\n");
		 printf("       -n: write end add enter\n");
		 printf("       -l: loop\n");
		 printf("       -ms: write xx ms interval\n");
		 printf("       -r: read only\n");
		 printf("       -w: write only  + [context]\n");
		 printf("       -rw: read write + [context]\n");
		 printf("       -h: for help\n");
		 printf("==================================================\n");
		 return 1;
	}
	printf("Device [%s] \n",argv[1]);
	//------------------------------------------------------------
	for(i = 1; i < argc; i ++){
		if(!strcmp(argv[i],"-asc")){
			format = 1;
			printf("format ascii\n");
		}else if(!strcmp(argv[i],"-hex")){
			format = 0;
			printf("format hex\n");
		}	
		else if(!strcmp(argv[i],"-b")){
			i++;
			boudrate = atoi(argv[i]);
		}
		else if(!strcmp(argv[i],"-n")){
			enter = 1;
			printf("enter true\n");
		}
		else if(!strcmp(argv[i],"-l")){
			loop = 1;
			printf("loop true\n");
		}
		else if(!strcmp(argv[i],"-ms")){
			i++;
			intervalMS = atoi(argv[i]);
		}
		else if(!strcmp(argv[i],"-w")){	
			i++;
			setmode = Mode_W;
			wchr = (char*)argv[i];
			printf("write only  w:%s\n",wchr);
		}	
		else if(!strcmp(argv[i],"-r")){
			printf("read only\n");
			setmode = Mode_R;
		}	
		else if(!strcmp(argv[i],"-rw")){
			i++;
			setmode = Mode_RW;
			wchr = (char*)argv[i];
			printf("read write w:%s\n",wchr);
		}		
	}
	printf("boudrate = %d\n",boudrate);
	printf("interval : %d ms\n",intervalMS);
	printf("==================================================\n");
    if((fd=open_port(fd,argv[1]))<0){
        perror("open_port error");
        return;
    }
    if((i=set_opt(fd,boudrate,8,'N',1))<0){
        perror("set_opt error");
        return;
    }
    while(loop){
		switch(setmode){
			case Mode_W:
				nwrite = sprintf(buff,"%s",wchr);
				if(enter){
					buff[nwrite++] = 0x0d;
					buff[nwrite++] = 0x0a;
				}
				write(fd,buff,nwrite);
			    printf("%s write :%s\n",argv[1],wchr);
				break;
			case Mode_R:
				nread=read(fd,buff,sizeof(buff));
				if(nread>0){
					printf("\n%s read=%d : ",argv[1],nread);
					if(format==0){
						for(i = 0; i < nread;i++){
							printf("%02x ",buff[i]);
						}
						printf("\n");
					}else{
						buff[nread] = '\0';
						printf("%s\n",buff);
					}
				}
				break;
			case Mode_RW:
				nwrite = sprintf(buff,"%s",wchr);
				if(enter){
					buff[nwrite++] = 0x0d;
					buff[nwrite++] = 0x0a;
				}
				write(fd,buff,nwrite);
			    printf("%s write :%s\n",argv[1],wchr);
				//sleep(1);
				nread=read(fd,buff,sizeof(buff));
				if(nread>0){
					printf("\n%s read=%d : ",argv[1],nread);
					if(format==0){
						for(i = 0; i < nread;i++){
							printf("%02x ",buff[i]);
						}
						printf("\n");
					}else{
						buff[nread] = '\0';
						printf("%s\n",buff);
					}
				}		
				break;
			
		}
		//sleep(1);
		msdelay(intervalMS);
	}
    close(fd);
    return;
}
