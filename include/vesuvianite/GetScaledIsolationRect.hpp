#pragma once

#include <opencv2/opencv.hpp>
#include <vesuvianite/SubjectPlacement.hpp>

namespace GetScaledIsolationRect
{
  Placement getScaledRectAndBound(cv::Mat &image, cv::RotatedRect);
}