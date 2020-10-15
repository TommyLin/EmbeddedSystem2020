#!/bin/bash

CC=arm-linux-gnueabihf-gcc

${CC} gpio_test_in.c -o gpio_test_in
${CC} gpio_test_out.c -o gpio_test_out

