#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

static void help()
{
    // print a welcome message, and the OpenCV version
    cout << "\nThis is a demo of Lukas-Kanade optical flow lkdemo(),\n"
    "Using OpenCV version " << CV_VERSION << endl;
    cout << "\nIt uses camera by default, but you can provide a path to video as an argument.\n";
    cout << "\nHot keys: \n"
    "\tESC - quit the program\n"
    "\tr - auto-initialize tracking\n"
    "\tc - delete all the points\n"
    "\tn - switch the \"night\" mode on/off\n"
    "To add/remove a feature point click it\n" << endl;
}



void drawFlow(cv::Mat prev, cv::Mat flow){
    int x, y;
    int width = prev.cols;
    int height = prev.rows;

    for(x = 0; x < width; x += 10){
        for(y = 0; y < height; y += 10){
            cv::line(prev, cv::Point(x, y), cv::Point(flow.at<cv::Point2f>(x, y).x + x, flow.at<cv::Point2f>(x, y).y + y), cv::Scalar(0, 0, 0));
            printf("(%d, %d) -> (%f, %f)\n", x, y, flow.at<cv::Point2f>(x, y).x, flow.at<cv::Point2f>(x, y).y);
        }
    }

}

int main( int argc, char** argv )
{
    cv::Mat m1, m2, g1, g2, flow;

    m1 = cv::imread("/Users/ryohei/gitrepos/cvgl/stereo/m1.jpg");
    m2 = cv::imread("/Users/ryohei/gitrepos/cvgl/stereo/m2.jpg");


    if(m1.empty() || m2.empty()){
        return 0;
    }

    cv::cvtColor(m1, g1, CV_RGB2GRAY);
    cv::cvtColor(m2, g2, CV_RGB2GRAY);

    cv::imshow("m1", m1);
    cv::imshow("m2", m2);

    // http://opencv.jp/opencv-2svn/cpp/video_motion_analysis_and_object_tracking.html?highlight=optical#calcOpticalFlowFarneback
    cv::calcOpticalFlowFarneback(
                                 g1,
                                 g2,
                                 flow,
                                 0.7,  // pyrScale
                                 5,    // levels
                                 10,   // winsize
                                 9,    // iterations
                                 7,    // polyN
                                 1.5,  // polySigma
                                 OPTFLOW_FARNEBACK_GAUSSIAN);

//    drawFlow(g1, flow);
    cv::imshow("Optical Flow", m1);

    Mat mov[61];
    Mat map(flow.size(), CV_32FC2);

    for(int k = 0; k < 61; ++k){
        double q = (double)(k-30) / 10; // とりあえず
        for (int y = 0; y < map.rows; ++y)
        {
            for (int x = 0; x < map.cols; ++x)
            {
                Point2f f = flow.at<Point2f>(y, x);
                map.at<Point2f>(y, x) = Point2f(x + f.x * q, y + f.y * q);
            }
        }
        cv::remap(m1, mov[k], map, cv::Mat(), CV_INTER_LINEAR);
    }



    while(1){
        for(int k = 0; k < 61; ++k){
            cv::imshow("Flow", mov[k]);
            cv::waitKey(50);
        }

    }
    return 0;
}

