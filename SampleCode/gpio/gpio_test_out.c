#include <stdlib.h>  
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define ERROR 0x01
#define IN	  0x80
#define OUT	  0x40
#define PINMAX	223
#define PINMIN	0
 
#define	DEV_INFO	"[GPIO_TEST]"
//#define	GPIO_DEBUG
#ifdef	GPIO_DEBUG
#define	dbg_info(format, args...)	printf("%d:"format, __LINE__, ##args)
#else
#define	dbg_info(format, args...)
#endif
#define	dbg_err(format, args...)	printf("%d:"format, __LINE__, ##args)

struct mesg{
	int pin;
	int value;
	unsigned char flags;
};
static int gpio_result(struct mesg* mesg[], int num);
static int gpio_unexport(struct mesg* mesg);
static int gpio_direction(struct mesg* mesg);
static int gpio_export(struct mesg* mesg);
static int gpio_write(struct mesg* mesg, char* value);

void umsg(void){
	printf("usage: gpio_in num1~num2 value \n\t example: gpio_out 7~14 1\n");
	exit(1);
}
int main(int argc, char *argv[]){
	int num1,num2,cnt;
	int value;
	if (argc != 3)
		umsg();
	sscanf(argv[1], "%d~%d", &num1, &num2);
	sscanf(argv[2], "%d", &value);
	dbg_info("pin_min = %d, pin_max = %d\n, value = %s\n",num1,num2,argv[2]);
	int num = num2-num1+1;
	struct mesg* Amesg[num2-num1+1];
	struct mesg* p_mesg = (struct mesg*)malloc(sizeof(struct mesg)*(num2-num1+1));
	struct mesg* pmesg = p_mesg;
		if(p_mesg == NULL){
			perror("malloc");
			return -1;
		}
			
	if((num1>=PINMIN)&&(num2<=PINMAX)){
		cnt = 0;
		do{
			Amesg[cnt] = pmesg++;
			dbg_info("num1=%d,cnt=%d\n",num1,cnt);
			Amesg[cnt]->pin = num1;
			Amesg[cnt]->flags |= OUT;
			Amesg[cnt]->value = value;
			gpio_export(Amesg[cnt]);
			gpio_direction(Amesg[cnt]);
			gpio_write(Amesg[cnt], argv[2]);
			cnt++;num1++;
		}while(cnt < num);
	}
	gpio_result(Amesg, num);
	free(p_mesg);
	return 0;
}

static int gpio_export(struct mesg* mesg){  
    char name[4];  
    int fd,len;  

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        perror("open export");
        return(-1);
    }
    len = sprintf(name, "%d", mesg->pin);  
    dbg_info("gpiopin = gpio%s\n",name);
    if (write(fd, name, sizeof(name)) < 0) {  
        perror("write export");
        return -1;  
    }  
    close(fd);  
    return 0;  
}  
static int gpio_direction(struct mesg* mesg){  
    char path[64];  
    int fd,res;
    char *dir;
    if(mesg->flags & IN)
		dir = "in";
	if(mesg->flags & OUT)
		dir = "out";
    sprintf(path,"/sys/class/gpio/gpio%d/direction", mesg->pin);
	
	dbg_info("direction path: %s,direction = %s\n", path, dir);  
    
    fd = open(path, O_WRONLY);
    if (fd < 0) {
		perror("open direction");
        return -1;  
    }  
	res = write(fd, dir, sizeof(dir));
    if (res <= 0) {
        perror("write direction");
        return -1;
    }
    close(fd);
    return 0;
}

static int gpio_write(struct mesg* mesg, char* value){  
    char path[64];  
    int fd;  

    sprintf(path, "/sys/class/gpio/gpio%d/value", mesg->pin);
    dbg_info("value path: %s, value = %s\n", path,value);
    
    fd = open(path, O_RDWR);
		if (fd < 0) {  
			perror("open value");  
			return -1;  
		}
	
 	int res = write(fd, value, sizeof(value));
    if (res <= 0) {
        perror("write value");
        return -1;
    }
    close(fd);  
    return 0;  
}
static int gpio_unexport(struct mesg* mesg)  
{  
    char name[4];  
    int len;  
    int fd;  
  
    fd = open("/sys/class/gpio/unexport", O_WRONLY);  
    if (fd < 0) {  
        perror("open unexport");
        return -1;  
    }  
    len = sprintf(name, "%d", mesg->pin);
    dbg_info("name = gpio%s\n",name);
    if (write(fd, name, len) < 0) {  
        perror("write unexport");
        return -1;  
    }  
    close(fd);  
    return 0;  
}
static int gpio_result(struct mesg* Amesg[], int num){
	int cnt=0;
	dbg_info("num=%d\n",num);
	do{
		dbg_info("pin = %d, value = %d, flags = %#x\n",Amesg[cnt]->pin, Amesg[cnt]->value, Amesg[cnt]->flags);
		printf("gpio%d, output value = %d\n", Amesg[cnt]->pin, Amesg[cnt]->value);
		gpio_unexport(Amesg[cnt]);
		cnt++;
	}while(cnt < num);
	return 0;
}







