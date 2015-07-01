#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

    if (argc == 0) return 0;

    char *filename = argv[1];

    VideoCapture capture(filename);

    double fps = capture.get(CV_CAP_PROP_FPS);

    namedWindow(filename, CV_WINDOW_AUTOSIZE);

    vector<int> startWhitePosPrev;
    vector<int> endWhitePosPrev;
    int f = 0;
    while (true) {
        Mat frame;

        bool bSuccess = capture.read(frame); // read a new frame from video

        int wid = frame.cols;
        int hei = frame.rows;

        Rect ROI(0, hei * 0.78, wid, hei * 0.12);

        Mat croppedFrame = frame(ROI);

        cvtColor(croppedFrame, croppedFrame, CV_RGB2GRAY);
        threshold(croppedFrame, croppedFrame, 100, 255, 0);


        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            break;
        }
        vector<int> startWhitePosCurr;
        vector<int> endWhitePosCurr;
        // set number of lines
        //int N = 3;
        // check thickness at each level
        int tol = 4;
        for (int k = 1; k <= 3; k+=2) {
            for (int i = (croppedFrame.rows * k / 4 - tol/2); i < croppedFrame.rows * k / 4 + tol/2; i++) {
                //cout << i << " ";
                for (int j = 0; j < croppedFrame.cols; j++) {
                    if (croppedFrame.at<uchar>(i, j) >= 250) {
                        startWhitePosCurr.push_back(j);
                        break;
                    }
                }
                for(int j = croppedFrame.cols; j > 0; j--) {
                    if (croppedFrame.at<uchar>(i, j) >= 250) {
                        endWhitePosCurr.push_back(j);
                        break;
                    }
                }
            }
            //cout << endl;
        }
        if (f == 0) {
            startWhitePosPrev = startWhitePosCurr;
            endWhitePosPrev = endWhitePosCurr;
        }
        else {
            int changes = 0;
            for (int i = 0; i < startWhitePosCurr.size(); i++) {
                if(startWhitePosCurr.size() != startWhitePosPrev.size() || endWhitePosCurr.size() != endWhitePosPrev.size()){
                    changes ++;
                }else {
                    if (((startWhitePosCurr.at(i) - startWhitePosPrev.at(i)) *
                        (startWhitePosCurr.at(i) - startWhitePosPrev.at(i)) > 4) ||
                            ((endWhitePosCurr.at(i) - endWhitePosPrev.at(i)) *
                             (endWhitePosCurr.at(i) - endWhitePosPrev.at(i)) > 4)){
                        changes++;
                    }
                }
            }
            if (changes > 2 * tol * 0.75) {
                cout << f / fps << endl;
            }
        }
        startWhitePosPrev = startWhitePosCurr;
        endWhitePosPrev = endWhitePosCurr;

        imshow("MyVideo", croppedFrame); //show the frame in "MyVideo" window

        if (waitKey(1) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
        f++;
    }

    return 0;
}