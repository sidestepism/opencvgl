#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>

#define WINSIZE 31
#define USE_FUSHIMI 1

using namespace cv;
using namespace std;
int calcDistance(cv::Mat prev, cv::Mat next, int deltaX, int deltaY);
void calcOpticalFlow(cv::Mat prev, cv::Mat next, cv::Mat flow);
void calcPartialOpticalFlow(cv::Mat prev, cv::Mat next, cv::Mat flow, int iteration);
//cv::Point2f calcMinimize(cv::Mat prev, cv::Mat next, int size, int* minDist);
cv::Point2f calcMinimize(cv::Mat prev, cv::Mat next, cv::Point2f ref, int size, int* minDist, int* avgDist);
cv::Mat m1, m2, g1, g2, flow;
cv::Mat mov[61];

void onMouse( int event, int x, int y, int flag, void* );

void drawFlow(cv::Mat prev, cv::Mat flow){
    int x, y;
    int width = prev.cols;
    int height = prev.rows;

    for(x = 10; x < width; x += 10){
        for(y = 10; y < height; y += 10){

            int fx = -flow.at<cv::Point2f>(y, x).x + x;
            int fy = -flow.at<cv::Point2f>(y, x).y + y;

//            if(x + fx < 0) fx = 0 - x;
//            if(x + fx > width) fx = width - x;
//            if(y + fx < 0) fx = 0 - y;
//            if(y + fy > height) fy = height - y;

            cv::line(prev, cv::Point(x, y), cv::Point(fx, fy), cv::Scalar(0, 0, 0));
//            printf("(%d, %d) -> (%f, %f)\n", x, y, flow.at<cv::Point2f>(x, y).x, flow.at<cv::Point2f>(x, y).y);
        }
    }

}

void drawFlowColorMap(cv::Mat prev, cv::Mat flow){
    int x, y;
    int width = prev.cols;
    int height = prev.rows;

    for(x = 0; x < width; x += 1){
        for(y = 0; y < height; y += 1){

            float dx = flow.at<cv::Point2f>(y, x).x;
            float dy = flow.at<cv::Point2f>(y, x).y;

            int sat = sqrt(dx * dx + dy * dy) * 40;
            int hue = (sat == 0) ? 0 : (double)atan2(dx, dy) / 2.0 / M_PI * 180 + 90;
            int bri = 255;
            cv::rectangle(prev, cv::Point(x, y), cv::Point(x+1, y+1), cv::Scalar(hue, sat, bri));

//            printf("(%d, %d, %d)", hue, sat, bri);

            //            printf("(%d, %d) -> (%f, %f)\n", x, y, flow.at<cv::Point2f>(x, y).x, flow.at<cv::Point2f>(x, y).y);
        }
    }
    
}


void calcOpticalFlow(cv::Mat prev, cv::Mat next, cv::Mat flow){
    assert(prev.cols == next.cols);
    calcPartialOpticalFlow(prev, next, flow, 7);

    int width = prev.cols;
    int height = prev.rows;
    for(int x = 0; x < height; x += 1){
        for(int y = 0; y < width; y += 1){
            flow.at<cv::Point2f>(x, y) = flow.at<cv::Point2f>(x, y);
        }
    }

    cv::GaussianBlur(flow, flow, Size(WINSIZE, WINSIZE), WINSIZE, WINSIZE);
//    cv::blur(flow, flow, Size(10, 10));
}

void calcPartialOpticalFlow(cv::Mat prev, cv::Mat next, cv::Mat flow, int iteration)
{
    assert(prev.cols == next.cols);
    assert(prev.rows == next.rows);

    if(iteration == 0) return;
    if(prev.cols < 6 || next.cols < 6){
        return;
    }

    int minDist = INT_MAX;
    int avgDist = 0;
    int width = prev.cols;
    int height = prev.rows;
    int size = min(20, width/5);

    cv::Point2f delta = calcMinimize(prev, next, flow.at<cv::Point2f>(width/2, height/2), size, &minDist, &avgDist);

    if(avgDist < minDist * 1.4){
        // コントラスト低いので 0, 0 に
        delta = cv::Point2f(0, 0);
    }

    for(int x = 0; x < height; x += 1){
        for(int y = 0; y < width; y += 1){
            flow.at<cv::Point2f>(x, y) = delta;
        }
    }

    if(minDist > 1500){
        Rect r1 = Rect(0, 0,              width/2, height/2);
        Rect r2 = Rect(0, height/2,       width/2, height - height/2);
        Rect r3 = Rect(width/2, 0,        width - width/2, height/2);
        Rect r4 = Rect(width/2, height/2, width - width/2, height - height/2);
        calcPartialOpticalFlow(prev(r1), next(r1), flow(r1), iteration - 1);
        calcPartialOpticalFlow(prev(r2), next(r2), flow(r2), iteration - 1);
        calcPartialOpticalFlow(prev(r3), next(r3), flow(r3), iteration - 1);
        calcPartialOpticalFlow(prev(r4), next(r4), flow(r4), iteration - 1);
    }else{
        printf("delta: (%f, %f), size: %d minDist: %d avgDist: %d\n", delta.x, delta.y, size,  minDist, avgDist);
    }

    cv::rectangle(prev, Point2i(0, 0), Point2i(width-1, height-1), Scalar(255, 255, 255));
    cv::rectangle(next, Point2i(0, 0), Point2i(width-1, height-1), Scalar(255, 255, 255));

    return;
}

cv::Point2f calcMinimize(cv::Mat prev, cv::Mat next, cv::Point2f ref, int size, int* minDist, int* avgDist){
    cv::Point2f minPoint = cv::Point2f(ref.x, ref.y);
    int sumDist = 0;
    int count = 0;
    for(double x = -size; x <= size; x+= 1){
        for(double y = -size; y <= size; y+= 1){
            if(ref.x + x < 0 || ref.y + y < 0 || ref.x + x > prev.cols || ref.y + y > prev.rows) continue;
            int dist = calcDistance(prev, next, ref.x + x, ref.y + y);
            if(dist == INT_MAX) continue;
            if(dist < *minDist){
                minPoint.x = ref.x + x;
                minPoint.y = ref.y + y;
                *minDist = dist;
            }
            sumDist += dist;
            count ++;
        }
    }
    *avgDist = count != 0 ? sumDist / count : 0;
    if(count < 10) return ref;
    return minPoint;
}

int calcDistance(cv::Mat prev, cv::Mat next, int deltaX, int deltaY){
    int w = prev.cols - abs(deltaX);
    int h = prev.rows - abs(deltaY);

//    if(w < 2 || h < 2){
//        return INT_MAX;
//    }

    Mat imgA = prev(Rect(deltaX > 0 ? 0 : -deltaX, deltaY > 0 ? 0 : -deltaY, w, h));
    Mat imgB = next(Rect(deltaX > 0 ? deltaX : 0 , deltaY > 0 ? deltaY : 0 , w, h));
    Mat imgC;
    cv::absdiff(imgA, imgB, imgC);

    Scalar distxy = cv::mean(imgC);

    return (int)(distxy.val[0] * 1024);
}

int main( int argc, char** argv )
{

//    m1 = cv::imread("/Users/ryohei/gitrepos/cvgl/mid/eval-data/Teddy/frame10.png");
//    m2 = cv::imread("/Users/ryohei/gitrepos/cvgl/mid/eval-data/Teddy/frame11.png");
    m1 = cv::imread("/Users/ryohei/gitrepos/cvgl/stereo/pa1.jpg");
    m2 = cv::imread("/Users/ryohei/gitrepos/cvgl/stereo/pa2.jpg");


    if(m1.empty() || m2.empty()){
        return 0;
    }

    cv::cvtColor(m1, g1, CV_RGB2GRAY);
    cv::cvtColor(m2, g2, CV_RGB2GRAY);

//    cv::equalizeHist(g1, g1);
//    cv::equalizeHist(g2, g2);

//    cv::Canny(g1, g1, 200, 60);
//    cv::Canny(g2, g2, 200, 60);

    cv::GaussianBlur(g1, g1, Size(3, 3), 8, 6);
    cv::GaussianBlur(g2, g2, Size(3, 3), 8, 6);

//    cv::Laplacian(g1, g1, 8, 3);
//    cv::Laplacian(g2, g2, 8, 3);

//    cv::imshow("m1", m1);
//    cv::imshow("m2", m2);

    // http://opencv.jp/opencv-2svn/cpp/video_motion_analysis_and_object_tracking.html?highlight=optical#calcOpticalFlowFarneback
    flow = cv::Mat(g1.size(), CV_32FC2);


    if(USE_FUSHIMI){
        calcOpticalFlow(g1, g2, flow);
    }else{
        cv::calcOpticalFlowFarneback(
                                     g1,
                                     g2,
                                     flow,
                                     0.6,  // pyrScale
                                     15,    // levels
                                     50,   // winsize
                                     15,    // iterations
                                     7,    // polyN
                                     1.5,  // polySigma
                                     OPTFLOW_FARNEBACK_GAUSSIAN);

    }

    cv::Mat colorMap = m1.clone();

    drawFlowColorMap(colorMap, flow);
    cv::cvtColor(colorMap, colorMap, CV_HSV2RGB);
    drawFlow(colorMap, flow);

    cv::imshow("Optical Flow", colorMap);

    cv::imshow("g1", g1);
//    cv::imshow("g2", g2);

    Mat map(flow.size(), CV_32FC2);

    for(int k = 0; k < 61; ++k){
        double q = (double)(k - 30) / 30; // とりあえず
        for (int y = 0; y < map.rows; ++y)
        {
            for (int x = 0; x < map.cols; ++x)
            {
                Point2f f = flow.at<Point2f>(y, x);
                map.at<Point2f>(y, x) = Point2f(x + f.x * q, y + f.y * q);
            }
        }
        cv::remap(m2, mov[k], map, cv::Mat(), CV_INTER_LINEAR);
    }
    cv::Mat kasane;

    cv::namedWindow("Flow", 0 );
    cv::setMouseCallback("Flow", onMouse, 0);

    cv::addWeighted(m1, 0.5, m2, 0.5, 1, kasane);
    cv::imshow("kasane", kasane);

    cv::imshow("Flow", mov[0]);
    cv::waitKey(3600 * 1000);
//    while(1){
//        for(int k = 0; k < 61; ++k){
//            cv::imshow("Flow", mov[k]);
//            cv::waitKey(50);
//        }
//        cv::imshow("Flow", m1);
//        cv::waitKey(500);
//        cv::imshow("Flow", m2);
//        cv::waitKey(500);
//    }
    return 0;
}



void onMouse( int event, int x, int y, int flag, void* )
{
    std::string desc;

    if(event == EVENT_MOUSEMOVE){
        cv::imshow("Flow", mov[x * 60 / m1.cols]);
    }

    // マウスイベントを取得
    switch(event) {
        case cv::EVENT_MOUSEMOVE:
            desc += "MOUSE_MOVE";
            break;
        case cv::EVENT_LBUTTONDOWN:
            desc += "LBUTTON_DOWN";
            break;
        case cv::EVENT_RBUTTONDOWN:
            desc += "RBUTTON_DOWN";
            break;
        case cv::EVENT_MBUTTONDOWN:
            desc += "MBUTTON_DOWN";
            break;
        case cv::EVENT_LBUTTONUP:
            desc += "LBUTTON_UP";
            break;
        case cv::EVENT_RBUTTONUP:
            desc += "RBUTTON_UP";
            break;
        case cv::EVENT_MBUTTONUP:
            desc += "MBUTTON_UP";
            break;
        case cv::EVENT_LBUTTONDBLCLK:
            desc += "LBUTTON_DBLCLK";
            break;
        case cv::EVENT_RBUTTONDBLCLK:
            desc += "RBUTTON_DBLCLK";
            break;
        case cv::EVENT_MBUTTONDBLCLK:
            desc += "MBUTTON_DBLCLK";
            break;
    }

    // マウスボタン，及び修飾キーを取得
    if(flag & cv::EVENT_FLAG_LBUTTON)
        desc += " + LBUTTON";
    if(flag & cv::EVENT_FLAG_RBUTTON)
        desc += " + RBUTTON";
    if(flag & cv::EVENT_FLAG_MBUTTON)
        desc += " + MBUTTON";
    if(flag & cv::EVENT_FLAG_CTRLKEY)
        desc += " + CTRL";
    if(flag & cv::EVENT_FLAG_SHIFTKEY)
        desc += " + SHIFT";
    if(flag & cv::EVENT_FLAG_ALTKEY)
        desc += " + ALT";
    
//r    std::cout << desc << " (" << x << ", " << y << ")" << std::endl;
}
