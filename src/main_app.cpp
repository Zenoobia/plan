#include "Application.h"

#include <iostream>
#include <memory>
#include <map>

#include <tesseract/baseapi.h>

namespace pl {

  static auto rectKernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
  static auto sqKernel = cv::getStructuringElement(cv::MORPH_RECT, {1, 3});

  int SZ = 20;
  float affineFlags = WARP_INVERSE_MAP|INTER_LINEAR;


  Mat deskew(Mat& img){
    Moments m = moments(img);
    if(abs(m.mu02) < 1e-2){
      return img.clone();
    }
    float skew = m.mu11/m.mu02;
    Mat warpMat = (Mat_<float>(2,3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);
    Mat imgOut = Mat::zeros(img.rows, img.cols, img.type());
    cv::warpAffine(img, imgOut, warpMat, imgOut.size(),affineFlags);

    return imgOut;
  }
  inline
  void drawText(const cv::Mat &img, const cv::String &text ,const cv::Point &ofs) {
    cv::putText(img, text, ofs, cv::FONT_HERSHEY_SIMPLEX, 0.5, {255,255,25});
  }

  static inline
  float angleBetweenLinesInRadians(const cv::Vec4i &line1, const cv::Vec4i &line2) {
    cv::Point line1Start{line1[0], line1[1]};
    cv::Point line1End{line2[2], line2[3]};
    cv::Point line2Start{line2[0], line2[1]};
    cv::Point line2End{line2[2], line2[3]};
    float angle1 = atan2(line1Start.y-line1End.y, line1Start.x-line1End.x);
    float angle2 = atan2(line2Start.y-line2End.y, line2Start.x-line2End.x);
    float result = (angle2-angle1) * 180 / 3.14;
    if (result<0) {
      result+=360;
    }
    return result;
  }
#include <limits>
//#include <iomanip>
#include <iostream>
#include <type_traits>
#include <algorithm>

  template<class T>
  typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
  almost_equal(T x, T y, int ulp)
  {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
      // unless the result is subnormal
      || std::abs(x-y) < std::numeric_limits<T>::min();
  }

  class App : public Application {
  public:

    void run() override {
      m_clearColor = ImColor(114, 144, 154);

      m_chosenPath = "../data/BN42_small.png";
      //m_chosenPath = "/home/jq/Documents/Arkiv/Arkiv/BN101444.pdf";
      //m_chosenPath = "/home/jq/Documents/Arkiv/Arkiv/BN101444.png";

      //cv::Mat stamp = cv::imread("../build-linux/subImage.png", 0);
      //bitwise_not(stamp, stamp);

      //processImage(stamp, m_stampComparison);

      while (!glfwWindowShouldClose(m_pGLFWwindow)) {
        initDraw();

        if(m_stampComparison.empty() == false) {
          //drawFrame(m_stampComparison);
        }

        draw();
        finishDraw();
      }
    }

    void filterSpots(const cv::Mat &img, cv::Mat &imgOut) {
      // Define the structuring elements
      static const auto se1 = cv::getStructuringElement(MORPH_RECT, Size(5, 5));
      static const auto se2 = cv::getStructuringElement(MORPH_RECT, Size(2, 2));

      // Perform closing then opening
      cv::Mat mask;
      cv::morphologyEx(img, mask, MORPH_CLOSE, se1);
      cv::morphologyEx(mask, mask, MORPH_OPEN, se2);

      // Filter the output
      imgOut = img.clone();
      imgOut.setTo(Scalar(0), mask == 0);
    };
#if 0
    void processImage(const cv::Mat &image, cv::Mat &imageDest)  override {
      cv::Mat dimg;
      cv::Mat imageCropped = image({150, 600, image.cols - 200, image.rows - 1150});

#if 1 // Need to convert it to CV_8UC1 to use with canny. Can be CV_8UC3 aswell, for color.
      image.convertTo(dimg, CV_8UC1);//CV_32FC1);
#endif
      cvtColor(image, imageDest, CV_GRAY2BGR);

      dimg = dimg({150, 600, image.cols - 200, image.rows - 1150});
      imageDest = imageDest({150, 600, image.cols - 200, image.rows - 1150});

      cv::GaussianBlur(dimg, dimg, {3, 3}, 0, 0);
      cv::equalizeHist(dimg, dimg);
      cv::threshold(dimg, dimg, 0, 255 , ADAPTIVE_THRESH_GAUSSIAN_C | CV_THRESH_OTSU);
      filterSpots(dimg, dimg);
#if 0

      // cv::Canny(dimg, dimg, 150, 200, 5);

      static auto baseKernel = cv::getStructuringElement(cv::MORPH_RECT, {1, 1});
      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, baseKernel);
      cv::morphologyEx(dimg, dimg, cv::MORPH_OPEN, baseKernel);
      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, sqKernel);
      //cv::bitwise_not(dimg, dimg);
#endif

#if 0
      cv::Canny(image, dimg, 150, 200, 3);
      cv::resize(dimg, dimg, {0, 0}, 2.0, 2.0, cv::INTER_LANCZOS4);
#endif
#if 0
      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, rectKernel);
      cv::morphologyEx(dimg, dimg, cv::MORPH_OPEN, rectKernel);
      cv::threshold(dimg, dimg, 0, 255 ,CV_THRESH_BINARY | CV_THRESH_OTSU);

      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, sqKernel);
#endif

#if 0
      std::vector<cv::Vec4i> lines;
      cv::HoughLinesP(dimg, lines, 1, CV_PI / 180, 20, 10, 20);

      cvtColor(dimg, dimg, CV_GRAY2BGR); // convert to BGR for debug drawing of countours
      for (size_t i = 0; i < lines.size(); i++) {
	cv::Vec4i l = lines[i];
	double angle = atan2(l[0] - l[1],l[0] - l[1]); // in radians: * 180.0 / CV_PI;
	if (!almost_equal(angle, 90.0, 0)
	    || !(angle > 80.0 && angle < 100.0)
	    || !(angle < -80.0 && angle > -100.0)
	    )
	  cv::line(dimg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 4, 6);
      }
#endif

#if 1
      cv::bitwise_not(dimg, dimg);
      std::vector<std::vector<cv::Point> > contours;
      std::vector<cv::Vec4i> hierarchy;

      cv::findContours(dimg, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

      cvtColor(dimg, dimg, CV_GRAY2BGR); // convert to BGR for debug drawing of countours
      std::vector<cv::Point> approx;
      for( size_t i = 0; i < contours.size(); i++ ) {
	// approximate contour with accuracy proportional
	// to the contour perimeter
	approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);

	cv::Rect rect = boundingRect(contours[i]);
	if(approx.size() >= 1
	   //&& isContourConvex(cv::Mat(approx))
#if 1
	   && rect.width < 80
	   && rect.height < 100
#endif
	   && (cv::contourArea(contours[i], true) > 120)
	   ) {
	  //cv::rectangle(imageDest, rect, cv::Scalar(0, 255, 0));
	  cv::rectangle(dimg, rect, cv::Scalar(0, 255, 0));

#if 0
	  const cv::Mat subImage = imageCropped(rect);
	  cv::imwrite("output/contour_" + std::to_string(i) + ".png", subImage);
#endif
	}

      }
#endif
      imageDest = dimg.clone();

    };
#endif

    void processImage(const cv::Mat &image, cv::Mat &imageDest) override {
      cvtColor(image, imageDest, CV_GRAY2BGR);
      cv::Mat dimg = image.clone();

      cv::GaussianBlur(dimg, dimg, {3, 3}, 0, 0);
      cv::equalizeHist(dimg, dimg);
      cv::threshold(dimg, dimg, 0, 255 , ADAPTIVE_THRESH_GAUSSIAN_C | CV_THRESH_OTSU);
      filterSpots(dimg, dimg);

#if 0
      static auto baseKernel = cv::getStructuringElement(cv::MORPH_RECT, {1, 1});
      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, baseKernel);
      cv::morphologyEx(dimg, dimg, cv::MORPH_OPEN, baseKernel);
      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, sqKernel);
#else
      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, rectKernel);
      cv::morphologyEx(dimg, dimg, cv::MORPH_OPEN, rectKernel);
      cv::threshold(dimg, dimg, 0, 255 ,CV_THRESH_BINARY | CV_THRESH_OTSU);

      cv::morphologyEx(dimg, dimg, cv::MORPH_CLOSE, sqKernel);
#endif
#if 1 // crop squares
      std::vector<std::vector<cv::Point> > squares;
      std::vector<cv::Rect> rects;
      findSquares(image, squares, rects);

      //cv::Mat subImage{imageDest.size(), CV_8UC3};
      cv::Mat subImage{cv::Size{0, 0}, CV_8UC3};
      //cv::Mat subImage;

      std::cout << "Rect size: " << rects.size() << std::endl;
      if(rects.size() > 0) {
        for( size_t i = 0; i < rects.size(); i++ )
          {

            cv::Rect r = rects[i];
            cv::RotatedRect rr(cv::Point2i{r.x,r.y},
                               cv::Point2i{r.x+r.width, r.y},
                               cv::Point2i{r.x+r.width,r.y+r.height});

            std::vector<cv::Point> pts { r.tl(), r.br() };

            rr.angle = atan2(pts[0].y - pts[1].y,pts[0].x - pts[1].x);// in radians: * 180.0 / CV_PI;
            std::cout << "Rect angle: " << rr.angle << std::endl;

            //dont detect the border
            if (r.x > 3 && r.y > 3) {
              //cv::rectangle(imageDest, r , Scalar(0,255,0), 1, LINE_AA); // draw the outline

              //getRotRectImg(rr, imageDest, subImage);

              //extractPatchFromOpenCVImage(imageDest, subImage, r.x, r.y, rr.angle, r.width, r.height);
              cropRotatedRect(image, subImage, rr);

              //subImage = imageDest(r);
              //DrawRotatedRectangle(imageDest, rr);
            }
            //      cv::putText(image, std::to_string(p->x), *p, 1, 1,  cv::Scalar(0,0,255), 2, LINE_AA);
          }
        //( cv::Mat& src, cv::Mat& dest, int x, int y, double angle, int width, int height);
        if(!subImage.empty()) {
          dimg = subImage;
        }
      }
      //cv::imwrite("imageDest.png", imageDest);

#endif

#if 1 // Find lines and rotate rect appropiately
      std::vector<cv::Vec4i> lines;
      cv::HoughLinesP(dimg, lines, 1, CV_PI / 180, 10, 200, 2);

      if (lines.size() > 0) {
        cvtColor(dimg, dimg, CV_GRAY2BGR); // convert to BGR for debug drawing of countours
        cv::RotatedRect rr;

        double angle;
        for (size_t i = 0; i < lines.size(); i++) {
          cv::Vec4i l = lines[i];
          angle = atan2(l[0] - l[1], l[0] - l[1]); // in radians: * 180.0 / CV_PI;
          if (!almost_equal(angle, 0.0, 0)
              || !(angle > 80.0 && angle < 100.0)
              || !(angle < -80.0 && angle > -100.0)
              )
            cv::line(dimg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 4, 6);
          std::cout << "Angle: " << angle << std::endl;

        }

        // get rotation matrix for rotating the image around its center
        cv::Point2f center(dimg.cols/2.0, dimg.rows/2.0);
        cv::Mat rot = cv::getRotationMatrix2D(center, angle*2.0, 1.0);
        // determine bounding rectangle
        cv::Rect bbox = cv::RotatedRect(center,dimg.size(), angle*2.0).boundingRect();
        // adjust transformation matrix
        rot.at<double>(0,2) += bbox.width/2.0 - center.x;
        rot.at<double>(1,2) += bbox.height/2.0 - center.y;

        cv::warpAffine(dimg, dimg, rot, bbox.size());
      }

#endif

      //cv::Canny(dimg, dimg, 150, 200, 3);
        imageDest = dimg;

    };
  };
}

int main(int argc, char *argv[]) {
  const cv::Mat img1 = imread( argv[1], cv::IMREAD_GRAYSCALE );

  //tesseract::TessBaseAPI ocr = tesseract::text::OCRTesseract::create();

  if( !img1.data)
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  pl::App app;
  app.initGLFW();
  app.loadDocument(argv[1]);
  app.run();

#if 0
  cv::Mat imageOut;
  app.m_chosenPath = argv[1];
  app.processImage(img1, imageOut);
  if(!imageOut.empty()) {
    //cv::imshow("sub image", imageOut);
    cv::waitKey(0);
  };
#endif

  return 0;
};

