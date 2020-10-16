
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -Wextra

TARGET = uart_app


.PHONY: clean test

$(TARGET): uart_app.c
	$(CC) $(CFLAGS) $? -o $@

test: $(TARGET)
	./$(TARGET) /dev/ttySAC1 -b 115200 -rw 123asd -asc -l

clean:
	rm -Rf $(TARGET)
