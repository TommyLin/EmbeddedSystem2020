#include <iostream>

#include "getfbi.hpp"

using namespace std;


int main(int argc, const char *argv[])
{
    framebuffer_info fb_info;

    for (int i = 0; i < 3; i++) {
        string dev_name = "/dev/fb" + to_string(i);
        fb_info = get_framebuffer_info(dev_name.c_str());
        cout << "[" << fb_info.xres_virtual << "x" << fb_info.yres_virtual << "] "
            << fb_info.bits_per_pixel << endl;
    }

    return 0;
}

