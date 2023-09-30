#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <vesuvianite/IsolateSubject.hpp>

using namespace cv;
using namespace std;

namespace IsolateSubject
{
  int kernelSize = 9;
  Mat shrunken, grey, blurred, blurred_grey, canny_output, contours, convex_hull, dst;
  int threshold_value = 18;
  int threshold_type = 0;
  int const max_value = 255;
  int const max_type = 4;
  int const max_binary_value = 255;
  RNG rng(12345);
  RotatedRect rect;
  cv::Size rawSize;
  cv::RotatedRect isolate(cv::Mat src)
  {
    rawSize = src.size();
    // namedWindow("IsolateSubject", cv::WINDOW_NORMAL);

    cv::Mat isolationSource(100, 150, CV_8UC3);
    cv::resize(src, shrunken, isolationSource.size(), 0, 0, cv::INTER_CUBIC);
    cvtColor(shrunken, grey, COLOR_BGR2GRAY); // Convert the image to Gray
    // imshow("IsolateSubject", grey);
    // waitKey(0);
    GaussianBlur(grey, blurred_grey, Size(kernelSize, kernelSize), 0);
    // imshow("IsolateSubject", blurred_grey);
    // waitKey(0);
    Canny(blurred_grey, canny_output, threshold_value, threshold_value * 2, 3, true);
    // adaptiveThreshold(blurred_grey, canny_output, 256, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 12);
    // imshow("IsolateSubject", canny_output);
    // waitKey(0);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> minRect(contours.size());
    for (size_t i = 0; i < contours.size(); i++)
    {
      minRect[i] = minAreaRect(contours[i]);
    }
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++)
    {
      if ((minRect[i].size.width * minRect[i].size.height) >= ((100 * 150) / 4))
      {
        // Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
        Scalar color = Scalar(255, 0, 0);
        // contour
        // drawContours(shrunken, contours, (int)i, color);
        // rotated rectangle
        Point2f rect_points[4];
        minRect[i].points(rect_points);
        // for (int j = 0; j < 4; j++)
        // {
        //   line(shrunken, rect_points[j], rect_points[(j + 1) % 4], color);
        // }
        // imshow("IsolateSubject", shrunken);
        // waitKey(0);
        return minRect[i];
      }
    }
    // imshow("Contours", shrunken);
    // waitKey(0);
    // // imshow("IsolateSubject", canny_output);
    // return canny_output;
  }
}
