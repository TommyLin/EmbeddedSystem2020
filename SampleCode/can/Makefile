
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -Wextra

TARGET = can_test


.PHONY: clean

$(TARGET): can_test.c
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -Rf $(TARGET)
