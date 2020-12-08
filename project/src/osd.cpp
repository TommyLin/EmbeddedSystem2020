#include <iostream>
#include <time.h>
#include <unistd.h>

#include <opencv2/imgcodecs.hpp>

#include "getfbi.hpp"

using namespace std;


#define font    cv::FONT_HERSHEY_SIMPLEX
#define color   cv::Scalar(0, 255, 25)

int main(int argc, const char *argv[])
{
    const char *dev = "/dev/fb1";
    framebuffer_info fb_info = get_framebuffer_info(dev);
    ofstream ofs(dev);
    string text = "Group 19";
    cv::Mat image2, image;// = cv::Mat::zeros(cv::Size(fb_info.xres_virtual, fb_info.yres_virtual), CV_64FC1);

    time_t rawtime;
    string datetime;


    while (1) {
        time(&rawtime);
        string datetime = asctime(localtime(&rawtime));
        std::cout << datetime;

        image = cv::imread("/root/black240x100.jpg");
        cv::putText(image, text, cv::Point(10, 40), font, 1, color, 1, cv::LINE_AA);
        cv::putText(image, datetime, cv::Point(10, 75), font, 0.5, color, 1, cv::LINE_AA);
        cv::cvtColor(image, image2, cv::COLOR_BGR2BGRA);
        set_framebuffer(&ofs, &image2, image.size(), fb_info);
        sleep(1);
    }

    return 0;
}

