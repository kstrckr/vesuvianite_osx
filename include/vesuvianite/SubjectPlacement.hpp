#pragma once

#include <opencv2/opencv.hpp>

struct Placement {
  cv::RotatedRect isolationRect;
  cv::Rect isolationBoundingRect;
  cv::RotatedRect finalCropData;
};
