
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -Wextra

TARGET = v4l2_camera


.PHONY: clean test

$(TARGET): v4l2_camera.c mxcfb.h videodev2.h
	$(CC) $(CFLAGS) $? -o $@

test:
	./$(TARGET) -di /dev/video2

clean:
	rm -Rf $(TARGET)
