# Lab 2-2

## Command line
### How to disable cursor
`$ echo 0 > /sys/class/graphics/fbcon/cursor_blink`

### Set LCD upside down
`$ echo 1 > /sys/class/graphics/fb0/rotate`

### Rstore LCD direction
`$ echo 0 > /sys/class/graphics/fb0/rotate`

## Using JPEG format
### Need to rebuild opencv library with "JPEG" support
Follow steps described in document "109_lab_2-1.pdf".</br>
`$ cd opencv-3.4.7`</br>
`$ sudo cmake-gui` Step: 4.1.2, This command must start from opencv-3.4.7 source directory.

Search "JPEG" and check following options.</br>
`BUILD_JPEG`</br>
`WITH_JPEG`</br>
`old-jpeg`

Click "Configure"

Click ""Generate"

`cd /usr/local/arm-opencv/build`</br>
`sudo make`</br>
`sudo make install`</br>

Copy `/usr/local/arm-opencv/install/lib/libopencv_world.so.3.4.7` to your target board and rename library to `libopencv_world.so.3.4`.

You can find pre-built opencv library here:
lab2-2/bin/libopencv_world.so.3.4
