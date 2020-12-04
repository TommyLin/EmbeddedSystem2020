
LINARO_ARM = /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf
QT5_ROOTFS = $(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env
ifndef OPENCV_ARM
	OPENCV_ARM = /usr/local/arm-opencv
endif

CC       = arm-linux-gnueabihf-gcc
CXX      = arm-linux-gnueabihf-g++
CXXFLAGS = -std=c++11 \
		 -Wall \
		 -I $(LINARO_ARM)/include/ \
		 -I $(OPENCV_ARM)/install/include/ \
		 -I ../inc

LDFLAGS = \
		 -L $(OPENCV_ARM)/install/lib/ \
		 -Wl,-rpath-link=$(LINARO_ARM)/arm-linux-gnueabihf/libc/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/qt5.5_env/lib/ \
		 -Wl,-rpath-link=$(QT5_ROOTFS)/usr/lib/ \
		 -Wl,-rpath=. \
		 -Wl,-rpath=/root \
		 -lpthread \
		 -lopencv_world
SRCS    = capture.cpp getch.cpp getfbi.cpp
OBJS    = $(SRCS:.cpp=.o)
TARGET  = capture

.DEFAULT_GOAL = all
.PHONY: clean lint upload

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $? -o $@ $(LDFLAGS)

# https://github.com/github/super-linter
lint:
	sudo docker run -e RUN_LOCAL=true -v $(shell pwd):/tmp/lint github/super-linter

# Implicit rule already done by default
#%: %.cpp
#	$(CC) $(CXXFLAGS) $? -o $@

upload:
	scp $(TARGET) root@$(E9V3_IP):/root/

clean:
	@rm -rfv $(TARGET) $(OBJS)
	@rm -rfv super-linter.log
