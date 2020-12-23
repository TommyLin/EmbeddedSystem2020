# 嵌入式系統設計 期末專題


## :heavy_check_mark: [HDMI] Face Recognition



## :heavy_check_mark: [LCD] Display

### 
1. [Background `/dev/fb0`] Diaplay wallpapers (Same as Lab2-3)
   - [project/src/auto_play.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/auto_play.cpp) - Able to assign framebuffer device node
1. [Foreground `/dev/fb1`] Overlay information
   - [project/src/osd.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp) - Get system time and format date / time string
   - [project/src/get_ip.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/get_ip.cpp) - Get device (eth0) ip address

#### Notes
   - Enable /dev/fb1
     ```
     # echo 0 > /sys/class/graphics/fb1/blank
     ```
   - Display on auxilary framebuffer /dev/fb1 by call [OpenCV api cv::putText](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)
   

## :heavy_check_mark: Audio

* :cake: [How to install mpg123 on E9V3](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/doc/howto_install_mpg123.md)

### Play mp3 files in a folder
```
# mpg123 /root/mp3/spring/*
```

### Volume Control (application)
- Source file: [project/src/volumectrl.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/volumectrl.cpp)
- Command: `# volumectrl`
- Keyboard input code: 114 => volume +  (max = 100%)
- Keyboard input code: 115 => volume -  (min =   0%)


## :heavy_check_mark: USB Device Mode

### [Gadget] Mass storage class
- Prepare image file (do it once)
   ```
   # dd if=/dev/zero of=vfat.img bs=1M count=20
   # mkfs.vfat vfat.img
   ```
- Insert usb mass storage device mode driver
   ```
   # losetup /dev/loop0 vfat.img
   # cd /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/
   # insmod -f gadgetfs.ko
   # insmod -f g_mass_storage.ko file=/dev/loop0 stall=0 removable=1 iSerialNumber=123456
   ```
   - Plug usb into host
   
* Rebuild module dependency
   ```
   /sbin/depmod -a
   ```

### [Gadget] USB serial
   ```
   # cd /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/
   # insmod -f g_serial.ko
   ```

### [Gadget] rndis
   ```
   # cd /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/
   # insmod -f g_ether.ko
   ```
   NG =< Core dump

### [Gadget] NCM???
   ```
   # cd /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/
   # insmod -f g_ncm.ko
   ```
   No built-in windows driver



## :heavy_check_mark: Basic Framebuffer Information
| fb#      | Buffer Size | bits | virt. size |
| -------- | ----------: | ---- | ---------: |
| /dev/fb0 |   8,294,400 |  16  | 640X480(1) |
| /dev/fb1 |     921,600 |  32  | 240X320X3  |
| /dev/fb2 |   1,843,200 |  24  |   1024X600 |

*1. The resolution varies with monitor attached. It can be 640x480 or 1280x1024.


## :heavy_check_mark: Command Line Interface

### Disable cursor blink
`# echo 0 > /sys/class/graphics/fbcon/cursor_blink`

### Enable frambuffer

#### Enable /dev/fb1
`# echo 0 > /sys/class/graphics/fb1/blank`

#### Enable /dev/fb2
`# echo 0 > /sys/class/graphics/fb2/blank`
