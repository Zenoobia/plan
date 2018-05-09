/*
 * ImageProcessor.cpp
 *
 */

#include <vector>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ImageProcessor.h"
#include "Config.h"

ImageProcessor::ImageProcessor(const Config & config) :
        _config(config), _debugWindow(false), _debugSkew(false), _debugDigits(false), _debugEdges(false) {
}

/**
 * Set the input image.
 */
void ImageProcessor::setInput(cv::Mat &img) {
  _img = img;
}


void ImageProcessor::debugWindow(bool bval) {
  _debugWindow = bval;
  if (_debugWindow) {
    cv::namedWindow("ImageProcessor");
  }
}

void ImageProcessor::debugSkew(bool bval) {
    _debugSkew = bval;
}

void ImageProcessor::debugEdges(bool bval) {
    _debugEdges = bval;
}

void ImageProcessor::debugDigits(bool bval) {
    _debugDigits = bval;
}

void ImageProcessor::showImage() {
    cv::imshow("ImageProcessor", _img);
    cv::waitKey(1);
}

/**
 * Main processing function.
 */
void ImageProcessor::process() {

  // convert to gray if necessary
  if(_img.channels() >= 3 ) {
    cvtColor(_img, _img, CV_BGR2GRAY);
    //cvtColor(_img, _imgGray, CV_BGR2GRAY);
  }

  // Crop image
  //_imgGray = _img(cv::Rect(800, 400, _img.cols - 800, _img.rows - 400)); 
  _imgGray = _img; 

  // clear spots

  BhThresholder bt; 
  bt.doThreshold(_imgGray, _imgGray, BhThresholdMethod::OTSU);
  filterSpots(_imgGray, _imgGray); 

  _img = _imgGray; 

  if (_debugWindow) {
    showImage();
  }
}


#if 0
void ImageProcessor::processTess(PIX* pix) {
  tessApi->Clear();
  tessApi->SetImage(pix);

#if 0
  for(auto &text : _vDetectedText) {
    cv::Rect const &rect = text.Rect;
    tessApi->SetRectangle(rect.x, rect.y, rect.width, rect.height);
    tessApi->Recognize(0);
    tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;//tesseract::RIL_WORD;
    tesseract::ResultIterator* ri = tessApi->GetIterator();

    if (ri != 0) {
           do {
#if 1
             const char *word = ri->GetUTF8Text(level);
             float conf = ri->Confidence(level);

             if(conf >=90.0 && word != nullptr) {

               //printf("Word: %s , Confidence: %f", word, conf );

               int x1, y1, x2, y2;
               ri->BoundingBox(level, &x1, &y1, &x2, &y2);
               auto rect = cv::Rect{x1, y1, x2-x1, y2-y1};

                 text.addCharacter(*word, conf, rect);
                 //charD.Character = word; 
               //charD.Confidence = conf; 

             }
#else
        if(word != 0) {
          printf("symbol %s, conf %f", word, conf);
          tesseract::ChoiceIterator ci(*ri);
          do {
            printf("\t- ");
            const char *choice = ci.GetUTF8Text();
            printf("%s conf: %f\n", choice, ci.Confidence());
          } while(ci.Next());
        }
#endif
        delete[] word;
      } while (ri->Next(level));


    }
  }
  
  //ri->BoundingBox(level, &x1, &y1, &x2, &y2);
  //auto rect = cv::Rect{x1, y1, x2-x1, y2-y1};
#else
  //for (auto const digit : _vDigitsRects) {
  for(auto &text : _vDetectedText) {
    for(auto &c : text.Characters) {
      cv::Rect const &rect = c.Rect;
#if 1
      tessApi->SetRectangle(rect.x, rect.y, rect.width, rect.height);
      if (ri != 0) {
        const char *word = ri->GetUTF8Text(level);
        float conf = ri->Confidence(level);
        if (word != nullptr) {
          //printf("Word: %s , Confidence: %f", word, conf );
          c.Character = *word;
          c.Confidence = conf; 
        } else {
          break; 
        }
      }

#else // Scale up to fit Pix image

      float const fx{0.7f}, fy{0.7f};

      tessApi->SetRectangle(
          round(fx * rect.x),
          round(fy * rect.y),
          round(fx * rect.width),
          round(fy * rect.height));

#endif

      tessApi->Recognize(0);
      tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;//tesseract::RIL_WORD;
      tesseract::ResultIterator* ri = tessApi->GetIterator();

      bool doneOnce = false;
      if (ri != 0) {
        const char *word = ri->GetUTF8Text(level);
        float conf = ri->Confidence(level);
        do {
#if 1
          if(conf >=90.0 && word !* nullptr) {


            //printf("Word: %s , Confidence: %f", word, conf );

            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            auto rect = cv::Rect{x1, y1, x2-x1, y2-y1};

            text.addCharacter(*word, conf, rect);
            //charD.Character = word; 
            //charD.Confidence = conf; 

          }
#else
          if(word != 0) {
            printf("symbol %s, conf %f", word, conf);
            tesseract::ChoiceIterator ci(*ri);
            do {
              printf("\t- ");
              const char *choice = ci.GetUTF8Text();
              printf("%s conf: %f\n", choice, ci.Confidence());
            } while(ci.Next());
          }
#endif
          delete[] word;
        } while (ri->Next(level));

#if 0
        do {
          const char *word = ri->GetUTF8Text(level);
          float conf = ri->Confidence(level);
          //if(!doneOnce) {
          c.Character = *word;
          c.Confidence = conf; 
          //doneOnce = true; 

          //} else {
          //return; 
          //}
#if 1
          if(conf >=90.0) {
           
            int x1, y1, x2, y2;
            printf("Word: %s , Confidence: %f", word, conf );
            //charD.Character = word; 
            //charD.Confidence = conf; 
          

            //ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            //auto rect = cv::Rect{x1, y1, x2-x1, y2-y1};
          }
#else
          if(word != 0) {
            printf("symbol %s, conf %f", word, conf);
            tesseract::ChoiceIterator ci(*ri);
            do {
              printf("\t- ");
              const char *choice = ci.GetUTF8Text();
              printf("%s conf: %f\n", choice, ci.Confidence());
            } while(ci.Next());
          }
#endif
          delete[] word;
        } while (ri->Next(level));
#endif
      }
    } 
  }
#endif
};
#endif


void ImageProcessor::filterSpots(const cv::Mat &img, cv::Mat &imgOut) {
  imgOut = img.clone();
#if 1
  // Define the structuring elements
  static const auto se1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  static const auto se2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));

  // Perform closing then opening
  cv::Mat mask;
  cv::morphologyEx(img, mask, cv::MORPH_CLOSE, se1);
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, se2);

  // Filter the output
  imgOut.setTo(cv::Scalar(0), mask == 0);
#endif
};
/**
 * Rotate image.
 */
void ImageProcessor::rotate(double rotationDegrees) {
  cv::Mat M = cv::getRotationMatrix2D(cv::Point(_imgGray.cols / 2, _imgGray.rows / 2), rotationDegrees, 1);
  cv::Mat img_rotated;
  cv::warpAffine(_imgGray, img_rotated, M, _imgGray.size());
  _imgGray = img_rotated;
  if (_debugWindow) {
    cv::warpAffine(_img, img_rotated, M, _img.size());
    _img = img_rotated;
  }
}
/**
 * Rotate image.
 */

cv::Mat ImageProcessor::rotate(const cv::Mat &cImg, double rotationDegrees) {
  // cImg must be gray
  cv::Mat M = cv::getRotationMatrix2D(cv::Point(cImg.cols / 2, cImg.rows / 2), rotationDegrees, 1);
  cv::Mat img_rotated;

  cv::warpAffine( cImg, img_rotated, M, cImg.size());
  //_imgGray = img_rotated;
  if (_debugWindow) {
    //_img = img_rotated;
  }
  return img_rotated;
}


/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec2f>& lines) {
  // draw lines
  for (size_t i = 0; i < lines.size(); i++) {
    float rho = lines[i][0];
    float theta = lines[i][1];
    double a = cos(theta), b = sin(theta);
    double x0 = a * rho, y0 = b * rho;
    cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
    cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
    cv::line(_img, pt1, pt2, cv::Scalar(255, 0, 0), 1);
  }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec4i>& lines, int xoff, int yoff) {
  for (size_t i = 0; i < lines.size(); i++) {
    cv::line(_img, cv::Point(lines[i][0] + xoff, lines[i][1] + yoff),
             cv::Point(lines[i][2] + xoff, lines[i][3] + yoff), cv::Scalar(255, 0, 0), 1);
  }
}

/**
 * Detect the skew of the image by finding almost (+- 30 deg) horizontal lines.
 */
float ImageProcessor::detectSkew() {
  cv::Mat edges = cannyEdges();

  // find lines
  std::vector<cv::Vec2f> lines;
  cv::HoughLines(edges, lines, 1, CV_PI / 180.f, 140);

  // filter lines by theta and compute average
  std::vector<cv::Vec2f> filteredLines;
  float theta_min = 60.f * CV_PI / 180.f;
  float theta_max = 120.f * CV_PI / 180.0f;
  float theta_avr = 0.f;
  float theta_deg = 0.f;
  for (size_t i = 0; i < lines.size(); i++) {
    float theta = lines[i][1];
    if (theta >= theta_min && theta <= theta_max) {
      filteredLines.push_back(lines[i]);
      theta_avr += theta;
    }
  }
  if (filteredLines.size() > 0) {
    theta_avr /= filteredLines.size();
    theta_deg = (theta_avr / CV_PI * 180.f) - 90;
    printf("detectSkew: %.1f deg", theta_deg);
  } else {
    printf("failed to detect skew");
  }

  if (_debugSkew) {
    drawLines(filteredLines);
  }

  return theta_deg;
}

float ImageProcessor::detectSkew(const cv::Mat &cImg) {

  cv::Mat edges = cannyEdges(cImg);

  // find lines
  std::vector<cv::Vec2f> lines;
  cv::HoughLines(edges, lines, 1, CV_PI / 180.f, 140);

  // filter lines by theta and compute average
  std::vector<cv::Vec2f> filteredLines;
  float theta_min = 60.f * CV_PI / 180.f;
  float theta_max = 120.f * CV_PI / 180.0f;
  float theta_avr = 0.f;
  float theta_deg = 0.f;
  for (size_t i = 0; i < lines.size(); i++) {
    float theta = lines[i][1];
    if (theta >= theta_min && theta <= theta_max) {
      filteredLines.push_back(lines[i]);
      theta_avr += theta;
    }
  }
  if (filteredLines.size() > 0) {
    theta_avr /= filteredLines.size();
    theta_deg = (theta_avr / CV_PI * 180.f) - 90;
    printf("detectSkew: %.1f deg", theta_deg);
  } else {
    printf("failed to detect skew");
  }

  if (_debugSkew) {
    //TODO: Fix
    //drawLines(filteredLines);
  }

  return theta_deg;
}
/**
 * Detect edges using Canny algorithm.
 */
cv::Mat ImageProcessor::cannyEdges() {
  cv::Mat edges;
  // detect edges
  cv::Canny(_imgGray, edges, _config.getCannyThreshold1(), _config.getCannyThreshold2());
  return edges;
}

/**
 * Detect edges using Canny algorithm.
 */
cv::Mat ImageProcessor::cannyEdges(const cv::Mat &cImg) {
  cv::Mat edges;
  // detect edges
  cv::Canny(cImg, edges, _config.getCannyThreshold1(), _config.getCannyThreshold2());
  return edges;
}



/**
 * Filter contours by size of bounding rectangle.
 */

// filter contours by bounding rect size
void ImageProcessor::filterContours(Contours &contours, Rects &boundingBoxes, Contours &filteredContours) {
  for (size_t i = 0; i < contours.size(); i++) {
    cv::Rect bounds = cv::boundingRect(contours[i]);
#if 1
    if (bounds.height > _config.getDigitMinHeight() && bounds.height < _config.getDigitMaxHeight()
        && bounds.width > 5 && bounds.width < bounds.height - 5) 
#endif
    {
      boundingBoxes.push_back(bounds);
      filteredContours.push_back(contours[i]);
    }
  }
}

//#include <stdafx.h>
using namespace cv; 
using std::cerr;
using std::endl;

#define uget(x,y)    at<unsigned char>(y,x)
#define uset(x,y,v)  at<unsigned char>(y,x)=v;
#define fget(x,y)    at<float>(y,x)
#define fset(x,y,v)  at<float>(y,x)=v;

// *************************************************************
// glide a window across the image and
// create two maps: mean and standard deviation.
// *************************************************************
//#define BINARIZEWOLF_VERSION  "2.3 (February 26th, 2013)"


double calcLocalStats (Mat &im, Mat &map_m, Mat &map_s, int win_x, int win_y) {

  double m,s,max_s, sum, sum_sq, foo;
  int wxh = win_x / 2;
  int wyh = win_y / 2;
  int x_firstth = wxh;
  int y_lastth = im.rows-wyh-1;
  int y_firstth= wyh;
  double winarea = win_x*win_y;

  max_s = 0;
  for (int j = y_firstth ; j<=y_lastth; j++) 
  {
    // Calculate the initial window at the beginning of the line
    sum = sum_sq = 0;
    for (int wy=0 ; wy<win_y; wy++)
      for (int wx=0 ; wx<win_x; wx++) {
        foo = im.uget(wx,j-wyh+wy);
        sum    += foo;
        sum_sq += foo*foo;
      }
    m  = sum / winarea;
    s  = sqrt ((sum_sq - (sum*sum)/winarea)/winarea);
    if (s > max_s)
      max_s = s;
    map_m.fset(x_firstth, j, m);
    map_s.fset(x_firstth, j, s);

    // Shift the window, add and remove new/old values to the histogram
    for (int i=1 ; i <= im.cols  -win_x; i++) {

      // Remove the left old column and add the right new column
      for (int wy=0; wy<win_y; ++wy) {
        foo = im.uget(i-1,j-wyh+wy);
        sum    -= foo;
        sum_sq -= foo*foo;
        foo = im.uget(i+win_x-1,j-wyh+wy);
        sum    += foo;
        sum_sq += foo*foo;
      }
      m  = sum / winarea;
      s  = sqrt ((sum_sq - (sum*sum)/winarea)/winarea);
      if (s > max_s)
        max_s = s;
      map_m.fset(i+wxh, j, m);
      map_s.fset(i+wxh, j, s);
    }
  }

  return max_s;
}




void NiblackSauvolaWolfJolion (InputArray _src, OutputArray _dst,const BhThresholdMethod &version,int winx, int winy, double k, double dR) {

  Mat src = _src.getMat();
  Mat dst = _dst.getMat();
  double m, s, max_s;
  double th=0;
  double min_I, max_I;
  int wxh = winx/2;
  int wyh = winy/2;
  int x_firstth= wxh;
  int x_lastth = src.cols-wxh-1;
  int y_lastth = src.rows-wyh-1;
  int y_firstth= wyh;
  int mx, my;

  // Create local statistics and store them in a double matrices
  Mat map_m = Mat::zeros (src.size(), CV_32FC1);
  Mat map_s = Mat::zeros (src.size(), CV_32FC1);
  max_s = calcLocalStats (src, map_m, map_s, winx, winy);

  minMaxLoc(src, &min_I, &max_I);

  Mat thsurf (src.size(), CV_32FC1);

  // Create the threshold surface, including border processing
  // ----------------------------------------------------

  for (int j = y_firstth ; j<=y_lastth; j++) {

    // NORMAL, NON-BORDER AREA IN THE MIDDLE OF THE WINDOW:
    for (int i=0 ; i <= src.cols-winx; i++) {

      m  = map_m.fget(i+wxh, j);
      s  = map_s.fget(i+wxh, j);

      // Calculate the threshold
      switch (version) {

        case BhThresholdMethod::NIBLACK:
          th = m + k*s;
          break;

        case BhThresholdMethod::SAUVOLA:
          th = m * (1 + k*(s/dR-1));
          break;

        case BhThresholdMethod::WOLFJOLION:
          th = m + k * (s/max_s-1) * (m-min_I);
          break;

        default:
          cerr << "Unknown threshold type in ImageThresholder::surfaceNiblackImproved()\n";
          exit (1);
      }

      thsurf.fset(i+wxh,j,th);

      if (i==0) {
        // LEFT BORDER
        for (int i=0; i<=x_firstth; ++i)
          thsurf.fset(i,j,th);

        // LEFT-UPPER CORNER
        if (j==y_firstth)
          for (int u=0; u<y_firstth; ++u)
            for (int i=0; i<=x_firstth; ++i)
              thsurf.fset(i,u,th);

        // LEFT-LOWER CORNER
        if (j==y_lastth)
          for (int u=y_lastth+1; u<src.rows; ++u)
            for (int i=0; i<=x_firstth; ++i)
              thsurf.fset(i,u,th);
      }

      // UPPER BORDER
      if (j==y_firstth)
        for (int u=0; u<y_firstth; ++u)
          thsurf.fset(i+wxh,u,th);

      // LOWER BORDER
      if (j==y_lastth)
        for (int u=y_lastth+1; u<src.rows; ++u)
          thsurf.fset(i+wxh,u,th);
    }

    // RIGHT BORDER
    for (int i=x_lastth; i<src.cols; ++i)
      thsurf.fset(i,j,th);

    // RIGHT-UPPER CORNER
    if (j==y_firstth)
      for (int u=0; u<y_firstth; ++u)
        for (int i=x_lastth; i<src.cols; ++i)
          thsurf.fset(i,u,th);

    // RIGHT-LOWER CORNER
    if (j==y_lastth)
      for (int u=y_lastth+1; u<src.rows; ++u)
        for (int i=x_lastth; i<src.cols; ++i)
          thsurf.fset(i,u,th);
  }
  cerr << "surface created" << endl;


  for (int y=0; y<src.rows; ++y) 
    for (int x=0; x<src.cols; ++x) 
    {
      if (src.uget(x,y) >= thsurf.fget(x,y))
      {
        dst.uset(x,y,255);
      }
      else
      {
        dst.uset(x,y,0);
      }
    }
}

void BhThresholder::doThreshold(InputArray _src ,OutputArray _dst,const BhThresholdMethod &method)
{
  Mat src = _src.getMat();

  int winx = 0;
  int winy = 0;
  float optK=0.5;
  if (winx==0 || winy==0) {
    winy = (int) (2.0 * src.rows - 1)/3;
    winx = (int) src.cols-1 < winy ? src.cols-1 : winy;

    // if the window is too big, than we asume that the image
    // is not a single text box, but a document page: set
    // the window size to a fixed constant.
    if (winx > 100)
      winx = winy = 40;
  }

  // Threshold
  _dst.create(src.size(), CV_8UC1);
  Mat dst = _dst.getMat();

  //medianBlur(src,dst,5);
  GaussianBlur(src,dst,Size(5,5),0);
  //#define _BH_SHOW_IMAGE
#ifdef _BH_DEBUG
#define _BH_SHOW_IMAGE
#endif
  //medianBlur(src,dst,7);
  switch (method)
  {
    case BhThresholdMethod::OTSU :
      threshold(dst,dst,128,255,CV_THRESH_OTSU);
      break;
    case BhThresholdMethod::SAUVOLA :
    case BhThresholdMethod::WOLFJOLION :
      NiblackSauvolaWolfJolion (src, dst, method, winx, winy, optK, 128);
    default:
      break; 
  }

  bitwise_not(dst,dst);

}
