#include <math.h>

#include <libraw.h>
#include <opencv2/opencv.hpp>

#include <vesuvianite/SourceImage.hpp>

using namespace std;

Source::ProcessingTarget::ProcessingTarget(std::string pathToRaw)
{
  // int i;
  int ret = 0;
  thumbnailIsLikelyIsolated = false;
  subjectIsLikelyIsolated = false;

  LibRaw RawProcessor;

  sourcePath = pathToRaw;

  // pull jpeg thumbnail from raw file
  const char *pathForLibraw = sourcePath.c_str();
  RawProcessor.open_file(pathForLibraw);
  RawProcessor.unpack_thumb();

  // convert in-memory thumbnail into opencv Mat
  uint bufsize = RawProcessor.imgdata.thumbnail.tlength;
  cv::Mat rawData(1, bufsize, CV_8UC1, RawProcessor.imgdata.thumbnail.thumb);

  imdecode(rawData, cv::IMREAD_COLOR, &sourceImage);
  if (sourceImage.data == NULL)
  {
    puts("Error reading raw file");
  }

  cv::Size sourceSize = sourceImage.size();

  thumbnailScaleFactor = 25;
  thumbnailAreaDivisor = 4;

  thumbnailWidth = sourceSize.width / thumbnailScaleFactor;
  thumbnailHeight = sourceSize.height / thumbnailScaleFactor;
  cv::resize(sourceImage, isolationThumbnail, cv::Size(thumbnailWidth, thumbnailHeight), 0, 0, cv::INTER_CUBIC);

  // namedWindow("isolationThumbnail jpg from buffer", cv::WINDOW_NORMAL);
  // imshow("jpg from buffer", isolationThumbnail);
  // cv::waitKey(0);

  thumbnailGaussianKernelSize = 15;
  thumbnailThresholdValue = 18;

  fullsizeGaussianKernelSize = 65;
  fullsizeThresholdValue = 115;

  lineColor = cv::Scalar(0, 255, 0);

  thumbnailIsolation();
  if (thumbnailIsLikelyIsolated)
  {
    scaleIsollationRect();
    fullsizeIsolation();
  }
}

void Source::ProcessingTarget::thumbnailIsolation()
{
  cv::Mat grey, blurred_grey, canny_output;
  cv::cvtColor(isolationThumbnail, grey, cv::COLOR_BGR2GRAY);
  cv::GaussianBlur(grey, blurred_grey, cv::Size(thumbnailGaussianKernelSize, thumbnailGaussianKernelSize), 0);
  cv::Canny(blurred_grey, canny_output, thumbnailThresholdValue, thumbnailThresholdValue * 2, 3, true);
  vector<vector<cv::Point>> contours;
  vector<cv::Vec4i> hierarchy;
  findContours(canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
  vector<cv::RotatedRect> minRect(contours.size());
  for (size_t i = 0; i < contours.size(); i++)
  {
    minRect[i] = minAreaRect(contours[i]);
  }
  for (size_t i = 0; i < contours.size(); i++)
  {
    // printf("angle %f\n", minRect[i].angle);
    if ((abs(minRect[i].angle) <= 20 || abs(minRect[i].angle) >= 80) && (minRect[i].size.width * minRect[i].size.height) >= ((thumbnailHeight * thumbnailWidth) / thumbnailAreaDivisor))
    {
      // cv::Point2f rect_points[4];
      // minRect[i].points(rect_points);
      thumbnailSubjectRectangle = minRect[i];
      thumbnailIsLikelyIsolated = true;
      break;
    }
  }
  printf("\nthumbnailIsLikelyIsolated %i\n", thumbnailIsLikelyIsolated);
};

void Source::ProcessingTarget::scaleIsollationRect()
{
  cv::Size baseSize;
  cv::RotatedRect scaledRotatedRect;

  baseSize = sourceImage.size();
  cv::Point2f scaledCenter = cv::Point2f(
      thumbnailSubjectRectangle.center.x / thumbnailWidth * baseSize.width,
      thumbnailSubjectRectangle.center.y / thumbnailHeight * baseSize.height);
  cv::Size2f scaledSize = cv::Size2f(
      (thumbnailSubjectRectangle.size.width / thumbnailWidth * baseSize.width) + (0.05 * baseSize.width),
      (thumbnailSubjectRectangle.size.height / thumbnailHeight * baseSize.height) + (0.05 * baseSize.width));
  scaledRotatedRect = cv::RotatedRect(scaledCenter, scaledSize, thumbnailSubjectRectangle.angle);
  // cv::Scalar lineColor = cv::Scalar(0, 255, 0);
  cv::Point2f scaled_points[4];
  // scaledRotatedRect.points(scaled_points);
  cv::Rect boundingRect = scaledRotatedRect.boundingRect();
  fullIsolationRotatedRect = scaledRotatedRect;
  fullIsolationBounding = boundingRect;
}

cv::Mat Source::ProcessingTarget::drawThumbnailWithRect()
{
  cv::Mat clonedThumbnail = isolationThumbnail.clone();
  cv::Point2f rect_points[4];
  thumbnailSubjectRectangle.points(rect_points);
  for (int j = 0; j < 4; j++)
  {
    cv::line(clonedThumbnail, rect_points[j], rect_points[(j + 1) % 4], lineColor);
  }
  return clonedThumbnail;
}

cv::Mat Source::ProcessingTarget::drawFullImageWithRect()
{
  cv::Mat clonedFullImage = sourceImage.clone();
  cv::Point2f rect_points[4];
  fullIsolationRotatedRect.points(rect_points);
  for (int j = 0; j < 4; j++)
  {
    cv::line(clonedFullImage, rect_points[j], rect_points[(j + 1) % 4], lineColor, 8);
  }
  return clonedFullImage;
}

cv::Mat Source::ProcessingTarget::drawFinalImageWithRect()
{
  cv::Mat clonedFullImage = sourceImage.clone();
  cv::Point2f rect_points[4];
  finalSubjectRectrangle.points(rect_points);
  for (int j = 0; j < 4; j++)
  {
    cv::line(clonedFullImage, rect_points[j], rect_points[(j + 1) % 4], lineColor, 8);
  }
  return clonedFullImage;
}

void Source::ProcessingTarget::fullsizeIsolation()
{
  cv::Mat grey, src_gray, blurred, blurred_grey, canny_output, convex_hull, dst;

  cv::Size rawSize = sourceImage.size();
  if (fullIsolationBounding.x < 0)
  {
    fullIsolationBounding.x = 0;
  }
  if (fullIsolationBounding.y < 0)
  {
    fullIsolationBounding.y = 0;
  }
  if (fullIsolationBounding.x + fullIsolationBounding.width > rawSize.width)
  {
    fullIsolationBounding.width = rawSize.width - fullIsolationBounding.x;
  }
  if (fullIsolationBounding.y + fullIsolationBounding.height > rawSize.height)
  {
    fullIsolationBounding.height = rawSize.height - fullIsolationBounding.y;
  }

  cv::Mat isolatedROI = sourceImage(fullIsolationBounding);
  cv::cvtColor(isolatedROI, src_gray, cv::COLOR_BGR2GRAY);
  cv::GaussianBlur(src_gray, blurred_grey, cv::Size(fullsizeGaussianKernelSize, fullsizeGaussianKernelSize), 0);

  // cv::namedWindow("blurred", cv::WINDOW_NORMAL);
  // cv::imshow("blurred", blurred_grey);
  // cv::waitKey(0);

  // cv::Canny(blurred_grey, canny_output, fullsizeThresholdValue, fullsizeThresholdValue * 3, 3);
  cv::threshold(blurred_grey, canny_output, fullsizeThresholdValue, 255, cv::THRESH_BINARY);
  // cv::namedWindow("canny", cv::WINDOW_NORMAL);
  // cv::imshow("canny", canny_output);
  // cv::waitKey(0);

  vector<vector<cv::Point>> contours;
  vector<cv::Vec4i> hierarchy;
  findContours(canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

  int red = 255;
  // draw contours
  // for (size_t i = 0; i < contours.size(); i++)
  // {
  // cv::drawContours(isolatedROI, contours, (int)i, cv::Scalar(0, red, 0), 15);
  // if (red <= 255) {
  // red++;
  // } else {
  //   red = 0;
  // }
  // cv::namedWindow("contours", cv::WINDOW_NORMAL);
  // cv::imshow("contours", isolatedROI);
  // cv::waitKey(0);
  // }
  // cv::namedWindow("contours", cv::WINDOW_NORMAL);
  // cv::imshow("contours", isolatedROI);
  // cv::waitKey(0);

  vector<vector<cv::Point>> hull(hierarchy.size());
  vector<cv::RotatedRect> minRect(hull.size());

  cv::RotatedRect rotatedRectInROI;

  for (size_t i = 0; i < hierarchy.size(); i++)
  {
    convexHull(contours[i], hull[i]);

    if (hierarchy[i][3] == -1)
    {
      // cv::drawContours(isolatedROI, contours, (int)i, cv::Scalar(0, 0, red), 5);
      // if (red <= 255)
      // {
      //   red++;
      // }
      // else
      // {
      //   red = 0;
      // }

      minRect[i] = minAreaRect(hull[i]);
      if (minRect[i].size.area() / fullIsolationRotatedRect.size.area() > 0.5)
      {
        printf("area ratio %f\n", minRect[i].size.area() / sourceImage.size().area());
      }

      if ((abs(minRect[i].angle) <= 20 || abs(minRect[i].angle) >= 80) && minRect[i].size.area() / fullIsolationRotatedRect.size.area() > 0.5)
      {
        cv::Point2f rect_points[4];
        minRect[i].points(rect_points);
        rotatedRectInROI = minRect[i];
        subjectIsLikelyIsolated = true;
        // for (int j = 0; j < 4; j++)
        // {
        //   line(isolatedROI, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 0, 255), 25);
        // }

        // break;
      }
    }
  }

  // draw hulls
  // cv::namedWindow("hulls", cv::WINDOW_NORMAL);
  // cv::imshow("hulls", isolatedROI);
  // cv::waitKey(0);

  cv::Point2f scaledCenter = cv::Point2f(
      rotatedRectInROI.center.x + fullIsolationBounding.x,
      rotatedRectInROI.center.y + fullIsolationBounding.y);

  finalSubjectRectrangle = cv::RotatedRect(scaledCenter, rotatedRectInROI.size, rotatedRectInROI.angle);
  cv::Size cropSize = finalSubjectRectrangle.size;
  cv::Size originalSize = sourceImage.size();
  cv::Point2f vtx[4];
  finalSubjectRectrangle.points(vtx); // bottomLeft, topLeft, topRight, bottomRight
  // topLeft and bottomRight are the only coordinates that matter for xmp
  cv::Point2f topLeft = vtx[1];
  cv::Point2f bottomRight = vtx[3];

  float rotatedYOffset = sin(rotatedRectInROI.angle) * 50;
  float rotatedXOffset = cos(rotatedRectInROI.angle) * 50;

  printf("angle = %f\n", rotatedRectInROI.angle);

  int padding = 330;

  if (rotatedRectInROI.angle > 45 && rotatedRectInROI.angle < 90)
  {
    // counter clockwise
    // printf("rotateRight?\n");
    cropAngle = rotatedRectInROI.angle - 90;
    cropTop = (topLeft.y - padding) / originalSize.height;
    cropLeft = (topLeft.x  + padding) / originalSize.width;

    cropBottom = (bottomRight.y + padding) / originalSize.height;
    cropRight = (bottomRight.x  + padding) / originalSize.width;
  }
  else
  {
    // clockwise
    // printf("rotateLeft?\n");
    cropAngle = rotatedRectInROI.angle;
    cropTop = (topLeft.y - padding) / originalSize.height;
    cropLeft = (topLeft.x  - padding) / originalSize.width;

    cropBottom = (bottomRight.y + padding) / originalSize.height;
    cropRight = (bottomRight.x  + padding) / originalSize.width;
  }
  printf("subjectIsLikelyIsolated   %i\n\n", subjectIsLikelyIsolated);
};
