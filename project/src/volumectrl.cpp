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


#define DEV_PATH "/dev/input/event3"

inline void msdelay(int milliseconds) { usleep(milliseconds * 1000); };


string get_command(unsigned int volume)
{
    string command = "amixer sset 'Headphone' " + to_string(volume) + "%";
    return command;
}


/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
    unsigned int volume = 100;
    unsigned int step = 7;
//    unsigned int key_delay = 500; /* ms */

    int keys_fd;
    struct input_event t;

    /*
     * 114 Volumn up
     * 115 Volumn down
     */
    keys_fd = open(DEV_PATH, O_RDONLY);
    if (keys_fd <= 0) {
        printf("open %s device error!/n", DEV_PATH);
        return -1;
    }

    system(get_command(100).c_str());

    while(1) {
        if (read(keys_fd, &t, sizeof(t)) == sizeof(t)) {
            if (t.type == EV_KEY) {
                switch (t.code) {
                case 114:
                    if (t.value == 0)
                        break;

                    if (volume == 100)
                        cout << "Vol = 100" << endl;
                    else {
                        volume += step;
                        if (volume > 100)
                            volume = 100;
                        system(get_command(volume).c_str());
                        cout << "Vol+" << endl;
                    }
                    break;
                case 115:
                    if (t.value == 0)
                        break;

                    if (volume == 0)
                        cout << "Vol = 0" << endl;
                    else {
                        if (volume > step)
                            volume -= step;
                        else
                            volume = 0;
                        system(get_command(volume).c_str());
                        cout << "Vol-" << endl;
                    }
                    break;
                }
            }
        }
        msdelay(100);
    }

    close(keys_fd);

    return 0;
}
