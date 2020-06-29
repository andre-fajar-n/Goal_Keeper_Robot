#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

using namespace cv;
using namespace std;

#define in1 0
#define in2 1
#define en1 2
#define in3 3
#define in4 4
#define en2 5

int main(int argc, char **argv)
{
    wiringPiSetup();
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    softPwmCreate(en1, 0, 480);
    softPwmCreate(en2, 0, 480);

    namedWindow("original", WINDOW_AUTOSIZE);
    namedWindow("result", WINDOW_AUTOSIZE);
    VideoCapture cap;
    cap.open(0);
    Mat frame;
    Mat hsv;
    Mat hasil;

    for (;;)
    {
        cap >> frame;
        if (frame.empty())
            break;
        resize(frame, frame, Size(), 0.75, 0.75);
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(0, 93, 163), Scalar(40, 255, 255), hasil);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(hasil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        int blob = contours.size();
        int jml_blob = 0, n = 0, a = 0;
        for (int n = 0; n < blob; n++)
        {
            if (contours[n].size() > 75)
            {
                jml_blob++;
                a = contours[n].size();
                cout << "blob =" << jml_blob << "    ukuran blob =" << contours[n].size() << "\n"
                     << endl;
                Moments m = moments(hasil, true);
                Point p(m.m10 / m.m00, m.m01 / m.m00);
                int x = p.x;
                int y = p.y;
                cout << "titik:" << p << "kolom:" << frame.cols << endl;
                circle(frame, p, 5, Scalar(0, 0, 255), -1);
                if (p.x > frame.cols / 2)
                {
                    digitalWrite(in1, LOW);
                    digitalWrite(in2, HIGH);
                    softPwmWrite(en1, p.x - (frame.cols / 2));
                    digitalWrite(in3, HIGH);
                    digitalWrite(in4, LOW);
                    softPwmWrite(en2, p.x - (frame.cols / 2));
                    cout << "kanan" << endl;
                }
                else if (p.x < frame.cols / 2)
                {
                    digitalWrite(in1, HIGH);
                    digitalWrite(in2, LOW);
                    softPwmWrite(en1, (frame.cols / 2) - p.x);
                    digitalWrite(in3, LOW);
                    digitalWrite(in4, HIGH);
                    softPwmWrite(en2, (frame.cols / 2) - p.x);
                    cout << "kiri" << endl;
                }
            }
        }
        imshow("original", frame);
        imshow("result", hasil);
        char pencet = waitKey(10);
        if (pencet == 32)
        {
            while (pencet == 32)
            {
                if (waitKey(10) == 32)
                    break; //tombol pause
            }
        }
        else if (pencet == 27)
            return 0; //tombol exit
    }
}