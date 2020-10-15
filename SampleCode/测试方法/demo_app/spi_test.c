
    #include <stdint.h> 
    #include <unistd.h> 
    #include <stdio.h> 
    #include <stdlib.h> 
    #include <getopt.h> 
    #include <fcntl.h> 
    #include <sys/ioctl.h> 
    #include <linux/types.h> 
    #include <linux/spi/spidev.h> 
     
    #define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0])) 
     
    static void pabort(const char *s) 
    { 
        perror(s); 
        abort(); 
    } 
     
    static const char *device = "/dev/spidev1.0"; 
    static uint8_t mode; 
    static uint8_t bits = 8; 
    static uint32_t speed = 500000; 
    static uint16_t delay; 
     
    static void transfer(int fd) 
    { 
        int ret; 
        uint8_t tx[] = {    //要发送的数据数组 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0x40, 0x00, 0x00, 0x00, 0x00, 0x95, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD, 
            0xF0, 0x0D, 
        }; 
        uint8_t rx[ARRAY_SIZE(tx)] = {0, }; //接收的数据数据 
        struct spi_ioc_transfer tr = {  //声明并初始化spi_ioc_transfer结构体 
            .tx_buf = (unsigned long)tx, 
            .rx_buf = (unsigned long)rx, 
            .len = ARRAY_SIZE(tx), 
            .delay_usecs = delay, 
            .speed_hz = speed, 
            .bits_per_word = bits, 
        }; 
        //SPI_IOC_MESSAGE(1)的1表示spi_ioc_transfer的数量 
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);   //ioctl默认操作,传输数据 
        if (ret < 1) 
            pabort("can't send spi message"); 
     
        for (ret = 0; ret < ARRAY_SIZE(tx); ret++) { //打印接收缓冲区 
            if (!(ret % 6))     //6个数据为一簇打印 
                puts(""); 
            printf("%.2X ", rx[ret]); 
        } 
        puts(""); 
    } 
     
    static void print_usage(const char *prog)   //参数错误则打印帮助信息 
    { 
        printf("Usage: %s [-DsbdlHOLC3]\n", prog); 
        puts("  -D --device   device to use (default /dev/spidev1.1)\n" 
             "  -s --speed    max speed (Hz)\n" 
             "  -d --delay    delay (usec)\n" 
             "  -b --bpw      bits per word \n" 
             "  -l --loop     loopback\n" 
             "  -H --cpha     clock phase\n" 
             "  -O --cpol     clock polarity\n" 
             "  -L --lsb      least significant bit first\n" 
             "  -C --cs-high  chip select active high\n" 
             "  -3 --3wire    SI/SO signals shared\n"); 
        exit(1); 
    } 
     
    static void parse_opts(int argc, char *argv[]) 
    { 
        while (1) { 
            static const struct option lopts[] = {  //参数命令表 
                { "device",  1, 0, 'D' }, 
                { "speed",   1, 0, 's' }, 
                { "delay",   1, 0, 'd' }, 
                { "bpw",     1, 0, 'b' }, 
                { "loop",    0, 0, 'l' }, 
                { "cpha",    0, 0, 'H' }, 
                { "cpol",    0, 0, 'O' }, 
                { "lsb",     0, 0, 'L' }, 
                { "cs-high", 0, 0, 'C' }, 
                { "3wire",   0, 0, '3' }, 
                { "no-cs",   0, 0, 'N' }, 
                { "ready",   0, 0, 'R' }, 
                { NULL, 0, 0, 0 }, 
            }; 
            int c; 
     
            c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL); 
     
            if (c == -1) 
                break; 
     
            switch (c) { 
            case 'D':   //设备名 
                device = optarg; 
                break; 
            case 's':   //速率 
                speed = atoi(optarg); 
                break; 
            case 'd':   //延时时间 
                delay = atoi(optarg); 
                break; 
            case 'b':   //每字含多少位 
                bits = atoi(optarg); 
                break; 
            case 'l':   //回送模式 
                mode |= SPI_LOOP; 
                break; 
            case 'H':   //时钟相位 
                mode |= SPI_CPHA; 
                break; 
            case 'O':   //时钟极性 
                mode |= SPI_CPOL; 
                break; 
            case 'L':   //lsb 最低有效位 
                mode |= SPI_LSB_FIRST; 
                break; 
            case 'C':   //片选高电平 
                mode |= SPI_CS_HIGH; 
                break; 
            case '3':   //3线传输模式 
                mode |= SPI_3WIRE; 
                break; 
            /*case 'N':   //没片选 
                mode |= SPI_NO_CS; 
                break; 
            case 'R':   //从机拉低电平停止数据传输 
                mode |= SPI_READY; 
                break;*/ 
            default:    //错误的参数 
                print_usage(argv[0]); 
                break; 
            } 
        } 
    } 
     
    int main(int argc, char *argv[]) 
    { 
        int ret = 0; 
        int fd; 
     
        parse_opts(argc, argv); //解析传递进来的参数 
     
        fd = open(device, O_RDWR);  //打开设备文件 
        if (fd < 0) 
            pabort("can't open device"); 
     
        /*
         * spi mode //设置spi设备模式
         */ 
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);    //写模式 
        if (ret == -1) 
            pabort("can't set spi mode"); 
     
        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);    //读模式 
        if (ret == -1) 
            pabort("can't get spi mode"); 
     
        /*
         * bits per word    //设置每个字含多少位
         */ 
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);   //写 每个字含多少位 
        if (ret == -1) 
            pabort("can't set bits per word"); 
     
        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);   //读 每个字含多少位 
        if (ret == -1) 
            pabort("can't get bits per word"); 
     
        /*
         * max speed hz     //设置速率
         */ 
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);   //写速率 
        if (ret == -1) 
            pabort("can't set max speed hz"); 
     
        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);   //读速率 
        if (ret == -1) 
            pabort("can't get max speed hz"); 
        //打印模式,每字多少位和速率信息 
        printf("spi mode: %d\n", mode); 
        printf("bits per word: %d\n", bits); 
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000); 
     
        transfer(fd);   //传输测试 
     
        close(fd);  //关闭设备 
     
        return ret; 
    } 

