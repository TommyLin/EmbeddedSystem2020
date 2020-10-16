
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -Wextra

TARGET = sample


.PHONY: clean

$(TARGET): sample.c
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -Rf $(TARGET)
