#include <fcntl.h>
#include <fstream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <vector>

struct framebuffer_info
{
    uint32_t bits_per_pixel;
    uint32_t xres_virtual;
    uint32_t yres_virtual;
};

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;
    struct fb_var_screeninfo screen_info;

    int fd = open(framebuffer_device_path, O_RDWR);

    ioctl(fd, FBIOGET_VSCREENINFO, &screen_info);

    fb_info.xres_virtual = screen_info.xres_virtual;
    fb_info.yres_virtual = screen_info.yres_virtual;
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

int scan_dir(const char* path, std::vector<std::string> *files)
{
    DIR *dir;
    struct dirent *ent;
    char *ext;

    if ((dir = opendir(path)) == NULL) {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }

    /* print all the files and directories within directory */
    while ((ent = readdir(dir)) != NULL) {
        ext = strrchr(ent->d_name, '.');
        if (strcmp(ext, ".jpg") == 0)
            files->push_back(ent->d_name);
    }
    closedir(dir);

    return 0;
}

int main(int argc, const char *argv[])
{
    cv::Mat image, bgr565;
    cv::Size2f image_size;
    const char *dev = "/dev/fb0";
    std::vector<std::string> files;
    std::string wallpaper_dir = "/root/wallpapers/";
    framebuffer_info fb_info = get_framebuffer_info(dev);
    std::ofstream ofs(dev);

    scan_dir(wallpaper_dir.c_str(), &files);

    while (1) {
        for (unsigned int i = 0; i < files.size(); i++) {
            fb_info = get_framebuffer_info(dev);
            image = cv::imread(wallpaper_dir + files[i]);
            std::cout << files[i] << " " << image.size() << " => ";
            cv::resize(image, image, cv::Size(fb_info.xres_virtual, fb_info.yres_virtual), 0, 0, cv::INTER_LINEAR);
            image_size = image.size();
            cv::cvtColor(image, bgr565, cv::COLOR_BGR2BGR565);
            set_framebuffer(&ofs, &bgr565, image_size, fb_info);
            std::cout << "[" << fb_info.xres_virtual << "x" << fb_info.yres_virtual << "]" << std::endl;
            sleep(2);
        }
    }

    return 0;
}

