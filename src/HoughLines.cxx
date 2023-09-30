#include <opencv2/opencv.hpp>
// #include <opencv2/core.hpp>
// #include <opencv2/imgproc.hpp>
#include <vesuvianite/HoughLines.hpp>

using namespace std;
using namespace cv;

namespace hl
{
  void houghLines(cv::Mat& image)
  {
    Mat dst, color_dst;

    Canny(image, dst, 50, 200, 3);
    cvtColor(dst, color_dst, COLOR_GRAY2BGR);
    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI / 180, 80, 30, 10);
    for (size_t i = 0; i < lines.size(); i++)
    {
      line(color_dst, Point(lines[i][0], lines[i][1]),
           Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
    }
    namedWindow("Source", 1);
    imshow("Source", image);
    namedWindow("Detected Lines", 1);
    imshow("Detected Lines", color_dst);
    waitKey(0);
  }
}