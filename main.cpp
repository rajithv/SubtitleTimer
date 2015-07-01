#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv ) {

    if(argc == 0) return 0;

    char * filename = argv[1];

    VideoCapture capture (filename);

    double fps = capture.get(CV_CAP_PROP_FPS);

    namedWindow(filename ,CV_WINDOW_AUTOSIZE);

    while(true){
        Mat frame;

        bool bSuccess = capture.read(frame); // read a new frame from video

        int wid = frame.cols;
        int hei = frame.rows;

        Rect ROI (0, hei * 7 / 10, wid, hei * 3 / 10 );

        Mat croppedFrame = frame(ROI);

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            break;
        }

        imshow("MyVideo", croppedFrame); //show the frame in "MyVideo" window

        if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}