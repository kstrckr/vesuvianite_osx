#pragma once

#include <vesuvianite/SourceImage.hpp>

namespace XmpTool
{
  class XmpWriter
  {
    public:
      XmpWriter(std::string pathToRaw, Source::ProcessingTarget processedSource);
      int ProcessFile (const std::string pathToRaw);
      int WriteXmp(std::string xmpRawText);
    private:
      float cropLeft;
      float cropRight;
      float cropTop;
      float cropBottom;
      float cropAngle;
      bool hasCrop;
      std::string label;
      std::string outPathString;
  };
}
