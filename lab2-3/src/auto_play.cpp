#include <fcntl.h>
#include <fstream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <sys/ioctl.h>

struct framebuffer_info
{
    uint32_t bits_per_pixel;
    uint32_t xres_virtual;
};

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;
    struct fb_var_screeninfo screen_info;

    int fd = open(framebuffer_device_path, O_RDWR);

    ioctl(fd, FBIOGET_VSCREENINFO, &screen_info);

    fb_info.xres_virtual = screen_info.xres_virtual;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;

    return fb_info;
};

void set_framebuffer(std::ofstream *ofs, cv::Mat *bgr565, cv::Size2f image_size,
        framebuffer_info fb_info)
{
    for (int y = 0; y < image_size.height; y++) {
        ofs->seekp(y * fb_info.xres_virtual * (fb_info.bits_per_pixel / 8));
        ofs->write(reinterpret_cast<char*>(bgr565->ptr(y)),
                image_size.width * (fb_info.bits_per_pixel / 8));
    }

}

int main(int argc, const char *argv[])
{
    cv::Mat image, bgr565;
    cv::Size2f image_size;
    const char *dev = "/dev/fb0";

    framebuffer_info fb_info = get_framebuffer_info(dev);
    std::ofstream ofs(dev);

    while (1) {
        image = cv::imread("sample.bmp");

        image_size = image.size();

        cv::cvtColor(image, bgr565, cv::COLOR_BGR2BGR565);

        set_framebuffer(&ofs, &bgr565, image_size, fb_info);
    }

    return 0;
}

