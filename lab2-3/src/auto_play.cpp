#include <fcntl.h>
#include <fstream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <sys/ioctl.h>

struct framebuffer_info
{
    uint32_t bits_per_pixel;    // framebuffer depth
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
};

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path);

int main(int argc, const char *argv[])
{
    cv::Mat image, bgr565;
    cv::Size2f image_size;

    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    std::ofstream ofs("/dev/fb0");

    image = cv::imread("sample.bmp");

    image_size = image.size();

    cv::cvtColor(image, bgr565, cv::COLOR_BGR2BGR565);

    for (int y = 0; y < image_size.height; y++)
    {
        ofs.seekp(y * fb_info.xres_virtual * (fb_info.bits_per_pixel / 8));
        ofs.write(reinterpret_cast<char*>(bgr565.ptr(y)), image_size.width * (fb_info.bits_per_pixel / 8));
    }

    return 0;
}

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;        // Used to return the required attrs.
    struct fb_var_screeninfo screen_info;   // Used to get attributes of the device from OS kernel.

    int fd = open(framebuffer_device_path, O_RDWR);

    ioctl(fd, FBIOGET_VSCREENINFO, &screen_info);

    fb_info.xres_virtual = screen_info.xres_virtual;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;

    return fb_info;
};
