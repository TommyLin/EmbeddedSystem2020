#include <fstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <unistd.h>
#include <dirent.h>
#include <iostream>

#include "getfbi.hpp"

using namespace std;


int scan_dir(const char* path, vector<string> *files)
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
    vector<string> files;
    string wallpaper_dir = "/root/wallpapers/";

    if (argc > 1)
        dev = argv[1];

    framebuffer_info fb_info = get_framebuffer_info(dev);
    ofstream ofs(dev);
    string text = "Group 19";

    scan_dir(wallpaper_dir.c_str(), &files);

    while (1) {
        for (unsigned int i = 0; i < files.size(); i++) {
            fb_info = get_framebuffer_info(dev);
            image = cv::imread(wallpaper_dir + files[i]);
            cout << files[i] << " " << image.size() << " => ";
            cv::resize(image, image, cv::Size(fb_info.xres_virtual, fb_info.yres_virtual), 0, 0, cv::INTER_LINEAR);
            image_size = image.size();
            switch (fb_info.bits_per_pixel) {
            case 16:
                cv::cvtColor(image, bgr565, cv::COLOR_BGR2BGR565);
                set_framebuffer(&ofs, &bgr565, image_size, fb_info);
                break;
            case 24:
                set_framebuffer(&ofs, &image, image_size, fb_info);
                break;
            case 32:
                break;
            default:
                return 1;
            }
            cout << "[" << fb_info.xres_virtual << "x" << fb_info.yres_virtual << "]" << endl;
            sleep(2);
        }
    }

    return 0;
}

