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


#define DEV_PATH "/dev/input/by-path/platform-gpio-keys-event"

inline void msdelay(int milliseconds) { usleep(milliseconds * 1000); };


class volume_ctrl
{
public:
    volume_ctrl(int volume)
    {
        set_volume(volume);
    }

    void set_volume(int volume)
    {
        string command = "amixer -q sset 'Headphone' " + to_string(volume) + "%";
        system(command.c_str());
    }

    void inc(int step)
    {
        if (volume >= 100) {
            cout << "Vol = 100" << endl;
        } else {
            volume += step;
            if (volume > 100)
                volume = 100;
            set_volume(volume);
            cout << "Vol+ (" << volume << ")" << endl;
        }
    }

    void dec(int step)
    {
        if (volume <= 0) {
            cout << "Vol = 0" << endl;
        } else {
            volume -= step;
            if (volume < 0)
                volume = 0;
            set_volume(volume);
            cout << "Vol- (" << volume << ")" << endl;
        }
    }

private:
    int volume = 100;
};


/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
    int step = 7;
    int keys_fd;
    struct input_event t;

    volume_ctrl vc(100);

    /*
     * 114 Volumn up
     * 115 Volumn down
     */
    keys_fd = open(DEV_PATH, O_RDONLY);
    if (keys_fd <= 0) {
        printf("open %s device error!/n", DEV_PATH);
        return -1;
    }

    while (1) {
        if (read(keys_fd, &t, sizeof(t)) == sizeof(t)) {
            if (t.value == 0)
                /* Key release, don't care */
                continue;
            if (t.type == EV_KEY) {
                switch (t.code) {
                case 114:
                    vc.inc(step);
                    break;
                case 115:
                    vc.dec(step);
                    break;
                }
            }
        }
        msdelay(100);
    }

    close(keys_fd);

    return 0;
}
