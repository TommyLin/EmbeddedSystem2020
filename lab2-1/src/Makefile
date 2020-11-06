
LINARO_ARM = /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf
QT5_ROOTFS = $(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env
ifndef OPENCV_SRC
	OPENCV_SRC = ~/embedded/opencv-3.4.7/modules
endif
ifndef OPENCV_ARM
	OPENCV_ARM = /usr/local/arm-opencv
endif

CC = arm-linux-gnueabihf-g++
CFLAGS = -I $(LINARO_ARM)/include/ \
		 -I $(OPENCV_SRC)/highgui/include/ \
		 -I $(OPENCV_SRC)/core/include/ \
		 -I $(OPENCV_SRC)/imgcodecs/include/ \
		 -I $(OPENCV_SRC)/imgproc/include/ \
		 -I $(OPENCV_SRC)/videoio/include/ \
		 -I $(OPENCV_ARM)/build/ \
		 -I /usr/local/arm-opencv/install/include/ \
		 -L $(OPENCV_ARM)/build/lib \
		 -L /usr/local/arm-opencv/install/lib/ \
		 -Wl,-rpath-link=$(LINARO_ARM)/arm-linux-gnueabihf/libc/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/qt5.5_env/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/usr/lib/ \
		 -Wl,-rpath=. \
		 -Wl,-rpath=/root \
		 -lpthread \
		 -lopencv_world

default: all

all: sample bonus

sample: source.cpp
	$(CC) $(CFLAGS) $? -o $@

bonus: bonus.cpp
	$(CC) $(CFLAGS) $? -o $@

.PHONY: clean
clean:
	rm -rf sample bonus
