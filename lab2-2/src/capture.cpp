#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/ioctl.h>

using namespace std;

bool running = true;
bool saving = false;
int counter = 0;
vector<int> compression_params;
char buff[64];

struct framebuffer_info
{
    uint32_t bits_per_pixel;    // depth of framebuffer
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
    uint32_t yres_virtual;      // how many pixel in a col in virtual screen
};

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path );
#include <unistd.h>
#include <termios.h>

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

void wait_key()
{
    char c;

    while (running) {
        c = getch();
        if (c == 'c') {
            counter++;
        }
    }
}


void dump(int index, cv::Mat frame)
{
    if ((index > 0) && (index < 999)) {
        snprintf(buff, sizeof(buff), "/run/media/mmcblk1p1/file-%03d.jpg", index);
        cout << buff;
        cv::imwrite(buff, frame, compression_params);
        cout << " Complete ";
        counter--;
        if (counter)
            cout << counter << " to go!";
        cout << endl;
        saving = false;
    }
}


int main ( int argc, const char *argv[] )
{
    // variable to store the frame get from video stream
    cv::Mat frame, bgr565;
    cv::Size2f frame_size;
    int w, h;
    int index = 1;
    thread m_thread, checkInput;

    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(9);

    // open video stream device
    cv::VideoCapture camera ( 2 );

    // get info of the framebuffer
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");

    // open the framebuffer device
    ofstream ofs("/dev/fb0");

    // check if video stream device is opened success or not
    if( !camera.isOpened() ) {
        cerr << "Could not open video device." << endl;
        return -1;
    }

    // Prepare display image size
    camera.read(frame);
    m_thread = thread(dump, 0, frame);
    checkInput = thread(wait_key);
    w = fb_info.xres_virtual * frame.size().height / frame.size().width;
    h = fb_info.yres_virtual;

    // set propety of the frame
    camera.set(cv::CAP_PROP_FRAME_WIDTH, w);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, h);

    while ( true ) {
        // get video frame from stream
        camera.read(frame);

        // Write frame to file
        if (index <= 30) {
            if (counter) {
                if (!saving) {
                    saving = true;
                    m_thread.join();
                    m_thread = thread(dump, index, frame);
                    index++;
                }
            }
        } else {
            running = false;
            break;
        }

        // get size of the video frame
        frame_size = frame.size();

        // transfer color space from BGR to BGR565 (16-bit image) to fit the requirement of the LCD
        cv::cvtColor(frame, bgr565, cv::COLOR_BGR2BGR565);

        // output the video frame to framebufer row by row
        for ( int y = 0; y < frame_size.height; y++ )
        {
            // move to the next written position of output device framebuffer by "std::ostream::seekp()"
            ofs.seekp((y * fb_info.xres_virtual + (fb_info.xres_virtual - w) / 2)
                    * (fb_info.bits_per_pixel / 8));

            // write to the framebuffer by "std::ostream::write()"
            // you could use "cv::Mat::ptr()" to get the pointer of the corresponding row.
            ofs.write(reinterpret_cast<char*>(bgr565.ptr(y)), frame_size.width * (fb_info.bits_per_pixel / 8));
        }
    }

    // closing video stream
    camera.release ( );
    m_thread.join();
    running = false;
    checkInput.join();

    cout << argv[0] << " exit" << endl;

    return 0;
}

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path )
{
    struct framebuffer_info fb_info;        // Used to return the required attrs.
    struct fb_var_screeninfo screen_info;   // Used to get attributes of the device from OS kernel.

    // open deive with linux system call "open( )"
    int fd = open(framebuffer_device_path, O_RDWR);

    // get attributes of the framebuffer device thorugh linux system call "ioctl()"
    // the command you would need is "FBIOGET_VSCREENINFO"
    ioctl(fd, FBIOGET_VSCREENINFO, &screen_info);

    // put the required attributes in variable "fb_info" you found with "ioctl() and return it."
    fb_info.xres_virtual = screen_info.xres_virtual;
    fb_info.yres_virtual = screen_info.yres_virtual;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;

    return fb_info;
};
