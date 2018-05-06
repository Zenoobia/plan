/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim $
   ======================================================================== */
#include "OCR.h"

#include <iostream>
#include <dirent.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>
#include "feature.h"

namespace pl {
  using cv::ml::SVM;

  class OCR {
    Ptr<SVM> m_pSvm;

    bool TrainSVM(string savepath, string trainImgpath) {
      const int number_of_class = 30;
      const int number_of_sample = 10;
      const int number_of_feature = 32;

      //Train SVM OpenCV 3.1
      Ptr<SVM> svm = SVM::create();
      svm->setType(SVM::C_SVC);
      svm->setKernel(SVM::LINEAR);
      svm->setGamma(0.5);
      svm->setC(16);
      svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

      vector<string> folders = listFolder(trainImgpath);
      if (folders.size() <= 0)
	{
	  //do something
	  cerr << "No folders found at: " << trainImgpath << endl;
	  return false;
	}
      if (number_of_class != folders.size() || number_of_sample <= 0 || number_of_class <= 0)
	{
	  cerr << "ERROR, Number of classes/samples does not match folders provided at: " << trainImgpath << endl;
	  return false;
	}
      Mat src;
      Mat data = Mat(number_of_sample * number_of_class, number_of_feature, CV_32FC1);
      Mat label = Mat(number_of_sample * number_of_class, 1, CV_32SC1);
      int index = 0;
      std::sort(folders.begin(),folders.end());
      for (size_t i = 0; i < folders.size(); ++i)
	{
	  vector<string> files = listFile(folders.at(i));
	  if (files.size() <= 0 || files.size() != number_of_sample)
	    {
	      return false;
	    }
	  string folder_path = folders.at(i);
	  cout << "list folder" << folders.at(i) << endl;
	  string label_folder = folder_path.substr(folder_path.length() - 1);
	  for (size_t j = 0; j < files.size(); ++j)
	    {
	      src = imread(files.at(j));
	      if (src.empty())
		{
		  return false;
		}
	      vector<float> feature = calculate_feature(src);
	      for (size_t t = 0; t < feature.size(); ++t)
		data.at<float>(index, t) = feature.at(t);
	      label.at<int>(index, 0) = i;
	      index++;
	    }
	}
      // SVM Train OpenCV 3.1
      svm->trainAuto(ml::TrainData::create(data, ml::ROW_SAMPLE, label));
      svm->save(savepath);
      return true;
    }
  };


} // namespace pl
