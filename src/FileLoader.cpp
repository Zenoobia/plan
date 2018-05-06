/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim $
   ======================================================================== */
#include "FileLoader.h"

namespace plan {

  cv::Mat FileLoader::readPDFtoCV(const std::string& fileName,int DPI) {
    document* mypdf = document::load_from_file(fileName);

    if(mypdf == NULL) {
      std::cerr << "couldn't read pdf : " << fileName << "\n";
      return cv::Mat();
    }
    std::cout << "pdf has " << mypdf->pages() << " pages\n";
    page* mypage = mypdf->create_page(0);
 
    page_renderer renderer;
    renderer.set_render_hint(page_renderer::text_antialiasing);
    image myimage = renderer.render_page(mypage,DPI,DPI);
    std::cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";
 
    cv::Mat cvimg;
    if(myimage.format() == image::format_rgb24) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
    } else if(myimage.format() == image::format_argb32) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
    } else {
      std::cerr << "PDF format no good\n";
      return cv::Mat();
    }
    return cvimg;
    return cv::Mat();
  }



  std::string FileLoader::getFileExtension(const std::string& fileName) {
    if(fileName.find_last_of(".") != std::string::npos)
      return {fileName.substr(fileName.find_last_of(".")+1), true};
    return {"", false};
  }
};
