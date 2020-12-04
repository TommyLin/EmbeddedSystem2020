#include <fstream>
#include <iostream>
#include <thread>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "getch.hpp"
#include "getfbi.hpp"

using namespace std;


bool running = true;
bool saving = false;
int counter = 0;
vector<int> compression_params;
char buff[64];


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


int main(int argc, const char *argv[])
{
    cv::Mat frame, bgr565;
    cv::Size2f frame_size;
    int w, h;
    int index = 1;
    thread m_thread, checkInput;

    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(9);

    cv::VideoCapture camera ( 2 );

    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    ofstream ofs("/dev/fb0");

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

        frame_size = frame.size();

        cv::cvtColor(frame, bgr565, cv::COLOR_BGR2BGR565);

        for ( int y = 0; y < frame_size.height; y++ )
        {
            ofs.seekp((y * fb_info.xres_virtual + (fb_info.xres_virtual - w) / 2)
                    * (fb_info.bits_per_pixel / 8));
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

