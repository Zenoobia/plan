#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void getQuadrangleSubPix_8u32f_CnR( const uchar* src, size_t src_step, Size src_size,
                                    float* dst, size_t dst_step, Size win_size,
                                    const double *matrix, int cn );

void myGetQuadrangleSubPix(const Mat& src, Mat& dst,Mat& m );
void getRotRectImg(cv::RotatedRect rr,Mat &img,Mat& dst);


