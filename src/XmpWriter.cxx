#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>

#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1
#define XMP_MacBuild 1
#define MAC_ENV 1
// #define UNIX_ENV 1

#include <XMP.incl_cpp>
#include <XMP.hpp>
#include <XMP_Const.h>

#include <vesuvianite/XmpWriter.hpp>

using namespace std;

static FILE *sLogFile = stdout;

int XmpTool::XmpWriter::WriteXmp(std::string xmpRawText)
{
  ofstream o;
  std::string outFilePath = outPathString + ".xmp";
  cout << outFilePath << endl;
  o.open(outFilePath, ios::out);
  o << xmpRawText;
  o.close();
  return 0;
}

int XmpTool::XmpWriter::ProcessFile(std::string pathToRaw)
{
  int generalCloseFlags = 0;
  XMP_FileFormat xmpFileFormat = kXMP_UnknownFile;

  SXMPFiles xmpFile;
  std::string xmpDump;
  xmpFile.OpenFile(pathToRaw,xmpFileFormat,kXMPFiles_OpenForRead);
  printf("file opened");
  SXMPMeta xmpMeta;
  if (!xmpFile.GetXMP(&xmpMeta, 0, 0))
    printf("file contains no XMP");
  // Log::warn("file contains no XMP. - says xmpFile.GetXMP()");
  else
  {
    if (hasCrop) {
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "CropTop", std::to_string(cropTop));
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "CropLeft", std::to_string(cropLeft));
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "CropBottom", std::to_string(cropBottom));
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "CropRight", std::to_string(cropRight));
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "CropAngle", std::to_string(cropAngle));
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "CropConstrainToWarp", "0");
      xmpMeta.SetProperty("http://ns.adobe.com/camera-raw-settings/1.0/", "HasCrop", "True");
    } else {
      xmpMeta.SetProperty("http://ns.adobe.com/xap/1.0/", "Label", "Red");
    }


    XMP_OptionBits outOpts = kXMP_OmitPacketWrapper | kXMP_UseCanonicalFormat;
    xmpMeta.SerializeToBuffer(&xmpDump, outOpts);
    XmpTool::XmpWriter::WriteXmp(xmpDump);
    // printf("%s",xmpDump.c_str());
  }


  xmpFile.CloseFile(generalCloseFlags);
  return 0;
}

XmpTool::XmpWriter::XmpWriter(std::string pathToRaw, Source::ProcessingTarget processedSource)
{
  if (!SXMPMeta::Initialize())
  {
    printf("## SXMPMeta::Initialize failed!\n");
    exit(1);
  }

  XMP_OptionBits options = 0;
#if UNIX_ENV
  options |= kXMPFiles_ServerMode;
#endif

  if (!SXMPFiles::Initialize(options))
  {

    printf("## SXMPFiles::Initialize failed!\n");
    exit(1);
  }
  const char *path = pathToRaw.c_str();
  if (processedSource.thumbnailIsLikelyIsolated && processedSource.subjectIsLikelyIsolated) {
    cropLeft = processedSource.cropLeft;
    cropRight = processedSource.cropRight;
    cropTop = processedSource.cropTop;
    cropBottom = processedSource.cropBottom;
    cropAngle = processedSource.cropAngle;
    hasCrop = true;
  } else {
    label = "Red";
    hasCrop = false;
  }


  int len = strlen(path);
  outPathString = pathToRaw.substr(0, len - 4);

  XmpTool::XmpWriter::ProcessFile(path);

  SXMPFiles::Terminate();
  SXMPMeta::Terminate();
}
