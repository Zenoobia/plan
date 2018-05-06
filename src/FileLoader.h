#if !defined(FILELOADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim Planstedt $
   ======================================================================== */

#define FILELOADER_H

#include <iostream>
#include <dirent.h>

#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>

#include <opencv2/imgproc.hpp>
//#include <opencv2/core/core.hpp>

struct loaded_doc {
  std::string path;
};

namespace plan {

  using poppler::document;
  using poppler::page;
  using poppler::image;
  using poppler::page_renderer;

  class FileLoader {
    static std::string getFileExtension(const std::string &fileName);
    static cv::Mat readPDFtoCV(const std::string& fileName,int DPI);
  };
};

#endif
