#include <stdio.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>


// TODO Change I2C address to DHT11. It might be 0x44(python) or 0x38(DHT.h)
#define DEVICE_I2C_ADDR                 0x5d


int i2c_read(int fd, unsigned short reg, unsigned char *rbuf, int len){
    //i2c读取数据需要发送2个i2c_msg，一个用于写入要读取的寄存器地址，另一个用于接收读取的数据
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msgs[2];
    int ret = 0;

    i2c_data.nmsgs = 2;
    i2c_data.msgs = msgs;
    unsigned char tbuf[] = {
        (char)(reg >> 8),
        (char)(reg & 0xff),
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
