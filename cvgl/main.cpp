#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>

#define FLAG 1
#define IN_VIDEO_FILE "sample_video_input.avi"
#define OUT_VIDEO_FILE "sample_video_output.avi"

cv::Mat inpaint_mask;
cv::Mat img0, img, inpainted;
cv::Point prev_pt;

void on_mouse(int event, int x, int y, int flags, void *){
    if(img.empty()){
        return;
    }

    if(event == CV_EVENT_LBUTTONDOWN){
        printf("down\n");
        prev_pt = cv::Point(x, y);
    }else if(event == CV_EVENT_LBUTTONUP && (flags & CV_EVENT_FLAG_LBUTTON)){
        printf("up\n");
        cv::Point pt(x, y);
        img = img0.clone();

        cv::Rect roi(fmin(pt.x, prev_pt.x), fmin(pt.y, prev_pt.y), abs(prev_pt.x - pt.x), abs(prev_pt.y - pt.y));
        cv::Mat image_roi = img(roi);
        cv::bitwise_not(image_roi, image_roi);

//        cv::rectangle(img, pt, prev_pt, cv::Scalar::all(255));
        img0 = img.clone();
        cv::imshow("image", img);
        prev_pt = cv::Point(-1, -1);
    }else if(event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)){
        printf("move\n");
        cv::Point pt(x, y);
        img = img0.clone();

        cv::rectangle(img, pt, prev_pt, cv::Scalar::all(255));
        cv::imshow("image", img);
        //        prev_pt = cv::Point(-1, -1);
    }
}

int main(int argc, char* argv[]){
    char *filename = (argc >= 2) ? argv[1] : (char*) "/Users/ryohei/gitrepos/cvgl/sushi.jpg";

    img0 = cv::imread(filename);
    if(img0.empty()){
        return 0;
    }

    printf("Hot keys: \n"
           "\t ESC - quit the program\n"
           "\t i or ENTER - run inpainting algorithm\n"
           "\t (before running it, paint something on  the image)");

    cv::namedWindow("image", 1);

    img = img0.clone();
    inpainted = img0.clone();
    inpaint_mask.create(img0.size(), CV_8UC1);

    inpaint_mask = cv::Scalar(0);
    inpainted = cv::Scalar(0);
    imshow("image", img);

    cv::setMouseCallback("image", on_mouse, 0);

    bool loop_flag = true;

    while(loop_flag){
        int c = cv::waitKey();
        switch(c){
            case 27: // ESC
            case 'q':
                loop_flag = false;
                break;
            case 'r':
                inpaint_mask = cv::Scalar(0);
                img0.copyTo(img);
                cv::imshow("image", img);
                break;
            case 'i':
            case 10:
                cv::namedWindow("inpainted image", 1);
                cv::inpaint(img, inpaint_mask, inpainted, 3.0, cv::INPAINT_TELEA);
                cv::imshow("inpainted image", inpainted);
                break;
        }
    }
    return 0;


}