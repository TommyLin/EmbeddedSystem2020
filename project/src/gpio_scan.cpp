#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <iostream>
#include "gpio.h"

using namespace std;


unsigned int ms = 1000;


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
    /*
     * GPIO28 Volumn up
     * GPIO27 Volumn down
     */
    unsigned int gpio;
    unsigned int value, normal;
    unsigned int volume = 100;
    unsigned int step = 7;
    unsigned int key_delay = 500 * ms;


    if (argc < 2) {
        printf("Usage: gpio-int <gpio-pin>\n\n");
        printf("Waits for a change in the GPIO pin voltage level or input on stdin\n");
        exit(-1);
    }

    system(get_command(100).c_str());

    gpio = atoi(argv[1]);
    normal = get_value(gpio);

    while (1) {
        value = get_value(gpio);
        if (value != normal) {
            cout << normal << " => " << value << endl;
            while (1) {
                if (volume == 100)
                    cout << "Vol = 100" << endl;
                else {
                    volume += step;
                    if (volume > 100)
                        volume = 100;
                    system(get_command(volume).c_str());
                    cout << "Vol+" << endl;
                }
                value = get_value(gpio);
                if (value == normal)
                    break;
                usleep(key_delay);
            }
        }
        value = get_value(gpio+1);
        if (value != normal) {
            cout << normal << " => " << value << endl;
            while (1) {
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
                value = get_value(gpio+1);
                if (value == normal)
                    break;
                usleep(key_delay);
            }
        }
        usleep(100 * ms);
    }

    return 0;
}
