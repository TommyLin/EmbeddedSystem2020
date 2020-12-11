#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <iostream>

#include "gpio.h"

using namespace std;


/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
    /*
     * GPIO28 Volumn up
     * GPIO27 Volumn down
     */
    struct pollfd fdset[2];
    int nfds = 2;
    int gpio_fd, timeout, rc;
    char *buf[MAX_BUF];
    unsigned int gpio;


    if (argc < 2) {
        printf("Usage: gpio-int <gpio-pin>\n\n");
        printf("Waits for a change in the GPIO pin voltage level or input on stdin\n");
        exit(-1);
    }

    gpio = atoi(argv[1]);

    gpio_export(gpio);
    gpio_set_dir(gpio, 0);
    gpio_set_edge(gpio, (char *)"rising");
    gpio_fd = gpio_fd_open(gpio);

    timeout = POLL_TIMEOUT;

    while (1) {
        memset((void*)fdset, 0, sizeof(fdset));

        fdset[0].fd = STDIN_FILENO;
        fdset[0].events = POLLIN;

        fdset[1].fd = gpio_fd;
        fdset[1].events = POLLPRI;

        rc = poll(fdset, nfds, timeout);

        if (rc < 0) {
            printf("\npoll() failed!\n");
            return -1;
        }

        if (rc == 0) {
            printf(".");
        }

        if (fdset[1].revents & POLLPRI) {
            read(fdset[1].fd, buf, MAX_BUF);
            printf("\npoll() GPIO %d interrupt occurred\n", gpio);
        }

        if (fdset[0].revents & POLLIN) {
            (void)read(fdset[0].fd, buf, 1);
            printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
        }

        fflush(stdout);
    }

    gpio_fd_close(gpio_fd);

    return 0;
}
