# Lab2-1

## 特別注意事項:
1. Checked the “Advanced” option, so that you can find all options.
2. CMAKE_EXE_LINKER_FLAGS
> -lpthread -ldl -lrt ***-Wl,-rpath-link***=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/qt5.5_env/lib ***-Wl,-rpath-link***=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/usr/lib ***-Wl,-rpath-link***=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/lib
3. If you want to do 4.2.4.4 again, open cmake-gui. File -> Delete Cache, then "Configure"


## 5. Questions and Answers:
#### 5.1 What are the `cmake` and `make` for? What is the relationship between them?
Answer: 參考Wiki資料如下
    
#### [cmake](https://zh.wikipedia.org/wiki/CMake)
> CMake是個一個開源的跨平台自動化建構系統，用來管理軟體建置的程式，並不依賴於某特定編譯器，並可支援多層目錄、多個應用程式與多個函式庫。 它用組態檔控制建構過程（build process）的方式和Unix的make相似，只是CMake的組態檔取名為***CMakeLists.txt***。

#### [make](https://zh.wikipedia.org/wiki/Make)
> 在軟體開發中，make是一個工具程式（Utility software），經由讀取叫做「***makefile***」的檔案，自動化建構軟體。

#### 5.2 Why there are so many arguments in the compilation command(step 4.4.2)? What are they for?

Answer:

Compile options:
> `-I` Setting header file search paths for compiler</br>
> `-L` Setting library search paths for compiler</br>

Linker options:
> `-l` Link library for linker, e.g. `-lpthread -lopencv_world`</br>
> `-Wl` Following option is used for linker **NOT** for compiler</br>
> `-rpath-link` Linked library search paths for linker</br>
> `-rpath` Seting library search paths at runtime

[動態庫的鏈接和鏈接選項-L，-rpath-link，-rpath](https://www.cntofu.com/book/46/linux_system/1515.md)

#### 5.3 What is `libopencv_world.so.3.4.7` for? Why do we need to use `LD_LIBRARY_PATH=. ./demo` to run the executable? What would happen if we just run with `./demo`? Why?

Answer: `libopencv_world.so.3.4.7` is runtime library required for application. `LD_LIBRARY_PATH=.` assign the search path of runtime library to current directory. Demo will complain about library not found if we just run with `./demo`.
```
./demo: error while loading shared libraries: libopencv_world.so.3.4: cannot open shared object file: No such file or directory
```
This can be solved by add compiler option: `-Wl,rpath=.`. This option assign runtime library search path to current directory with application.

Improvement:
1. Create library folder on target board. e.g. `/usr/local/opencv/lib`
2. Copy `libopencv_world.so.3.4` to `/usr/local/opencv/lib`
3. Compile application with `-Wl,rpath=/usr/local/opencv/lib`

Then applicaion can run anywhere and refer runtime library with assigned path.

#### 5.4 It is so complex and difficult to show a picture by using the framebuffer. Why don’t we just use [cv::imshow](https://docs.opencv.org/3.4.7/d7/dfc/group__highgui.html#ga453d42fe4cb60e5723281a89973ee563) to do this work?

Answer: [cv::imshow](https://docs.opencv.org/3.4.7/d7/dfc/group__highgui.html#ga453d42fe4cb60e5723281a89973ee563) need window manager to display image in a "window".

以下是[cv::imshow](https://docs.opencv.org/3.4.7/d7/dfc/group__highgui.html#ga453d42fe4cb60e5723281a89973ee563)使用參考資料：</br>
* [Python 與 OpenCV 基本讀取、顯示與儲存圖片教學](https://blog.gtwang.org/programming/opencv-basic-image-read-and-write-tutorial/)

#### 5.5 What is a framebuffer?

Answer:

[What is Frame Buffer](https://ecomputernotes.com/computer-graphics/basic-of-computer-graphics/what-is-frame-buffer)

#### 5.6 What is the result of the command below? Why?
`sudo cat /dev/fb0 > fb0`</br>
`sudo cat fb0 > /dev/fb0`

Answer:

`sudo cat /dev/fb0 > fb0` 擷取framebuffer的資料，放到檔案fb0中</br>
`sudo cat fb0 > /dev/fb0` 將檔案fb0的資料，放進framebuffer中

Framebuffer格式輸出為raw data format，可使用以下命令轉換格式：

`$ sudo apt install libnetpbm10`</br>
`$ perl iraw2png <screen.raw >screen.png`

#### 5.7 You can find there is a file named fb1 under /dev directory. What is the difference between /dev/fb0 and /dev/fb1? Why we use /dev/fb0 rather than /dev/fb1?

Answer:

`/dev/fb0` main framebuffer</br>
`/dev/fb1` auxiliary framebuffer</br>

Activate auxiliary framebuffer (`/dev/fb1`)</br>
`$ echo 0 > /sys/class/graphics/fb1/blank`

## 6. Bonus:
#### Show bonus.png on the screen. It is ok to modify either your code or configurations above. However, the provided picture is not allowed to be modified. Board is not allowed to connect to the Internet also.

Follow steps described in document "109_lab_2-1.pdf".</br>
`$ cd opencv-3.4.7`</br>
`$ sudo cmake-gui` Step: 4.1.2, This command must start from opencv-3.4.7 source directory.

Search "PNG" and check following options.</br>
`BUILD_PNG`</br>
`WITH_PNG`

Click "Configure"

Click ""Generate"

`cd /usr/local/arm-opencv/build`</br>
`sudo make`</br>
`sudo make install`</br>

Copy `/usr/local/arm-opencv/install/lib/libopencv_world.so.3.4.7` to your target board and rename library to `libopencv_world.so.3.4`.

Run your program then get bonus point!!!

## References:
1. [Introduction to Framebuffer](https://www.slideshare.net/raspberrypi-tw/introduction-to-framebuffer)
2. [I.MX6 Framebuffer Overlay](http://www.armadeus.org/wiki/index.php?title=I.MX6_Framebuffer_Overlay)
3. [[OpenCV] Image file reading and writing](https://docs.opencv.org/3.4.7/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56)
