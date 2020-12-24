# 嵌入式系統設計 期末專題


## :heavy_check_mark: [HDMI] Face Recognition
### :mag: Face Detect
- Source: [project/FaceID/FaceDetection.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/FaceID/FaceDetection.cpp)

### :mag: Face Recognition
- Source: [project/FaceRec/facerec_from_webcam_faster_speedup.py](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/FaceRec/facerec_from_webcam_faster_speedup.py)



## :heavy_check_mark: [LCD] Display

### :tv: Background (`/dev/fb0`)
Diaplay wallpapers (same as Lab2-3)
- Command: `# /root/auto_play`
- Source: [project/src/auto_play.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/auto_play.cpp)
   - Able to assign framebuffer device node

### :tv: Foreground (`/dev/fb1`)
Overlay information (OSD)
- Command: `# /root/osd`
- Source: [project/src/osd.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)
   - Get system time, format date / time string then display on screen
- Source: [project/src/get_ip.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/get_ip.cpp)
   - Get ethernet (eth0) ip address
- Source: [project/src/gpio.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/gpio.cpp)
   - Show or no show OSD is controlled by IR detector which connected to GPIO27
   - GPIO API is called here: [project/src/osd.cpp line 53](https://github.com/TommyLin/EmbeddedSystem2020/blob/d8c9fb26e4e9000c02f17c13cc9672b691d6b903/project/src/osd.cpp#L53)
- Source: [project/src/touch.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/touch.cpp)
   - Using touch panel (touch) button to control OSD on / off

### :tv: References
- Enable `/dev/fb1`
  ```
  # echo 0 > /sys/class/graphics/fb1/blank
  ```
- Overlay text on auxilary framebuffer `/dev/fb1` by calling [OpenCV api cv::putText](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)
- List all input device information. Device node /dev/input/event*
  ```
  # sudo apt-get install input-utils
  # lsinput
  ```


## :heavy_check_mark: Audio

### :musical_note: Prepare testing environment
- [How to install mpg123 on E9V3](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/doc/howto_install_mpg123.md)

### :musical_note: Play mp3 files in a folder
-
   ```
   # mpg123 /root/mp3/spring/*
   ```

### :musical_note: Volume Control (application)
- Command: `# /root/volumectrl`
- Source: [project/src/volumectrl.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/volumectrl.cpp)
- Keyboard input code: KEY_VOLUMEDOWN(114) => volume -  (min =   0%)
- Keyboard input code: KEY_VOLUMEUP(115)   => volume +  (max = 100%)



## :heavy_check_mark: USB Device Mode (gadget)
![USB](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/doc/usb.png)

### :link: Mass storage class (g_mass_storage.ko)
- Prepare a 20 MB image file (do it once)
   ```
   # cd /root
   # dd if=/dev/zero of=vfat.img bs=1M count=20
   # mkfs.vfat vfat.img
   ```
- Insert usb mass storage device mode driver
   ```
   # losetup /dev/loop0 /root/vfat.img
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_mass_storage.ko file=/dev/loop0 stall=0 removable=1 iSerialNumber=123456
   ```
- Plug usb into host, a storage device will appear.
   - Linux - /dev/sdx, e.g. /dev/sdb
   - Windows - Diskx, e.g. H:

### :link: USB serial (g_serial.ko)
-
   ```
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_serial.ko
   ```
- Plug usb into host, a serial device will appear.
   - Linux - /dev/ttyACMn, e.g. /dev/ttyACM0
   - Windows - COMn, e.g. COM6

### :link: [Remote Network Driver Interface Specification (RNDIS)](https://docs.microsoft.com/zh-hk/windows-hardware/drivers/network/remote-ndis--rndis-2) (g_ether.ko)
-
   ```
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_ether.ko
   ```
   :beetle: Segmentation fault
   ```
   [ 3560.070472] [<8082439c>] (register_netdevice) from [<80824608>] (register_netdev+0x14/0x24)
   [ 3560.077546] [<80824608>] (register_netdev) from [<8061e2e4>] (gether_register_netdev+0x1c/0xc4)
   [ 3560.084969] [<8061e2e4>] (gether_register_netdev) from [<7f177198>] (eth_bind+0xe8/0x3e0 [g_ether])
   ......
   ```

### :link: [Network Control Model (NCM) Devices](https://www.usb.org/document-library/network-control-model-devices-specification-v10-and-errata-and-adopters-agreement) (g_ncm.ko)
-
   ```
   # insmod -f /lib/modules/4.1.15-1.0.0+g3924425/kernel/drivers/usb/gadget/legacy/g_ncm.ko
   ```
   :warning: There is NO WINDOWS built-in device driver for NCM.

- Configuration after load NCM device driver
   |                  |Device side                    | Host side                                |
   | ---------------- | ----------------------------- | ---------------------------------------- |
   | Enable interface | `# ifconfig usb0 up`          |                                          |
   | Configure IP     | `# ifconfig usb0 192.168.1.1` | `# ifconfig enx5a5098106ac6 192.168.1.2` |
   | Testing          | `# ping 192.168.1.2`          | `# ping 192.168.1.1`                     |

- Furthur ethernet testing
   - `# ssh root@192.168.1.1` from host

### Refernces
   - Rebuild module dependency: `# /sbin/depmod -a`
   - List all ethernet devices: `# ifconfig -a`
   - [编译kernel固件](http://wiki.armbbs.net/tqwiki/public/docs/E9V3#10802) from E9V3 - Embedsky WIKI
   - [Build kernel from source](https://github.com/TommyLin/EmbeddedSystem2020/edit/main/project/doc/rebuild_kernel.md)



## :heavy_check_mark: Basic Framebuffer Information
- Source: [project/src/show_fbi.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/show_fbi.cpp)
-
   | Device Node | Buffer Size | bits | Virt. Size |
   | ----------- | ----------: | ---- | ---------: |
   | /dev/fb0    |   8,294,400 |  16  | 640X480(1) |
   | /dev/fb1    |     921,600 |  32  | 240X320X3  |
   | /dev/fb2    |   1,843,200 |  24  |   1024X600 |

   *1. The resolution varies with monitor attached. It can be 640x480 or 1280x1024.



## :heavy_check_mark: Command Line Interface

### Disable cursor blink
- `# echo 0 > /sys/class/graphics/fbcon/cursor_blink`

### Enable frambuffer (show pixels in display)
- Enable /dev/fb1 `# echo 0 > /sys/class/graphics/fb1/blank`
- Enable /dev/fb2 `# echo 0 > /sys/class/graphics/fb2/blank`
