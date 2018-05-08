#include <ctime>
#include <string>
#include <list>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ImageInput.h"

#if (!defined(_WIN32) && !defined(_WIN64))

#else
#define localtime_r localtime;
#endif

  Pix *mat8ToPix(cv::Mat const &mat8)  {
    Pix *pixd = pixCreate(mat8.size().width, mat8.size().height, 8);
    for(int y=0; y<mat8.rows; y++) {
      for(int x=0; x<mat8.cols; x++) {
        pixSetPixel(pixd, x, y, (l_uint32) mat8.at<uchar>(y,x));
      }
    }
    return pixd;
  }

  ImageInput::~ImageInput() {
  _img.release();
  pixDestroy(&_pPix); 
  }

  cv::Mat& ImageInput::getImage() {
  return _img;
  }
std::string &ImageInput::getImageName() {
  return _imgName; 
  }

time_t ImageInput::getTime() {
  return _time;
  }

void ImageInput::setOutputDir(const std::string & outDir) {
  _outDir = outDir;
}

void ImageInput::saveImage(cv::Mat &img, const std::string &fileName) const {
  char filename[PATH_MAX];
  std::string path = _outDir + fileName;
  if (cv::imwrite(path, _img)) {
    std::cout << "Image (" << fileName << ") saved to " + path;
  }
}


void ImageInput::saveImage() {
  struct tm date;
  localtime_r(&_time, &date);
  char filename[PATH_MAX];
  strftime(filename, PATH_MAX, "/%Y%m%d-%H%M%S.png", &date);
  std::string path = _outDir + filename;
  if (cv::imwrite(path, _img)) {
    std::cout << "Image saved to " + path;
  }
}

Pix* ImageInput::getPix() const {
  return _pPix;
}

Pix* ImageInput::loadPix(Directory &directory, const std::string &fileName) {
  return pixRead(directory.fullpath(fileName).c_str());
}

cv::Mat DirectoryInput::loadImage(const std::string &path) {
  std::cout << "PDF.length" << std::string{"pdf"}.length() << std::endl;
  if (path.find("pdf", (path.length() - 3)) != std::string::npos) {
    return readPDFtoCV(path);
  }
  return cv::imread(path.c_str(), 0);
}

DirectoryInput::DirectoryInput(const Directory& directory) : _directory(directory) {
  _filenameList = _directory.list();
  _filenameList.sort();
  _itFilename = _filenameList.begin();
}

cv::Mat DirectoryInput::loadImage(Directory &directory, const std::string &fileName) {
  std::cout << "PDF.length " << std::string{"pdf"}.length() << std::endl;
  //return loadImage(directory.fullpath(fileName)); 
  if (directory.hasExtension(fileName.c_str(), "pdf")) {
     return readPDFtoCV(directory.fullpath(fileName));
  }
  return cv::imread(directory.fullpath(fileName));
}

bool DirectoryInput::nextPix() {
  if (_itFilename == _filenameList.end()) {
    return false;
  }
  const std::string path = _directory.fullpath(*_itFilename);

  pixDestroy(&_pPix);
  _pPix = nullptr;
  _pPix = loadPix(_directory, *_itFilename);
  _imgName = *_itFilename; 

  if(_pPix == nullptr) {
    std::cout << "Failed to load " << *_itFilename << " as a Pix." << std::endl;
    return false;
  }


  // read time from file name
  struct tm date;
  memset(&date, 0, sizeof(date));
  date.tm_year = atoi(_itFilename->substr(0, 4).c_str()) - 1900;
  date.tm_mon = atoi(_itFilename->substr(4, 2).c_str()) - 1;
  date.tm_mday = atoi(_itFilename->substr(6, 2).c_str());
  date.tm_hour = atoi(_itFilename->substr(9, 2).c_str());
  date.tm_min = atoi(_itFilename->substr(11, 2).c_str());
  date.tm_sec = atoi(_itFilename->substr(13, 2).c_str());
  _time = mktime(&date);

  std::cout << "Processing " << *_itFilename << " of " << ctime(&_time);

  _itFilename++;
  return true;
};


bool DirectoryInput::nextImage() {
  if (_itFilename == _filenameList.end()) {
    return false;
  }
  const std::string path = _directory.fullpath(*_itFilename);

  _img = loadImage(_directory, *_itFilename);
  _imgName = *_itFilename; 
  if (_img.empty())
    return false;
#if 0 // Resize cv::Mat
  cv::resize(_img, _img, {0, 0}, 0.7, 0.7, CV_INTER_LANCZOS4); 
#endif


#if 1 // Load Pix
  if(_pPix != nullptr) {
    pixDestroy(&_pPix);
    _pPix = nullptr;
  }

  _pPix = loadPix(_directory, *_itFilename);
  if(_pPix == nullptr) {
    std::cout << "Failed to load " << *_itFilename << " as a Pix." << std::endl;
    return false;
  } else {
    std::cout << "Successfully loaded " << *_itFilename << " as a Pix." << std::endl;
  }

#if 0// Resize Pix
  pixResizeToMatch(_pPix, nullptr, _img.cols, _img.rows); 
#endif

#endif // End of Load Pix

#if 0
  // read time from file name
  struct tm date;
  memset(&date, 0, sizeof(date));
  date.tm_year = atoi(_itFilename->substr(0, 4).c_str()) - 1900;
  date.tm_mon = atoi(_itFilename->substr(4, 2).c_str()) - 1;
  date.tm_mday = atoi(_itFilename->substr(6, 2).c_str());
  date.tm_hour = atoi(_itFilename->substr(9, 2).c_str());
  date.tm_min = atoi(_itFilename->substr(11, 2).c_str());
  date.tm_sec = atoi(_itFilename->substr(13, 2).c_str());
  _time = mktime(&date);

  std::cout << "Processing " << *_itFilename << " of " << ctime(&_time);
#endif

  // save copy of image if requested
  if (!_outDir.empty()) {
    saveImage();
  }

  _itFilename++;
  return true;
}

CameraInput::CameraInput(int device) {
  _capture.open(device);
}

bool CameraInput::nextImage() {
  time(&_time);
  // read image from camera
  bool success = _capture.read(_img);

  std::cout << "Image captured: " << success;

  // save copy of image if requested
  if (success && !_outDir.empty()) {
    saveImage();
  }

  return success;
}


