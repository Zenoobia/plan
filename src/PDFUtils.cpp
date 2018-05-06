#include "PDFUtils.h"

std::string basename( std::string const& pathname) {
  return std::string( 
		     std::find_if( pathname.rbegin(), pathname.rend(),
				   []( const char ch )
				   {
#if 1 // UNIX
				     return ch == '/';
#else // WINDOWS
				     return ch == '\\' || ch == '/';
#endif
				   }
				   ).base(), pathname.end() );
}

std::string removeExtension( std::string const& filename ) {
  std::string::const_reverse_iterator
    pivot
    = std::find( filename.rbegin(), filename.rend(), '.' );
  return pivot == filename.rend()
    ? filename
    : std::string( filename.begin(), pivot.base() - 1 );
}

static std::vector<cv::String> ListDirectory(const cv::String path) noexcept {
  std::vector<cv::String> result;; // std::string in opencv2.4, but cv::String in 3.0
  cv::glob(path, result, false);
  return result;
}

//static cv::Mat readPDFtoCV(std::string const &filename, int const DPI) {
cv::Mat readPDFtoCV(std::string const &filename) {
  poppler::document* mypdf = poppler::document::load_from_file(filename);
  static int const DPI = 200; 
  if(mypdf == NULL) {
    std::cerr << "couldn't read pdf\n";
    return cv::Mat();
  }
  std::cout << "pdf has " << mypdf->pages() << " pages\n";
  page* mypage = mypdf->create_page(0);

  page_renderer renderer;
  renderer.set_render_hint(page_renderer::text_antialiasing);
  image myimage = renderer.render_page(mypage,DPI,DPI);
  std::cout << "created image of  " << myimage.width() << "x" << myimage.height() << "\n";

  cv::Mat cvimg;
#if 0
    if(myimage.format() == image::format_rgb24) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
      std::cout << "Loaded PDF format rgb24 as CV_8UC3\n";
    } else if(myimage.format() == image::format_argb32) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
      std::cout << "Loaded PDF format argb32 as CV_8UC4\n";
    } else if(myimage.format() == image::format_mono) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC1,myimage.data()).copyTo(cvimg);
      std::cout << "Loaded PDF format mono as CV_8UC1\n";
    } else {
      std::cerr << "PDF format no good\n";
      return cv::Mat();
    }
#else // Force mono
    cv::Mat(myimage.height(),myimage.width(),CV_8UC1,myimage.data()).copyTo(cvimg);
#endif
    return cvimg;
  }
