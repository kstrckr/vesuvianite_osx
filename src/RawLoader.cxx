#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vesuvianite/RawLoader.hpp>
using namespace std;
using namespace cv;

namespace imageProc {
  cv::Mat loadRaw(std::string path) {

    cv::Mat image;
    cv::Mat downsizedImage(1158, 1736, CV_8UC3);

    image = cv::imread(path);
    if ( !image.data )
    {
        printf("No image data \n");
    }

    // cv::resize(image, downsizedImage, downsizedImage.size(), 0, 0, cv::INTER_CUBIC);
    // return downsizedImage;

    // namedWindow("Display Image", cv::WINDOW_NORMAL );
    // cv::imshow("Display Image", downsizedImage);
    // cv::waitKey(0);

    return image;
  }
} // namespace lib
