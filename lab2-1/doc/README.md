## 特別注意事項:
1. Checked the “Advanced” option, so that you can find all options.
2. CMAKE_EXE_LINKER_FLAGS
> -lpthread -ldl -lrt ***-Wl,-rpath-link***=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/qt5.5_env/lib ***-Wl,-rpath-link***=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/usr/lib ***-Wl,-rpath-link***=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/lib
3. If you want to do 4.2.4.4 again, open cmake-gui. File -> Delete Cache, then "Configure"


## Lab2-1 5. Questions and Answers:
#### 5.1 What are the `cmake` and `make` for? What is the relationship between them?
Answer: 請自行參考Wiki資料如下
    
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

#### 5.4 It is so complex and difficult to show a picture by using the framebuffer. Why don’t we just use cv::imshow to do this work?

Answer: 參考資料，待測試

[Python 與 OpenCV 基本讀取、顯示與儲存圖片教學](https://blog.gtwang.org/programming/opencv-basic-image-read-and-write-tutorial/)

#### 5.5 What is a framebuffer?

Answer:

[What is Frame Buffer](https://ecomputernotes.com/computer-graphics/basic-of-computer-graphics/what-is-frame-buffer)

#### 5.6 What is the result of the command below? Why?
```
sudo cat /dev/fb0 > fb0
sudo cat fb0 > /dev/fb0
```

Answer: 以下待測試

`sudo cat /dev/fb0 > fb0` 擷取frame buffer的資料，放到檔案fb0中

`sudo cat fb0 > /dev/fb0` 將檔案fb0的資料，放進frame buffer中

#### 5.7 You can find there is a file named fb1 under /dev directory. What is the difference between /dev/fb0 and /dev/fb1? Why we use /dev/fb0 rather than /dev/fb1?

Answer:
