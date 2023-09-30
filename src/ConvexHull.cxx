#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <ctime>
#include <chrono>
#include <vesuvianite/ConvexHull.hpp>

using namespace cv;
using namespace std;

namespace cvxHull
{
  int kernelSize = 15;
  Mat shrunken, grey, src_gray, blurred, blurred_grey, canny_output, contours, convex_hull, dst;
  int threshold_value = 50;
  int threshold_type = 0;
  int const max_value = 255;
  int const max_type = 4;
  int const max_binary_value = 255;
  RNG rng(12345);

  void convexHull(Placement pd, cv::Mat &image)
  {
    Size rawSize = image.size();
    if (pd.isolationBoundingRect.x < 0)
    {
      pd.isolationBoundingRect.x = 0;
    }
    if (pd.isolationBoundingRect.y < 0)
    {
      pd.isolationBoundingRect.y = 0;
    }
    if (pd.isolationBoundingRect.x + pd.isolationBoundingRect.width > rawSize.width)
    {
      pd.isolationBoundingRect.width = rawSize.width - pd.isolationBoundingRect.x;
    }
    if (pd.isolationBoundingRect.y + pd.isolationBoundingRect.height > rawSize.height)
    {
      pd.isolationBoundingRect.height = rawSize.height - pd.isolationBoundingRect.y;
    }

    Mat isolatedROI(image, pd.isolationBoundingRect);
    cvtColor(isolatedROI, src_gray, COLOR_BGR2GRAY);
    GaussianBlur(src_gray, blurred_grey, Size(kernelSize, kernelSize), 0);
    // imshow("Sample", isolatedROI);
    // waitKey(0);

    Mat canny_output;
    Canny(blurred_grey, canny_output, threshold_value, threshold_value * 2);
    // namedWindow("Canny", cv::WINDOW_NORMAL);
    // imshow("Canny", canny_output);
    // waitKey(0);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // draw contours
    // for (size_t i = 0; i < contours.size(); i++)
    // {
    //   drawContours(isolatedROI, contours, (int)i, Scalar(0, 255, 0), 15);
    // }
    // namedWindow("contours", cv::WINDOW_NORMAL);
    // imshow("contours", isolatedROI);
    // waitKey(0);

    // hulls and rectangles
    vector<vector<Point>> hull(hierarchy.size());
    vector<RotatedRect> minRect(hull.size());

    for (size_t i = 0; i < hierarchy.size(); i++)
    {
      convexHull(contours[i], hull[i]);
      minRect[i] = minAreaRect(hull[i]);
      if (minRect[i].size.area() >= pd.isolationBoundingRect.area() * 0.66)
      {
        Point2f rect_points[4];
        minRect[i].points(rect_points);
        for (int j = 0; j < 4; j++)
        {
          line(isolatedROI, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 25);
        }

        break;
      }
    }
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    long duration = value.count();
    std::string timeString = std::to_string(duration);
    std::string outputPath = "/home/a/proj/vesuvianite/output_images/";

    imwrite(outputPath + timeString + ".jpg", isolatedROI);

    // namedWindow("minRects", cv::WINDOW_NORMAL);
    // imshow("minRects", isolatedROI);
    // waitKey(0);

    // // Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    // for (size_t i = 0; i < hull.size(); i++)
    // {
    //   if (minRect[i].size.area() >= rawSize.area() / 4)
    //   {
    //     Scalar color = Scalar(0, 255, 0);
    //     Point2f rect_points[4];
    //     minRect[i].points(rect_points);
    //     for (int j = 0; j < 4; j++)
    //     {
    //       line(isolatedROI, rect_points[j], rect_points[(j + 1) % 4], color, 25);
    //     }

    //   }
    // Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
    // vector<vector<Point>> hull(contours.size());
    // convexHull(contours[i], hull[i]);
    // drawContours(isolatedROI, hull, (int)i, color, 2);
    // }
    // imshow("Sample", isolatedROI);
    // waitKey(0);
    // find the one without any parents
    // for (size_t i = 0; i < hierarchy.size(); i++)
    // {
    //   if (hierarchy[i][3] == -1)
    //   {
    //     Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
    //     // drawContours(isolatedROI, contours, (int)i, color, 5);
    //     vector<vector<Point>> hull(contours.size());
    //     convexHull(contours[i], hull[i]);
    //     RotatedRect rotatedRect = minAreaRect(hull[i]);

    //     // drawContours(isolatedROI, hull, (int)i, Scalar(0, 0, 255), 5);
    //     if (rotatedRect.size.area() > pd.isolationBoundingRect.area() * 0.75)
    //     {
    //       Point2f rect_points[4];
    //       rotatedRect.points(rect_points);
    //       for (int j = 0; j < 4; j++)
    //       {
    //         line(isolatedROI, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 2);
    //       }
    //       namedWindow("Sample", cv::WINDOW_NORMAL);
    //       imshow("Sample", isolatedROI);
    //       waitKey(0);
    //       // auto now = std::chrono::system_clock::now();
    //       // auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    //       // auto epoch = now_ms.time_since_epoch();
    //       // auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    //       // long duration = value.count();
    //       // std::string timeString = std::to_string(duration);
    //       // std::string outputPath = "/home/a/proj/vesuvianite/output_images/";
    //       // imwrite(outputPath + timeString + ".png", isolatedROI);
    //       return;
    //     }
    //   }
    // }
    // namedWindow("Sample", cv::WINDOW_NORMAL);
    // imshow("Sample", isolatedROI);
    // waitKey(0);
  }
}
