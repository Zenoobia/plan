/*
 * ImageProcessor.h
 *
 */

#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "ImageInput.h"
#include "Config.h"

enum class BhThresholdMethod{OTSU,NIBLACK,SAUVOLA,WOLFJOLION};
class BhThresholder
{
public :
  void doThreshold(cv::InputArray src ,cv::OutputArray dst,const BhThresholdMethod &method);
private:
};


using Digits = std::vector<cv::Mat>;
using Rects = std::vector<cv::Rect>;
using Contours = std::vector<std::vector<cv::Point>>;

class ImageProcessor {
public:
  ImageProcessor(const Config & config);
  ~ImageProcessor() = default;

  void setOrientation(int rotationDegrees);
  void setInput(cv::Mat & img);
  void process();
  void processTess();
  void processTess(PIX* pix);

  const Digits & getDigits();
  const Rects & getDigitsRects();
  std::vector<PIX *> const & getDigitsPix();
  
  void debugWindow(bool bval = true);
  void debugSkew(bool bval = true);
  void debugEdges(bool bval = true);
  void debugDigits(bool bval = true);
  void showImage();
  void saveConfig();
  void loadConfig();

public:
  void filterSpots(const cv::Mat &img, cv::Mat &imgOut); 
  void rotate(double rotationDegrees);
  cv::Mat rotate(const cv::Mat &cImg, double rotationDegrees); 

  float detectSkew();
  float detectSkew(const cv::Mat &cImg); 

  void drawLines(std::vector<cv::Vec2f>& lines);
  void drawLines(std::vector<cv::Vec4i>& lines, int xoff=0, int yoff=0);
public:
  
  cv::Mat cannyEdges();
  cv::Mat cannyEdges(const cv::Mat &cImg); 
  void filterContours(Contours &contours, Rects &boundingBoxes, Contours &filteredContours);


private:
  cv::Mat _img;
  cv::Mat _imgGray;
  std::vector<cv::Mat> _digits;
  std::vector<Digits> _vDigits;
  Rects _vDigitsRects;

  std::vector<PIX* > _vDigitsPix;
  tesseract::TessBaseAPI *tessApi;

private:

  Config _config;
  bool _debugWindow;
  bool _debugSkew;
  bool _debugEdges;
  bool _debugDigits;
};

#endif /* IMAGEPROCESSOR_H_ */

