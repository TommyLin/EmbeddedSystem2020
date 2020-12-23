#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <linux/input.h>

#include <iostream>
#include "gpio.h"

using namespace std;


#define DEV_PATH "/dev/input/by-path/platform-21a0000.i2c-event"

inline void msdelay(int milliseconds) { usleep(milliseconds * 1000); };



/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
    int keys_fd;
    struct input_event t;

    keys_fd = open(DEV_PATH, O_RDONLY);
    if (keys_fd <= 0) {
        printf("open %s device error!/n", DEV_PATH);
        return -1;
    }

    while (1) {
        if (read(keys_fd, &t, sizeof(t)) == sizeof(t)) {
#if 0
            printf("type:%04x  ", t.type);
            printf("code:%04x  ", t.code);
            printf("value:%08x\n", t.value);
#else
            cout << "type:" << t.type << " code:" << t.code << " value:" << t.value << endl;
#endif
        }
        msdelay(20);
    }

    close(keys_fd);

    return 0;
}
