/** FaceDetection.cpp **/
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

/** Print error message **/
void PANIC(char *msg);
#define PANIC(msg){perror(msg); exit(-1);}

/** Function for face detection **/
void DetectAndDraw(Mat frame);

/** Global variables **/
String face_cascade_name = "Source/haarcascade_frontalface_default.xml";
String eyes_cascade_name = "Source/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade; // Declare the face classifier
CascadeClassifier eyes_cascade; // Declare the eyes classifier
String window_name = "Face detection";


class Timer
{
public:
    Timer() { reset(); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec + (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

    void show_time(string str) {
        cout << elapsed() << " - " << str << endl;
        reset();
    }

private:
    timespec beg_, end_;
};


Timer tmr;


int main(int argc, char *argv[]) {
    /* Open the web camera */
    VideoCapture capture = VideoCapture(2);
    Mat frame, image;

    /** Load cascade classifiers **/
    if (!face_cascade.load(face_cascade_name))
        PANIC("Error loading face cascade");
    if (!eyes_cascade.load(eyes_cascade_name))
        PANIC("Error loading eyes cascade");

    /** After the camera is opened **/
    if (capture.isOpened()) {
        cout << "Face Detection Started..." << endl;

        // set propety of the frame
#if 0
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 480);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
#endif
        cout << capture.get(cv::CAP_PROP_FRAME_WIDTH) << "x";
        cout << capture.get(cv::CAP_PROP_FRAME_HEIGHT) << endl;

        for (;;) {
            /* Get image from camera */
            capture >> frame;
            if (frame.empty())
                PANIC("Error capture frame");

            /* Start the face detection function */
            DetectAndDraw(frame);

            /** If you press ESC, q, or Q , the process will end **/
            char ch = (char)waitKey(10);
            if (ch == 27 || ch == 'q' || ch == 'Q')
                break;
        }
    } else
        PANIC("Error open camera");

    return 0;
}

void DetectAndDraw(Mat frame) {
    /* Declare vector for faces and eyes */
    std::vector<Rect> faces, eyes;
    Mat frame_gray, frame_resize;
    int radius;

    tmr.show_time("==================");

    /* Convert to gray scale */
    cvtColor(frame, frame_resize, COLOR_BGR2GRAY);
    //imshow("grayscale", frame_resize);
    tmr.show_time("cvtColor");

    /* Resize the grayscale Image */
    resize(frame_resize, frame_gray, Size(320, 240), 1, 1, INTER_LINEAR);
    //imshow("resize", frame_resize);
    tmr.show_time("resize");

    /* Histogram equalization */
    equalizeHist(frame_gray, frame_gray);
    //imshow("equalize", frame_gray);
    tmr.show_time("equalizeHist");

    /* Detect faces of different sizes using cascade classifier */
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 5, CV_HAAR_SCALE_IMAGE, Size(30, 30));
    tmr.show_time("detectMultiScale");

    /** Draw circles around the faces **/
    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center;

        /* Draw rectangular on face */
        Rect head = faces[i] + Point(faces[i].x, faces[i].y) + Size(faces[i].width, faces[i].height);
        rectangle(frame, head, Scalar(255, 0, 0), 3, 8, 0);

        Mat faceROI = frame_gray(faces[i]);

        /* Detection of eyes int the input image */
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 1, CV_HAAR_SCALE_IMAGE, Size(3, 3));
        tmr.show_time("Detect");

        /** Draw circles around eyes **/
        for (size_t j = 0; j < eyes.size(); j++)
        {
            center.x = cvRound((faces[i].x + eyes[j].x + eyes[j].width * 0.5) * 2);
            center.y = cvRound((faces[i].y + eyes[j].y + eyes[j].height * 0.5) * 2);
            radius = cvRound((eyes[j].width + eyes[j].height) * 0.25 * 2);
            circle(frame, center, radius, Scalar(0, 255, 0), 3, 8, 0);
        }
        tmr.show_time("Draw");
    }

    // Show Processed Image with detected faces
    imshow( "Face Detection", frame);
    tmr.show_time("imshow");
}
