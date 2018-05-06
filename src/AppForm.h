#if !defined(APPFORM_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim Planstedt $
   ======================================================================== */

#define APPFORM_H
/*
  OpenCV Tutorial Youtube channel: youtube/jackyltle
  Author: Jacky Le
  Date: 01.Sep.2016
  Project: licPlaterec
  Desciption: This project support to made a Automation Car/moto plate recognition use OpenCV API.
  This source code is free & use for education purpose only
*/

#include "feature.h"

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imguifilesystem.h"  

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>
#include <dirent.h>

#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
namespace licPlaterec {

  using namespace cv;
  using namespace cv::ml;
  using std::string;
  using std::vector;

  using poppler::document;
  using poppler::page;
  using poppler::image;
  using poppler::page_renderer;
  //str2char stringTochar;
  //mat2picture mat2bmp;


  class AppForm
  {
  public:
    AppForm();
    ~AppForm();

  public:
    char character_recognition(Mat img_character);
    bool TrainSVM(string savepath, string trainImgpath);
    bool SVMPredict();

  public:
    static std::pair<std::string, bool> getFileExtension(const std::string& FileName);
    static GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);
    static cv::Mat readPDFtoCV(const std::string &fileName, int DPI=100);
  public:
    GLFWwindow *m_pGLFWwindow;
    Mat srcImg;
    std::string srcPath;

    int m_window_width  = 640;
    int m_window_height = 480;
  public:
    void run();
  private:
    void resize_callback(GLFWwindow* window, int new_width, int new_height);
    void init_opengl(int w, int h);
    void draw_frame(const cv::Mat& frame);
    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

  };
}

#include "opencv2/objdetect.hpp" // For HOGDescriptor

using cv::ml::SVM;
using cv::Mat;
using cv::ml::TrainData;

class SVMClass {
public:
  SVMClass(){};
  ~SVMClass(){};
  vector<string> list_folder(string path);
  vector<string> list_file(string folder_path);
  char character_recognition(Mat img_character);
  bool TrainSVM(string savepath, string trainImgpath);
  bool SVMPredict();

public: //Handwritten Digits
  Ptr<SVM> m_pSVM;
  void trainDigits();
  void loadDigits();
  void svmPredict(Ptr<SVM> svm, Mat &testResponse, Mat &testMat );
public:
  Mat srcImg;
  std::string resultStr;
};

#endif
