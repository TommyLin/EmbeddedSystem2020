#include <opencv2/imgcodecs.hpp>

#include "getfbi.hpp"

using namespace std;


int main(int argc, const char *argv[])
{
    const char *dev = "/dev/fb1";
    framebuffer_info fb_info = get_framebuffer_info(dev);
    ofstream ofs(dev);
    string text = "Group 19";
    cv::Mat image2, image;// = cv::Mat::zeros(cv::Size(fb_info.xres_virtual, fb_info.yres_virtual), CV_64FC1);

    image = cv::imread("/root/wallpapers/1024x600_000.jpg");
    cv::resize(image, image, cv::Size(fb_info.xres_virtual, 180), 0, 0, cv::INTER_LINEAR);

    cv::putText(image, text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 25), 1, cv::LINE_AA);
    cv::cvtColor(image, image2, cv::COLOR_BGR2BGRA);
    set_framebuffer(&ofs, &image2, image.size(), fb_info);

    return 0;
}

