# 嵌入式系統設計 期末專題


## :heavy_check_mark: [HDMI] Face Recognition



## :heavy_check_mark: [LCD] Thermometer

### 
1. Diaplay wallpapers (Same as Lab2-3)
   - Able to assign framebuffer device node
1. Overlay group ID and date time
   - Display on auxilary framebuffer /dev/fb1 by call OpenCV api [cv::putText](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/osd.cpp)
   - [osd.cpp] Get system time and format date / time string
   - [src/get_ip.cpp] Get device ip address
   

## :heavy_check_mark: Audio

### :cake: Install mpg123
`# apt -y install mpg123`

### Command to play mp3 in a folder
`# mpg123 /root/mp3`

### Install volume control utility "amixer" from alsa-utils
`# apt -y install alsa-utils`

### List devices
```
# amixer scontrols
Simple mixer control 'Headphone',0
Simple mixer control 'Headphone Mux',0
Simple mixer control 'Headphone Playback ZC',0
Simple mixer control 'PCM',0
Simple mixer control 'Mic',0
Simple mixer control 'Capture',0
Simple mixer control 'Capture Attenuate Switch (-6dB)',0
Simple mixer control 'Capture Mux',0
Simple mixer control 'Capture ZC',0
```
### Set volume (command line)
`# amixer sset 'Headphone' 80%`

### Get volume (command line)
```
# amixer sget 'Headphone'
Simple mixer control 'Headphone',0
  Capabilities: pvolume
  Playback channels: Front Left - Front Right
  Limits: Playback 0 - 127
  Mono:
  Front Left: Playback 127 [100%] [-13.40dB]
  Front Right: Playback 127 [100%] [-13.40dB]
```

### Volume Control (application)
- Source file: [project/src/volumectrl.cpp](https://github.com/TommyLin/EmbeddedSystem2020/blob/main/project/src/volumectrl.cpp)

#### rev: 0.2
- Command: `# volumectrl`
- Keyboard input code: 114 => volume +  (max = 100%)
- Keyboard input code: 115 => volume -  (min =   0%)
#### rev: 0.1 (obsolete)

- Command: `# volumectrl 27`
- GPIO27 => volume + (max = 100%)
- GPIO28 => volume - (min = 0%)


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
