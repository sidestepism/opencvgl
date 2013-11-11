#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>

int size_of_mosaic = 0;

void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
                  cv::Mat &output, cv::Point2i location);

int main(int argc, char *argv[]){
    // load classifier
    std::string cascadeName = "/opt/local/share/OpenCV/haarcascades/haarcascade_frontalface_default.xml";
    cv::CascadeClassifier cascade;
    if(!cascade.load(cascadeName)){
        return -1;
    }

    cv::namedWindow("result", 1);
    cv::namedWindow("smallImg", 1);
    cv::namedWindow("mustache", 1);
    cv::createTrackbar("size", "result", &size_of_mosaic, 30, 0);
    cv::VideoCapture cap;
    cv::Mat frame, mosaic;
    cap.open(0);
    cap >> frame;
    double scale = 4.0;
    cv::Mat mustache, gray, smallImg(cv::saturate_cast<int>(frame.rows/scale), cv::saturate_cast<int>(frame.cols/scale), CV_8UC1);

    mustache = cvLoadImage("/Users/ryohei/gitrepos/cvgl/mustache.png", -1);

    for(;;){
        cap >> frame;
        cv::cvtColor(frame, gray, CV_BGR2GRAY);
        // scale-down th eimage
        cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);

        cv::equalizeHist(smallImg, smallImg);

        std::vector<cv::Rect> faces;
        // multi-scale face searching
        // image size scale num flag smallest rect
        cascade.detectMultiScale(smallImg, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE, cv::Size(30,30));
        // Show result

//        std::vector<cv::Rect>::const_iterator r = faces.begin();
//        for(; r != faces.end(); ++r){

        int i;
        for(i = 0; i < faces.size(); i++){
            cv::Point center;
            int radius;
            center.x = cv::saturate_cast<int>((faces[i].x + faces[i].width * 0.5) * scale);
            center.y = cv::saturate_cast<int>((faces[i].y + faces[i].height * 0.5) * scale);
            radius = cv::saturate_cast<int>((faces[i].width + faces[i].height) * 0.25 * scale);
            if(size_of_mosaic < 1) size_of_mosaic = 1;
            cv::Rect roi_rect(center.x-radius, center.y-radius, radius*2, radius * 2);
            cv::Mat mosaic = frame(roi_rect);
            cv::Mat tmp = mosaic.clone();

            cv::resize(mustache, tmp, tmp.size(), 0, 0, cv::INTER_LINEAR);
            overlayImage(mosaic, tmp, mosaic, cv::Point2i(0, 0));
        }

        cv::imshow("mustache", mustache);
        cv::imshow("smallImg", smallImg);
        cv::imshow("result", frame);
        int key = cv::waitKey(10);
        if(key == 'q' || key == 'Q'){
            break;
        }
    }
    return 0;
}

// http://jepsonsblog.blogspot.jp/2012/10/overlay-transparent-image-in-opencv.html
void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
                  cv::Mat &output, cv::Point2i location)
{
    background.copyTo(output);


    // start at the row indicated by location, or at row 0 if location.y is negative.
    for(int y = std::max(location.y , 0); y < background.rows; ++y)
    {
        int fY = y - location.y; // because of the translation

        // we are done of we have processed all rows of the foreground image.
        if(fY >= foreground.rows)
            break;

        // start at the column indicated by location,

        // or at column 0 if location.x is negative.
        for(int x = std::max(location.x, 0); x < background.cols; ++x)
        {
            int fX = x - location.x; // because of the translation.

            // we are done with this row if the column is outside of the foreground image.
            if(fX >= foreground.cols)
                break;

            // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
            double opacity =
            ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3]) / 255.;


            // and now combine the background and foreground pixel, using the opacity,

            // but only if opacity > 0.
            for(int c = 0; opacity > 0 && c < output.channels(); ++c)
            {
                unsigned char foregroundPx =
                foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                unsigned char backgroundPx =
                background.data[y * background.step + x * background.channels() + c];
                output.data[y*output.step + output.channels()*x + c] =
                backgroundPx * (1.-opacity) + foregroundPx * opacity;
            }
        }
    }
}