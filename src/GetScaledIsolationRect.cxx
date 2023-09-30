#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vesuvianite/GetScaledIsolationRect.hpp>
// #include <vesuvianite/SubjectPlacement.hpp>

using namespace cv;

float px;
float py;
cv::Size baseSize;
cv::RotatedRect scaledRotatedRect;
Placement scaledRects;
namespace GetScaledIsolationRect
{
  Placement getScaledRectAndBound(cv::Mat &image, cv::RotatedRect isolationRect)
  {
    baseSize = image.size();
    px = 150;
    py = 100;
    Point2f scaledCenter = Point2f(
      isolationRect.center.x / px * baseSize.width,
      isolationRect.center.y / py * baseSize.height
    );
    Size2f scaledSize = Size2f(
      (isolationRect.size.width / px * baseSize.width) + 0.02 * baseSize.width,
      (isolationRect.size.height / py * baseSize.height) + 0.02 * baseSize.width
    );
    scaledRotatedRect = RotatedRect(scaledCenter, scaledSize, isolationRect.angle);
    Scalar color = Scalar(0, 255, 0);
    Point2f scaled_points[4];
    scaledRotatedRect.points(scaled_points);
    // for (int j = 0; j < 4; j++)
    // {
    //   line(image, scaled_points[j], scaled_points[(j + 1) % 4], color, 25);
    // }
    Rect boundingRect = scaledRotatedRect.boundingRect();
    // rectangle(image, boundingRect, Scalar(0, 255, 0), 5);
    // namedWindow("Display Image", cv::WINDOW_NORMAL);
    // cv::imshow("Display Image", image);
    // cv::waitKey(0);
    // imwrite("/home/a/proj/vesuvianite/output_images/out_full.jpg", image);
    
    scaledRects.isolationRect = scaledRotatedRect;
    scaledRects.isolationBoundingRect = boundingRect;
    
    return scaledRects;
  }
}
