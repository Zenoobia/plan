#if !defined(PLANGUI_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim Planstedt $
   ======================================================================== */

#define PLANGUI_H
#include "FileLoader.h"
/*
  #include <imgui.h>
  #include "imgui_impl_glfw.h"
  #include "imguifilesystem.h"  
*/
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace plan {

  static bool detect_stamp(cv::Mat &img, cv::Mat &result);

  using std::cout;
  using std::cerr;
  using std::endl;

  struct size2 {
    int width;
    int height;
  };

  class PlanGUI
  {
  public:
    PlanGUI(const int width, const int height);
    ~PlanGUI();
    void run();
  public:
    static GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);
    //static cv::Mat readPDFtoCV(const std::string &fileName, int DPI=100);
  private:
    GLFWwindow *m_pWindow;
    size2 viewportSize;
    //std::vector<document> m_documents;

  private:
    void resize_callback(GLFWwindow* window, int newWidth, int newHeight);
    void init_opengl(int width, int height);
    void draw_frame(const GLuint &textureID);
    void draw_frame(const cv::Mat &frame);
    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mods);


  };

}; // end of namespace plan


#endif
