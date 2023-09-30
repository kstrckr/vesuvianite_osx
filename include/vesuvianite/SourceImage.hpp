#pragma once

#include <opencv2/opencv.hpp>


namespace Source
{
  class ProcessingTarget
  {
  public:
    std::string sourcePath;
    cv::Mat sourceImage;
    cv::Mat isolationThumbnail;

    cv::RotatedRect thumbnailSubjectRectangle;
    bool thumbnailIsLikelyIsolated;
    bool subjectIsLikelyIsolated;
    cv::Rect fullIsolationBounding;
    cv::Rect finalBoundingBox;
    cv::RotatedRect finalSubjectRectrangle;
    cv::RotatedRect fullIsolationRotatedRect;

    ProcessingTarget(std::string pathToRaw);

    void displaySourceImage();
    void displayThumbnailSource();
    cv::Mat drawThumbnailWithRect();
    cv::Mat drawFullImageWithRect();
    cv::Mat drawFinalImageWithRect();
    cv::Mat drawContours();

    float cropTop;
    float cropLeft;
    float cropBottom;
    float cropRight;
    float cropAngle;

  private:
    cv::Scalar lineColor;
    int thumbnailScaleFactor;
    int thumbnailWidth;
    int thumbnailHeight;
    int thumbnailGaussianKernelSize;
    int thumbnailThresholdValue;
    int thumbnailAreaDivisor;

    int fullsizeGaussianKernelSize;
    int fullsizeThresholdValue;

    void thumbnailIsolation();
    void fullsizeIsolation();
    void scaleIsollationRect();
  };
}

// #endif
