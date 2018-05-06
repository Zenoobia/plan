#pragma once
#pragma once
#include <iostream>
#include <memory>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
//#include <opencv2/xfeatures2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>
#include <dirent.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <imguifilesystem.h>  


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define INT2VOIDP(i)  (void*)(uintptr_t)(i)
#define GLUINT2TEX(i) (void*)(uintptr_t)(i)


class Application
{
public:
  Application();
  ~Application();

public:
  static GLuint matToTexture(const cv::Mat &mat,
                             const GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
                             const GLenum magFilter = GL_LINEAR,
                             const GLenum wrapFilter = GL_CLAMP);

  static cv::Mat readPDFtoCV(const std::string &fileName, int DPI=100, const bool forceMono=true);
private:
  GLFWwindow *m_pGLFWwindow;
  cv::Size2i m_windowSize{1600, 1024};
  int m_window_width  = 1600;
  int m_window_height = 1024;
  ImVec4 m_clearColor;

private:

};



