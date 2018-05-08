#ifndef APPLICATION_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim Planstedt $
   ======================================================================== */

#define APPLICATION_H

#include "PDFUtils.h"
//#include "FileUtils.cpp"

#include <dirent.h>

#include <iostream>
#include <memory>

#include <imgui.h>

#include "confdefs.h"
#ifdef USE_GL3
#include <imgui_impl_glfw_gl3.h>
#else
#include <imgui_impl_glfw_gl2.h>
#endif
#include <imguifilesystem.h>

//#include <entityplus/entity.h>
//namespace ep = entityplus;
//using ep::entity_manager;
//using ep::component_list;
//using ep::tag_list;
//using ep::entity_status;


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
//#include <opencv2/xfeatures2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>
#include <dirent.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// NOTE: This breaks windows compile, i think
/*
  #ifdef _WIN32
  #undef APIENTRY
  #define GLFW_EXPOSE_NATIVE_WIN32
  #define GLFW_EXPOSE_NATIVE_WGL
  #include <GLFW/glfw3native.h>
   #endif
*/
#define INT2VOIDP(i)  (void*)(uintptr_t)(i)
#define GLUINT2TEX(i) (void*)(uintptr_t)(i)

namespace pl {


  using namespace cv;
  using namespace cv::ml;
  //using namespace cv::xfeatures2d;
  using std::string;
  using std::vector;

  using p_document = poppler::document;
  using poppler::page;
  using poppler::image;
  using poppler::page_renderer;

  enum FrameType {
    FRAME_TYPE_PDF,
    FRAME_TYPE_IMAGE,
    FRAME_TYPE_MAT,
    FRAME_TYPE_UNKNOWN
  };

  class Frame {
  public:
#if 1
    Frame(GLuint _texture,int _x, int _y, int _width, int _height) : texture(_texture), x(_x), y(_y), width(_width), height(_height) {};
    Frame(GLuint _texture,int _width, int _height, cv::Point _position={0, 0}) : texture(_texture), x(_position.x), y(_position.y), width(_width), height(_height) {};
    Frame(int _x, int _y, int _width, int _height) : x(_x), y(_y), width(_width), height(_height) {};
    Frame(int _width, int _height, cv::Point _position={0, 0}) : x(_position.x), y(_position.y), width(_width), height(_height) {};
#endif
    Frame() = default;
public:
    GLuint texture;
    FrameType type;
  public:
    int x,y;
    int width,height;
  };

  class Document {
#if 1
  public:
    Document() {
      cv::Mat mat = cv::Mat::zeros(0,0, CV_8UC1);
      Frame frame{0, 0, 0, 0};
      std::string name="";
      std::string path="";
      std::string src="";
      bool valid = false;
    };
    Document(
	     cv::Mat _mat,
	     std::string _path,
	     GLuint _texture = 0,
	     cv::Point _position = {0, 0}) : mat(_mat),
      name(basename(_path)),path(_path), src(_path),
      frame(_texture, _position.x, _position.y, mat.cols, _mat.rows)
        {
          valid = true;
        };

  Document(
           cv::Mat _mat,
           std::string _path,
           GLuint _texture = 0,
           cv::Rect _rect = {0, 0, 0, 0})
      : mat(_mat),
	name(basename(_path)),path(_path), src(_path),
	frame(_texture, _rect.x, _rect.y, _rect.width, _rect.height)
    {
      valid = true;
    }
#endif
  cv::Mat mat;
    Frame frame;//{0, 0, 0, 0};
    std::string name="";
    std::string path="";
    std::string src="";
    bool valid = false;
  };

  //using entity_manager_t = entity_manager<component_list<int, Document, Frame>, tag_list<>>;
  //using entity_t = entity_manager_t::entity_t;


  class Application
  {
  public:
    Application();
    ~Application();


  public:
    //entity_manager_t m_entityManager;
  public:
    static std::string getFileExtension(const std::string& FileName);
    static GLuint matToTexture(const cv::Mat &mat,
                               const GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
                               const GLenum magFilter = GL_LINEAR,
                               const GLenum wrapFilter = GL_CLAMP);

    static ImGuiFs::PathStringVector // TODO: Path needs to be relative (currently ignores base /)
      getFilesInPath(const std::string &path) {
      assert(ImGuiFs::DirectoryExists(path.c_str()));
      if(!ImGuiFs::DirectoryExists(path.c_str()))
        return ImGuiFs::PathStringVector{};

      std::cout << path << std::endl;

      ImGuiFs::PathStringVector files;
      ImGuiFs::DirectoryGetFiles(path.c_str(), files);

      if(files.size() <= 0)
        return ImGuiFs::PathStringVector{};
      for(auto itr = files.begin(); itr != files.end(); itr++) {
        const auto file = std::string(*itr);
        std::cout << "File" << file << std::endl;

        const std::string ext = getFileExtension(file);

        if (!(ext == "jpg" || ext == "pdf" || ext == "tif")) {
          itr = files.erase(itr);
        };
      };
      return files;
    }

    static cv::Mat readPDFtoCV(const std::string &fileName, int DPI=100, const bool forceMono=true);
  public:
    //std::shared_ptr<Document>
    bool loadDocument(const std::string &filePath, const int DPI=100);
    virtual void processImage(const cv::Mat &image, cv::Mat &imageDest);
    void drawAllDocuments();
    void drawBrowsingWindow();
  public:
    GLuint createFrame(const cv::Mat &frame);
    GLuint createFrame(const std::string &filePath);
    void drawFrame(const cv::Mat &frame);
    void drawFrame(const GLuint &tex);
    inline void drawDocument(const Document &d);
 public:
    GLFWwindow *m_pGLFWwindow;
    Mat srcImg;
    std::string srcPath;

    GLuint m_ImageTex;

    cv::Size2i m_windowSize{1600, 1024};
    int m_window_width  = 1600;
    int m_window_height = 1024;
    std::map<std::string, Document> m_docs;
 private:
    int _key = 0;
 public:
    std::string m_chosenPath;
    std::string m_loadedPath;
    std::string m_activeFolder;
    ImGuiFs::PathStringVector m_files;
    ImVec4 m_clearColor;
  public:
    cv::Mat m_stampComparison;
    int flannMatcher(cv::Mat &img_1, cv::Mat &img_2, cv::Mat &img_matches);
    void findSquares( const Mat& image, vector<vector<Point> >& squares , std::vector<cv::Rect> &rects);
    void cropRotatedRect(const cv::Mat &image, cv::Mat &imageDest, cv::RotatedRect &rr);
    void initGLFW();
    void initDraw();
    virtual void draw();
    void finishDraw();
    virtual void update();
    virtual void run();
  private:
    void resizeWindow(int const new_width, int const new_height);
    void resize_callback(GLFWwindow* window, int new_width, int new_height);
    void init_opengl(int w, int h);
    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

  };
};

#endif
