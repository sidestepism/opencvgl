#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define FLAG 1
#define IN_VIDEO_FILE "sample_video_input.avi"
#define OUT_VIDEO_FILE "sample_video_output.avi"


char preset_file[] = "/Users/ryohei/gitrepos/cvgl/fruits.jpg";
void my_image_processing(cv::Mat &input, cv::Mat &processed);

int main(int argc, char* argv[])
{
    cv::VideoCapture cap;
    std::string input_index;
    if(argc >= 2){
        input_index = argv[1];
        cap.open(input_index);
    }else{
        cap.open(1);
    }

    cv::Mat frame, copy_frame;
    int rec_mode = 0;
    cv::namedWindow("video", 1);
    cv::VideoWriter output_video;
    output_video.open(OUT_VIDEO_FILE, CV_FOURCC('M', 'J', 'P', 'G'), 30, cv::Size(640, 480));

    /** using "MJPG" as the video codec */
    if(cap.isOpened() && output_video.isOpened()){
        bool loop_flag = true;
        while(loop_flag){
            cap >> frame;
            if(frame.empty()){
                break;
            }
            if(rec_mode){
                output_video << frame;
                frame.copyTo(copy_frame);
                cv::Size s = frame.size();
                cv::rectangle(copy_frame, cv::Point(0, 0), cv::Point(s.width-1, s.height-1), cv::Scalar(0, 0, 255), 4, 8, 0);
                cv::imshow("video", frame);


            }else{
                cv::imshow("video", frame);
            }
            int k = cvWaitKey(3);
            switch(k){
                    case 'q':
                    case 'Q':
                    loop_flag = false;
                    break;
                    case 'r':
                    rec_mode = rec_mode ? 0 : 1;
                    break;
            }
        }
    }else{
        fprintf(stderr, "no input video\n");
    }
    return 0;
}

void my_image_processing(cv::Mat &input, cv::Mat &processed){
#if FLAG
    cv::Mat temp;
    std::vector<cv::Mat> planes;
//    cv:cvtColor(input, temp, CV_BGR2YCrCb);
//    cv::GaussianBlur(input, processed, cv::Size(1, 37), 0, 0);
    cv::Sobel(input, processed, -1, 1, 1, 5);
    //    cv::split(temp, planes);
//    processed = planes[0];
#else
    cv::Size s = input.size();
    processed.create(s, CV_8UC1);

    for (int j=0; j<s.height; j++) {
        uchar *ptr1, *ptr2;
        ptr1 = input.ptr<uchar>(j);
        ptr2 = processed.ptr<uchar>(j);
        for(int i = 0; i < s.width; i++){
            double y = 0.114 * (double)ptr1[0] + 0.587 * (double)ptr1[1] + 0.299 * (double)ptr1[2];
            if(y > 255) y = 255;
            if(y < 0) y = 0;
            *ptr2 = (uchar) y;
            ptr1 += 3;
            ptr2 ++;
        }
    }
#endif
}
