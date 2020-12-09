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

#include "i2c-core.h"

#define DEVICE                          "/dev/i2c-0"
// TODO Change I2C address to DHT11. It might be 0x44(python) or 0x38(DHT.h)
#define DEVICE_I2C_ADDR                 0x5d

#define REG                             GOODIX_REG_VERSION

#define GOODIX_READ_COOR_ADDR           0x814E
#define GOODIX_REG_CONFIG_DATA          0x8047
#define GOODIX_REG_VERSION              0x8140


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
