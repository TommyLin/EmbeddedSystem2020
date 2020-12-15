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


## :heavy_check_mark: Basic Framebuffer Information
| fb#      | Buffer Size | bits | virt. size |
| -------- | ----------: | ---- | ---------: |
| /dev/fb0 |   8,294,400 |  16  |    640X480 |
| /dev/fb1 |     921,600 |  32  |    240X960 |
| /dev/fb2 |   1,843,200 |  24  |   1024X600 |


## :heavy_check_mark: Command Line Interface

### Disable cursor blink
`# echo 0 > /sys/class/graphics/fbcon/cursor_blink`

### Enable frambuffer

#### Enable /dev/fb1
`# echo 0 > /sys/class/graphics/fb1/blank`

#### Enable /dev/fb2
`# echo 0 > /sys/class/graphics/fb2/blank`
