#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define FLAG 1

char preset_file[] = "/Users/ryohei/gitrepos/cvgl/fruits.jpg";
void my_image_processing(cv::Mat &input, cv::Mat &processed);

int main(int argc, char* argv[])
{
    char *input_file;
    cv::Mat input, processed;
    if(argc == 2){
        input_file = argv[1];
    }else{
        input_file = preset_file;
    }
    //read an image from the specified file
    input = cv::imread(input_file);

    if(input.empty()){
        fprintf(stderr, "cannot open %s", input_file);
        exit(1);
    }

    my_image_processing(input, processed);

    cv::namedWindow("processed image", 1);
    cv::imshow("processed image", processed);
    cv::waitKey(0);
    cv::imwrite("processed.jpg", processed);
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
