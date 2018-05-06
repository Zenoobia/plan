/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim $
   ======================================================================== */
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/ml.hpp>

using namespace cv::ml;
using namespace cv;
using namespace std;

int main()
{

  auto svm = SVM::create();
  //svm = SVM::load<SVM>("results/eyeGlassClassifierModel.yml");
  svm->load("digits_svm.yml");
  
  const std::string imgname = "../data/digits.png";
  Mat training_mat;

  Mat img_mat = imread(imgname,0); // I used 0 for greyscale
  /*
  int file_num = 0;
  int ii = 0; // Current column in training_mat
  for (int i = 0; i<img_mat.rows; i++) {
    for (int j = 0; j < img_mat.cols; j++) {
      training_mat.at<float>(file_num,ii++) = img_mat.at<uchar>(i,j);
    }
  }
  */
float * testData1D = (float*)training_mat.data;

  Mat testResponse;
  Mat mat = (Mat_<float>(1,3)<<(img_mat.data)[0],(img_mat.data)[1],(img_mat.data)[2]);
  //svm->predict(mat.reshape(1, mat.cols* mat.rows), testResponse);
  //svm->predict(mat, testResponse);
  return 0;
}
