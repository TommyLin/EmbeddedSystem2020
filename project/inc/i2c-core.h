#pragma once


int i2c_read(int fd, unsigned short reg, unsigned char *rbuf, int len);
int i2c_write(int fd, unsigned char *tbuf, int len);
