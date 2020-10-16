
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -Wextra

TARGET = spi_test


.PHONY: clean test

$(TARGET): spi_test.c
	$(CC) $(CFLAGS) $? -o $@

test: $(TARGET)
	./$(TARGET) -s 50000 -p "ABCDEFG"

clean:
	rm -Rf $(TARGET)
