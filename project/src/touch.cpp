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

    cout << "EV_KEY=" << EV_KEY << endl;
    cout << "EV_ABS=" << EV_ABS << endl;

    while (1) {
        if (read(keys_fd, &t, sizeof(t)) == sizeof(t)) {
            if (t.type == EV_KEY) {
                switch (t.code) {
                case BTN_TOUCH: /* 0x14a */
                    cout << "^";
                    system("echo 0 > /sys/class/graphics/fb1/blank");
                    break;
                }

                if (t.value == 0)
                    system("echo 1 > /sys/class/graphics/fb1/blank");
            }

            if (t.type == EV_ABS) {
                cout << ".";
            }
        }
        msdelay(10);
    }

    close(keys_fd);

    return 0;
}
