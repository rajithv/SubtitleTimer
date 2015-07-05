#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>

using namespace std;
using namespace cv;

ofstream fout1;
ofstream fout2;
ofstream ftime;

void sendToOCR(Mat image){
    int height = image.rows;
    int width = image.cols;

    Rect ROI_lang1(0, 0, width, height*0.6);
    Rect ROI_lang2(0, height*0.6, width, height*0.4);

    Mat image_lang1 = image (ROI_lang1);
    Mat image_lang2 = image (ROI_lang2);

    imshow("Lang1", image_lang1);
    imshow("Lang2", image_lang2);

    tesseract::TessBaseAPI tess_en;
    tess_en.Init(NULL, "eng+deu", tesseract::OEM_DEFAULT);
    tess_en.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    tess_en.SetImage((uchar*)image_lang2.data, image_lang2.cols, image_lang2.rows, 1, image_lang2.cols);

    // Get the text
    char* out2 = tess_en.GetUTF8Text();
    string str2(out2);
    str2.erase(str2.end()-1, str2.end());
    fout2 << str2;
    fout2.flush();

    tesseract::TessBaseAPI tess_de;
    tess_de.Init(NULL, "deu", tesseract::OEM_DEFAULT);
    tess_de.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    tess_de.SetImage((uchar*)image_lang1.data, image_lang1.cols, image_lang1.rows, 1, image_lang1.cols);

    // Get the text
    char* out1 = tess_de.GetUTF8Text();
    string str1(out1);
    str1.erase(str1.end()-1, str1.end());
    fout1 << str1;
    fout1.flush();

}

int main(int argc, char **argv) {

    if (argc < 5) {
        cerr << "Usage : ./SubtitleTimer.o <Video File> <Time File> <Sub Main Language> <Sub Secondary Language>" << endl;
        return 0;
    }

    ftime.open(argv[2]);
    fout1.open(argv[3]);
    fout2.open(argv[4]);

    char *filename = argv[1];

    VideoCapture capture(filename);

    double fps = capture.get(CV_CAP_PROP_FPS);

    namedWindow(filename, CV_WINDOW_AUTOSIZE);

    vector<int> startWhitePosPrev;
    vector<int> endWhitePosPrev;
    int f = 0;

    bool skipped_frame = false;
    bool first_frame = true;

    while (capture.isOpened()) {
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

        double black = croppedFrame.cols;

        for (int i = 0; i < croppedFrame.cols; i++) {
            if (croppedFrame.at<uchar>((int) croppedFrame.rows * 0.6, i) >= 250) {
                black--;
            }
        }

        if (black / (double) croppedFrame.cols == 1) {

            if(first_frame){
                first_frame = false;
                cout << f / fps << endl;
                sendToOCR(croppedFrame);
                skipped_frame = false;
            }

            for (int k = 1; k <= 3; k += 2) {
                for (int i = (croppedFrame.rows * k / 4 - tol / 2); i < croppedFrame.rows * k / 4 + tol / 2; i++) {
                    //cout << i << " ";
                    for (int j = 0; j < croppedFrame.cols; j++) {
                        if (croppedFrame.at<uchar>(i, j) >= 250) {
                            startWhitePosCurr.push_back(j);
                            break;
                        }
                    }
                    for (int j = croppedFrame.cols; j > 0; j--) {
                        if (croppedFrame.at<uchar>(i, j) >= 250) {
                            endWhitePosCurr.push_back(j);
                            break;
                        }
                    }
                }
                //cout << endl;
            }
            if (f == 0) {
                cout << 0 << endl;
                startWhitePosPrev = startWhitePosCurr;
                endWhitePosPrev = endWhitePosCurr;
            }
            else {
                int changes = 0;
                for (int i = 0; i < startWhitePosCurr.size(); i++) {
                    if (startWhitePosCurr.size() != startWhitePosPrev.size() ||
                        endWhitePosCurr.size() != endWhitePosPrev.size()) {
                        changes++;
                    } else {
                        if (((startWhitePosCurr.at(i) - startWhitePosPrev.at(i)) *
                             (startWhitePosCurr.at(i) - startWhitePosPrev.at(i)) > 4) ||
                            ((endWhitePosCurr.at(i) - endWhitePosPrev.at(i)) *
                             (endWhitePosCurr.at(i) - endWhitePosPrev.at(i)) > 4)) {
                            changes++;
                        }
                    }
                }
                if (changes > 2 * tol * 0.75) {
                    cout << f / fps << endl;
                    sendToOCR(croppedFrame);
                    skipped_frame = false;
                }
            }
        }else{
            if(!skipped_frame) {
                //sendToOCR(croppedFrame);
                cout << f / fps << endl;
                cout << "-" << endl;
                skipped_frame = true;
            }
            cerr << "skipped frame " << f << endl;
        }
        startWhitePosPrev = startWhitePosCurr;
        endWhitePosPrev = endWhitePosCurr;

        line(croppedFrame, Point( 10 , croppedFrame.rows * 0.6), Point( croppedFrame.cols - 10 , croppedFrame.rows * 0.6), Scalar( 255, 255, 255 ), 2, 8);

        imshow("MyVideo", croppedFrame); //show the frame in "MyVideo" window

        if (waitKey(1) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cerr << "esc key is pressed by user" << endl;
            break;
        }
        f++;
    }

    return 0;
}