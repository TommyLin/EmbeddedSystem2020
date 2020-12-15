#include <iostream>
#include <time.h>
#include <unistd.h>

#include <opencv2/imgcodecs.hpp>

#include "getfbi.hpp"
#include "gpio.h"

using namespace std;


#define font    cv::FONT_HERSHEY_SIMPLEX
#define color   cv::Scalar(0, 255, 25)

char *get_ip(char *interface);


int main(int argc, const char *argv[])
{
    const char *dev = "/dev/fb1";
    framebuffer_info fb_info = get_framebuffer_info(dev);
    ofstream ofs(dev);
    string text = "Group 19";
    cv::Mat image2, image;// = cv::Mat::zeros(cv::Size(fb_info.xres_virtual, fb_info.yres_virtual), CV_64FC1);

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    string datetime;

    bool osd_on = false;

    string ipaddr = "IP: ";
    ipaddr.append(get_ip((char *)"eth0"));

    system("echo 0 > /sys/class/graphics/fb1/blank");
    sleep(3);
    system("echo 1 > /sys/class/graphics/fb1/blank");

    while (1) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
        string datetime(buffer);

        image = cv::imread("/root/black240x100.jpg");
        cv::putText(image, text,     cv::Point(10, 40), font, 1, color, 1, cv::LINE_AA);
        cv::putText(image, datetime, cv::Point(10, 75), font, 0.5, color, 1, cv::LINE_AA);
        cv::putText(image, ipaddr,   cv::Point(10, 95), font, 0.5, color, 1, cv::LINE_AA);
        cv::cvtColor(image, image2, cv::COLOR_BGR2BGRA);
        set_framebuffer(&ofs, &image2, image.size(), fb_info);
        switch (get_value(27)) {
        case 0:
            if (!osd_on) {
                osd_on = true;
                system("echo 0 > /sys/class/graphics/fb1/blank");
            }
            break;
        case 1:
            if (osd_on) {
                osd_on = false;
                system("echo 1 > /sys/class/graphics/fb1/blank");
            }
            break;
        }
        sleep(1);
    }

    return 0;
}

