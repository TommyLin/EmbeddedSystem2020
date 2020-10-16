
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -Wextra


.PHONY: clean test

all: gpio_test_in gpio_test_out

gpio_test_in: gpio_test_in.c
	$(CC) $(CFLAGS) $? -o $@

gpio_test_out: gpio_test_out.c
	$(CC) $(CFLAGS) $? -o $@


test:
	./gpio_test_out 7~10 1
	./gpio_test_in 11~14 1

clean:
	rm -Rf gpio_test_in gpio_test_out
