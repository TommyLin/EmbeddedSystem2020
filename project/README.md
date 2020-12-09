# 嵌入式系統設計 期末專題


## [HDMI] Face Recognition



## [LCD] Thermometer

### 
1. Diaplay wallpapers (Same as Lab2-3)
   - Able to assign framebuffer device node
1. Overlay group ID and date time
   - Get system time
   - Format date / time string
   - Display on auxilary framebuffer /dev/fb1 by call OpenCV api [cv::putText](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)

## Basic Framebuffer Information
| fb#      | Buffer Size | bits | virt. size |
| -------- | ----------: | ---- | ---------: |
| /dev/fb0 |   8,294,400 |  16  |    640X480 |
| /dev/fb1 |     921,600 |  32  |    240X960 |
| /dev/fb2 |   1,843,200 |  24  |   1024X600 |

## Command Line Interface
### Disable cursor blink
`# echo 0 > /sys/class/graphics/fbcon/cursor_blink`
### Enable frambuffer
#### Enable /dev/fb1
`# echo 0 > /sys/class/graphics/fb1/blank`
#### Enable /dev/fb2
`# echo 0 > /sys/class/graphics/fb2/blank`
