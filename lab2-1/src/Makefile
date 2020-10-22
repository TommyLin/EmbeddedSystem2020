
LINARO_ARM = /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/
OPENCV_SRC = ~/embedded/opencv-3.4.7/
OPENCV_ARM = /usr/local/arm-opencv/

CC = arm-linux-gnueabihf-g++
CFLAGS = -I $(LINARO_ARM)/include/ \
		 -I $(OPENCV_SRC)/modules/highgui/include/ \
		 -I $(OPENCV_SRC)/modules/core/include/ \
		 -I $(OPENCV_SRC)/modules/imgcodecs/include/ \
		 -I $(OPENCV_SRC)/modules/imgproc/include/ \
		 -I $(OPENCV_SRC)/modules/videoio/include/ \
		 -I $(OPENCV_ARM)/build/ \
		 -I /usr/local/arm-opencv/install/include/ \
		 -L /usr/local/arm-opencv/install/lib/ \
		 l,-rpath-link=$(LINARO_ARM)/arm-linux-gnueabihf/libc/lib/ \
		 -Wl,-rpath-link=$(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env/lib/ \
		 -Wl,-rpath-link=$(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env/qt5.5_env/lib/ \
		 -Wl,-rpath-link=$(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env/usr/lib/ \
		 -lpthread
#		 -lpthread -lopencv_world


demo: source.cpp
	$(CC) $(CFLAGS) $? -o $@

.PHONY: clean
clean:
	rm demo
