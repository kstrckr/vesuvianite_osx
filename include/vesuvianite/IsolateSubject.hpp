#pragma once

#include <opencv2/opencv.hpp>

namespace IsolateSubject
{
  cv::RotatedRect isolate(cv::Mat image);
}
