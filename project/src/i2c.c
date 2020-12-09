#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>


#define DEVICE                          "/dev/i2c-0"
// TODO Change I2C address to DHT11. It might be 0x44(python) or 0x38(DHT.h)
#define DEVICE_I2C_ADDR                 0x5d

#define REG                             GOODIX_REG_VERSION

#define GOODIX_READ_COOR_ADDR           0x814E
#define GOODIX_REG_CONFIG_DATA          0x8047
#define GOODIX_REG_VERSION              0x8140

int i2c_read(int fd, unsigned short reg, unsigned char *rbuf, int len){
    //i2c读取数据需要发送2个i2c_msg，一个用于写入要读取的寄存器地址，另一个用于接收读取的数据
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msgs[2];
    int ret = 0;

    i2c_data.nmsgs = 2;
    i2c_data.msgs = msgs;
    unsigned char tbuf[] = {
        reg >> 8,
        reg & 0xff,
    };
    msgs[0].flags = 0;                  //写标志
    msgs[0].addr  = DEVICE_I2C_ADDR;    //i2c地址
    msgs[0].len   = 2;                  //装有寄存器地址的buf的长度，若是8位寄存器此处应为1
    msgs[0].buf   = tbuf;               //寄存器地址

    msgs[1].flags = I2C_M_RD;           //读标志，实际值为0x1
    msgs[1].addr  = DEVICE_I2C_ADDR;    //i2c地址
    msgs[1].len   = len;                //读入的数据长度
    msgs[1].buf   = rbuf;               //读入数据的存放地址

    //驱动接口，际发送给驱动的数据结构是 i2c_data的指针
    ret = ioctl(fd, I2C_RDWR, (unsigned long) &i2c_data);
    if (ret < 0) {
        perror("ioctl");
        return ret;
    }

    return 0;
}

int i2c_write(int fd, unsigned char *tbuf, int len){
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msgs;
    int ret = 0;

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &msgs;

    msgs.flags = 0;                     //写标志
    msgs.addr  = DEVICE_I2C_ADDR;       //设备i2c地址
    msgs.len   = len;                   //i2c发送数据包的字节数，也就是msgs.buf的大小
    msgs.buf   = tbuf;                  //i2c要发送的数据包

    //驱动接口，实际发送给驱动的数据结构是 i2c_data的指针
    ret =ioctl(fd, I2C_RDWR, (unsigned long) &i2c_data);
    if (ret < 0) {
        perror("ioctl");
        return ret;
    }

    return 0;
}

int main (int argc, char **argv){

    int fd = -1;
    int ret = 0;

    if ((fd = open(DEVICE, O_RDWR)) < 0) {
        /* 错误处理 */
        perror("open");
        return -ENXIO;
    }

    ioctl(fd, I2C_SLAVE, (unsigned long)DEVICE_I2C_ADDR);  /*配置slave地址*/
    ioctl(fd, I2C_TIMEOUT, 10);         /*配置超时时间*/
    ioctl(fd, I2C_RETRIES, 2);          /*配置重试次数*/

    //写入数据到指定寄存器
    unsigned char tbuf[] = {            //i2c要发送的数据，
        REG >> 8,                       //第1或2字节是寄存器地址，16位寄存器使用2字节,8位寄存器使用1字节
        REG & 0xff,
        0x66,                           //要写入的数据
    };

    ret = i2c_write(fd, tbuf, sizeof(tbuf));
    if (ret != 0)
        printf("i2c_write failed\n");
    printf("write data seccess, write to reg %hx, value = %#x \n", REG, tbuf[2]);

    //从指定寄存器读取数据
    unsigned char rbuf;                 //接收数据buf
    ret = i2c_read(fd, REG, &rbuf, 1);  //从REG中读取1字节数据，存放在rbuf中
    if (ret< 0)
        printf("read data failed\n");
    else
        printf("read data success, read from register %hx, value is %#x \n", REG, rbuf);

    close(fd);

    return 0;
}
