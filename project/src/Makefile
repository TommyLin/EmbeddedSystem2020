
LINARO_ARM = /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf
QT5_ROOTFS = $(LINARO_ARM)/qt5.5/rootfs_imx6q_V3_qt5.5_env
ifndef OPENCV_ARM
OPENCV_ARM = /usr/local/arm-opencv
endif

CC       = arm-linux-gnueabihf-gcc
CXX      = arm-linux-gnueabihf-g++
CFLAGS   = \
		   -Wall \
		   -I $(OPENCV_ARM)/install/include/ \
		   -I ../inc
CXXFLAGS = -std=c++11 $(CFLAGS)

LDFLAGS  = \
		   -L $(OPENCV_ARM)/install/lib/ \
		   -Wl,-rpath-link=$(LINARO_ARM)/arm-linux-gnueabihf/libc/lib/ \
		   -Wl,-rpath-link=$(QT5_ROOTFS)/lib/ \
		   -Wl,-rpath-link=$(QT5_ROOTFS)/qt5.5_env/lib/ \
		   -Wl,-rpath-link=$(QT5_ROOTFS)/usr/lib/ \
		   -Wl,-rpath=. \
		   -Wl,-rpath=/root \
		   -lpthread \
		   -lopencv_world

SRCS     = \
		   getch.cpp \
		   getfbi.cpp \
		   DHT.cpp \
		   get_ip.cpp \
		   gpio.cpp
OBJS     = $(addprefix $(BUILDDIR)/,$(SRCS:.cpp=.o))
CSRCS    =
COBJS    = $(CSRCS:.c=.o)
BINARIES = auto_play show_fbi osd volumectrl gpio_monitor
TARGET   = $(addprefix $(BUILDDIR)/,$(BINARIES))
BUILDDIR = output

.DEFAULT_GOAL = all
.PHONY: clean lint upload

all: prepare $(OBJS) $(COBJS) $(TARGET)

test:
	@echo $(TARGET)

prepare:
	@if [ ! -d $(BUILDDIR) ]; then mkdir -pv $(BUILDDIR); fi

$(BUILDDIR)/%: %.c $(COBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(COBJS) -o $@

$(BUILDDIR)/%: %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(OBJS) -o $@

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $? -o $@

# https://github.com/github/super-linter
lint:
	sudo docker run -e RUN_LOCAL=true -v $(shell pwd):/tmp/lint github/super-linter

# Please setup IP address from environment variable. Such as
# export E9V3_IP=192.168.1.10
upload:
	./upload.exp $(TARGET) root /root

clean:
	@rm -rfv $(TARGET) $(OBJS) $(COBJS)
	@rm -rfv super-linter.log
