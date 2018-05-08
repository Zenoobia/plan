#ifndef IMAGEINPUT_H_
#define IMAGEINPUT_H_

#include <ctime>
#include <string>
#include <list>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <leptonica/pix.h>
#include <leptonica/allheaders.h>

#include "Directory.h"
#include "../PDFUtils.h"

Pix *mat8ToPix(cv::Mat const &mat8);

class ImageInput {
public:
  virtual ~ImageInput();

  virtual bool nextImage() = 0;

  virtual cv::Mat & getImage();
  virtual std::string &getImageName(); 
  virtual time_t getTime();
  virtual void setOutputDir(const std::string & outDir);
  static cv::Mat loadImage(const std::string &path);
  virtual void saveImage();
  virtual void saveImage(cv::Mat &img, const std::string &fileName) const;

public:
  virtual bool nextPix() = 0;
  Pix* getPix() const;
  static Pix* loadPix(Directory &directory, const std::string &fileName);
  Pix *_pPix = nullptr;
protected:
  cv::Mat _img;
  std::string _imgName; 
  time_t _time;
  std::string _outDir;
};

class DirectoryInput: public ImageInput {
public:
  DirectoryInput(const Directory & directory);

  virtual bool nextImage();
  virtual bool nextPix();

  static cv::Mat loadImage(Directory &directory, const std::string &fileName);
  static cv::Mat loadImage(const std::string &path);
private:
  Directory _directory;
  std::list<std::string>::const_iterator _itFilename;
  std::list<std::string> _filenameList;
};


class CameraInput: public ImageInput {
public:
  CameraInput(int device);

  virtual bool nextImage();

private:
  cv::VideoCapture _capture;
};

#endif /* IMAGEINPUT_H_ */
