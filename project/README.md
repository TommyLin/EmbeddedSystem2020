# 嵌入式系統設計 期末專題


## :heavy_check_mark: [HDMI] Face Recognition



## :heavy_check_mark: [LCD] Display

### :tv: Background
1. [`/dev/fb0`] Diaplay wallpapers (Same as Lab2-3)
   - [project/src/auto_play.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/auto_play.cpp)
      * Able to assign framebuffer device node

### :tv: Foreground
1. [`/dev/fb1`] Overlay information
   - [project/src/osd.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)
      * Get system time and format date / time string
   - [project/src/get_ip.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/get_ip.cpp)
      * Get device (eth0) ip address
   - [project/src/gpio.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/gpio.cpp)
      * Show or no show OSD controlled by IR detector connected to GPIO27
      * [project/src/osd.cpp line 53](https://github.com/TommyLin/EmbeddedSystem2020/blob/d8c9fb26e4e9000c02f17c13cc9672b691d6b903/project/src/osd.cpp#L53)

### :tv: Notes
   - Enable /dev/fb1
     ```
     # echo 0 > /sys/class/graphics/fb1/blank
     ```
   - Overlay text on auxilary framebuffer `/dev/fb1` by calling [OpenCV api cv::putText](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)
   

## :heavy_check_mark: Audio

* :cake: [How to install mpg123 on E9V3](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/doc/howto_install_mpg123.md)

### :musical_note: Play mp3 files in a folder
```
# mpg123 /root/mp3/spring/*
```

### :musical_note: Volume Control (application)
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
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_serial.ko
   ```

### [Gadget] rndis
   ```
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_ether.ko
   ```
   :beetle: NG =< Core dump

### [Gadget] NCM???
   ```
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_ncm.ko
   ```
   :beetle: No built-in windows driver



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
