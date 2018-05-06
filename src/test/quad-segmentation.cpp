/**
 * Automatic perspective correction for quadrilateral objects. See the tutorial at
 * http://opencv-code.com/tutorials/automatic-perspective-correction-for-quadrilateral-objects/
 */
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

cv::Point2f center(0,0);

cv::Point2f computeIntersect(cv::Vec4i a, 
                             cv::Vec4i b)
{
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
	float denom;

	if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
	{
		cv::Point2f pt;
		pt.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d;
		pt.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d;
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

void sortCorners(std::vector<cv::Point2f>& corners, 
                 cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}
	corners.clear();
	
	if (top.size() == 2 && bot.size() == 2){
		cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
		cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
		cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
		cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
	
		
		corners.push_back(tl);
		corners.push_back(tr);
		corners.push_back(br);
		corners.push_back(bl);
	}
}
#include <math.h>       /* sqrt */
inline
double angle(const cv::Point &pt1,const cv::Point &pt2, const cv::Point &pt0) {
  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
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
#include <iomanip>
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

int main()
{
  cv::Mat src = cv::imread("../data/BN42_small.png");
  if (src.empty())
    return -1;

  cv::Mat bw;
  cv::cvtColor(src, bw, CV_BGR2GRAY);
  cv::blur(bw, bw, cv::Size(3, 3));
  cv::blur(bw, bw, cv::Size(3, 3));

  cv::adaptiveThreshold(bw, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 11, 2);
  cv::threshold(bw, bw, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
  cv::Canny(bw, bw, 100, 100, 3);

  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(bw, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  cv::Mat srcColored;// = cv::Mat::zeros(src.size(), CV_8UC3);
  //cvtColor(src, srcColored,cv::COLOR_GRAY2RGB);
  cvtColor(src, srcColored,CV_32FC1);

  std::vector<cv::Point> approx;
  for( size_t i = 0; i < contours.size(); i++ )
    {
      // approximate contour with accuracy proportional
      // to the contour perimeter
      approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);

      cv::Rect rect = boundingRect(contours[i]);
      if(approx.size() >= 5 &&
	 isContourConvex(cv::Mat(approx)))
	{
	  cv::rectangle(srcColored, rect, cv::Scalar(0, 255, 0));
	}

    }


  std::vector<cv::Vec4i> lines;
  //cv::HoughLinesP(bw, lines, 1, CV_PI/180, 50, 30, 12);
  
  for (size_t i = 0; i < lines.size(); i++) {
    cv::Vec4i l = lines[i];
    //cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, 2);
  }

  std::vector<cv::Point2f> corners;
  for (int i = 0; i < lines.size(); i++) {
    for (int j = i + 1; j < lines.size(); j++) {       
      cv::Point2f pt = computeIntersect(lines[i], lines[j]);
      if (pt.x >= 0 && pt.y >= 0 && pt.x < bw.cols && pt.y < bw.rows) {
	//if(almost_equal(angleBetweenLinesInRadians(lines[i], lines[j]), 350.f, 0)) {
	float angle = angleBetweenLinesInRadians(lines[i], lines[j]);
	if(angle >= 85.f && angle <= 95.f) {
	  //std::cout << angleBetweenLinesInRadians(lines[i], lines[j]) << std::endl;
	  corners.push_back(pt);
	  cv::circle(src,pt, 1, {255, 0,0}, -1);
	}
      }
    }
  }


  cv::imshow("image", srcColored);
  cv::waitKey();
  return 0;
}

