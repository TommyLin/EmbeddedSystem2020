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
static int gpio_read(struct mesg* mesg, char* value);
static int gpio_direction(struct mesg* mesg);
static int gpio_export(struct mesg* mesg);

void umsg(void){
	printf("usage: gpio_in num1~num2 value \n\t example: gpio_in 2~8 1\n");
	exit(1);
}
int main(int argc, char *argv[]){
	int num1,num2,cnt;
	int value;
	//判断输入命令
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
			perror("mallo");
			return -1;
		}
			
	if((num1>=PINMIN)&&(num2<=PINMAX)){
		cnt = 0;
		do{
			Amesg[cnt] = pmesg++;
			dbg_info("num1=%d cnt=%d\n",num1,cnt);
			Amesg[cnt]->pin = num1;
			num1++;
			Amesg[cnt]->flags |= IN;
			gpio_export(Amesg[cnt]);//生成gpio节点
			gpio_direction(Amesg[cnt]);//设置方向
			gpio_read(Amesg[cnt], argv[2]);//读入value，
			cnt++;
		}while(cnt < num);
	}
	gpio_result(Amesg, num);//读取结果打印，判断是否正确
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
    dbg_info("name = gpio%s\n",name);
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

static int gpio_read(struct mesg* mesg, char* value){
    char path[64];
    char value_str[4];
    int fd, value1, value2;
  
    sprintf(path, "/sys/class/gpio/gpio%d/value", mesg->pin);  
    dbg_info("value path: %s\n", path);
    fd = open(path, O_RDONLY);
		if (fd < 0) {  
			perror("open value");  
			return -1;  
		}
    if (read(fd, value_str, 3) < 0) {  
        perror("read value");
        return -1;
    }
    sscanf(value_str,"%d",&mesg->value);
    sscanf(value,"%d",&value2);
    dbg_info("value_read = %d\n",mesg->value);
    dbg_info("value_cmd = %d\n",value2);
	if(mesg->value != value2)
		mesg->flags |= ERROR;
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
		printf("gpio%d, value = %d; ",Amesg[cnt]->pin,Amesg[cnt]->value);
		if(Amesg[cnt]->flags & ERROR)
			printf("ERROR: pin%d",Amesg[cnt]->pin);
		printf("\n");
		gpio_unexport(Amesg[cnt]);
		cnt++;
	}while(cnt < num);
	return 0;
}
