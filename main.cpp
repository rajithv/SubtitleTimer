#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>

using namespace std;
using namespace cv;

ofstream fout1("out_ocr_1.txt");
ofstream fout2("out_ocr_2.txt");

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
    fout2 << out2;
    fout2.flush();

    tesseract::TessBaseAPI tess_de;
    tess_de.Init(NULL, "deu", tesseract::OEM_DEFAULT);
    tess_de.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    tess_de.SetImage((uchar*)image_lang1.data, image_lang1.cols, image_lang1.rows, 1, image_lang1.cols);

    // Get the text
    char* out1 = tess_de.GetUTF8Text();
    fout1 << out1;
    fout1.flush();

}

int main(int argc, char **argv) {

    if (argc == 0) return 0;

    char *filename = argv[1];

    VideoCapture capture("https://r1---sn-nau-jhce.googlevideo.com/videoplayback?mt=1436106367&mv=m&pl=19&ms=au&signature=6638A791EC39D92F0B78C1729E262D94F6E4500D.1290B7007847392130D3C6B66AD7ADA50AD37CBD&fexp=901816,936110,9407141,9408142,9408420,9408710,9416126,952640&key=yt5&mm=31&mn=sn-nau-jhce&sver=3&initcwndbps=612500&ipbits=0&ratebypass=yes&expire=1436128043&mime=video/mp4&upn=r96TlOG5XU4&id=o-AByEs0xcovZ2o57eBjK_s1miC6d0rXM7n0TYgTTu-QZA&sparams=dur,id,initcwndbps,ip,ipbits,itag,lmt,mime,mm,mn,ms,mv,pl,ratebypass,requiressl,source,upn,expire&source=youtube&dur=486.016&ip=124.43.104.231&itag=22&lmt=1435531519368512&requiressl=yes");

    double fps = capture.get(CV_CAP_PROP_FPS);

    namedWindow(filename, CV_WINDOW_AUTOSIZE);

    vector<int> startWhitePosPrev;
    vector<int> endWhitePosPrev;
    int f = 0;

    bool skipped_frame = false;
    bool first_frame = true;

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