
LINARO_ARM = /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf
QT5_ROOTFS = $(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env
ifndef OPENCV_ARM
	OPENCV_ARM = /usr/local/arm-opencv
endif

CC = arm-linux-gnueabihf-g++
CFLAGS = -I $(LINARO_ARM)/include/ \
		 -I $(OPENCV_ARM)/install/include/ \
		 -L $(OPENCV_ARM)/install/lib/ \
		 -Wl,-rpath-link=$(LINARO_ARM)/arm-linux-gnueabihf/libc/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/qt5.5_env/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/usr/lib/ \
		 -Wl,-rpath=. \
		 -Wl,-rpath=/root \
		 -lpthread \
		 -lopencv_world

default: all

all: lab22demo capture

lab22demo: lab_2-2_source.cpp
	$(CC) $(CFLAGS) $? -o $@

capture: capture.cpp
	$(CC) $(CFLAGS) $? -o $@

.PHONY: clean
clean:
	rm -rf lab22demo capture
