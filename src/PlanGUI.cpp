/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim $
   ======================================================================== */
#include "PlanGUI.h"

#include <math.h>


namespace plan {

  PlanGUI::PlanGUI(const int width, const int height) : viewportSize{width, height} {
    //cv::namedWindow( "floating window", WINDOW_AUTOSIZE );
#if 0
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
      exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_pWindow = glfwCreateWindow(width, height, "datum", NULL, NULL);
    if (!m_pWindow) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }


    glfwSetKeyCallback(m_pWindow, key_callback);
    glfwMakeContextCurrent(m_pWindow);
    //glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err)
      {
	std::cout << "GLEW initialisation error: " << glewGetErrorString(err) << std::endl;
	exit(-1);
      }
    std::cout << "GLEW okay - using version: " << glewGetString(GLEW_VERSION) << std::endl;

    init_opengl(width, height);
#endif
    //ImGui_ImplGlfw_Init(m_pWindow, true);
    //ImGui::StyleColorsDark();

  };

  PlanGUI::~PlanGUI() {
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
  };

  static cv::Mat GetKernel(int erosion_size)
  {
    cv::Mat element = getStructuringElement(cv::MORPH_CROSS,
					    cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
					    cv::Point(erosion_size, erosion_size) );
    return element;
  }

  static bool detect_stamp(cv::Mat &img, cv::Mat &result) {
    std::vector<std::vector<cv::Point> > contours;
    cv::Mat binary;
    std::vector<cv::Vec4i> hierarchy;
    cv::cvtColor(img, result, CV_GRAY2BGR);

    GaussianBlur(img,img,cv::Size(3,3),1.5,1.5);

    cv::Mat dimg;
    adaptiveThreshold(img,dimg,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,17,1);

    dilate(dimg,img,GetKernel(2)); 
    //erode(img,dimg,GetKernel(2)); 
    //erode(dimg,img,GetKernel(1)); 


    dimg = img;
    //    adaptiveThreshold(img, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 55, 5);
    //cv::findContours(dimg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    cv::findContours( dimg, contours, hierarchy,CV_RETR_TREE , CV_CHAIN_APPROX_NONE ); // Find the contours in the image

    if (contours.size() <= 0) return false;
    for (size_t i = 0; i < contours.size(); ++i) {
      double a=contourArea( contours[i],false);
      cv::Rect r = boundingRect(contours.at(i));
      //      if((r.width > 80 && r.height > 133) ||
      //       (r.width < 100 && r.height < 170)) {
      // NOTE: Magic numbers = W: 727, H: 1054, A: 41088
      if(a > 40000 && a < 42000000) {
	double arch = arcLength(contours.at(i), true);
        double squareness = 4 * M_PI * a / pow(arch,2);
	std::cout << squareness << std::endl;

	if(squareness >= 0.004 ) {
	  cv::drawContours(result, contours, i, {0, 255,0}, 2, cv::LINE_8, hierarchy );
	  cv::rectangle(result, r, cv::Scalar(255, 0, 0), 3, 8.0);
	  cv::putText(result,
		      {std::to_string(r.width)+", " + std::to_string(r.height) + ", A: " + std::to_string(a) +
			  " Squareness: " + std::to_string(squareness)},
		      {r.x,r.y}, cv::FONT_HERSHEY_PLAIN, 4, {0,0,255}, 4);
	}
	}
    }
    return true;
  };


  void PlanGUI::run() {

    cv::Mat img = cv::imread("../data/BN42_small.png", cv::IMREAD_GRAYSCALE);
    //cv::Mat img = cv::imread("../data/BN42.tif", cv::IMREAD_COLOR);
    //const ImVec4 clear_color = ImColor(114, 144, 154);
    //cv::findContours(img, contours,cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);


    cv::Mat img_small;
    //cv::resize(img, img_small, {0, 0}, 8.0, 8.0, cv::INTER_LANCZOS4);
    cv::Mat img_sobel;
    //Sobel(img_small, img_sobel, CV_8U, 1, 0, 3, 1, 0);
    cv::Mat img_result;

    cv::Canny(img, img_small, 100, 200);

    //detect_stamp(img_small, result);
    imwrite("img.png", img_small);
    imwrite("img_result.png", img_result);
    system("gpicview img_result.png");

    if(!img.empty()) {
      //GLuint image_tex = matToTexture(img, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
    }

    //glDeleteTextures(1, &image_tex);
    //glEnable(GL_TEXTURE_2D);
    //glDisable(GL_TEXTURE_2D);
    /*
      while (!glfwWindowShouldClose(m_pWindow)) {
 
      glfwGetFramebufferSize(m_pWindow, &viewportSize.width, &viewportSize.height);
      glViewport(0, 0, viewportSize.width, viewportSize.height);
      //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClearColor(120, 120, 120, 0);
      glClear(GL_COLOR_BUFFER_BIT);

      //ImGui_ImplGlfw_NewFrame();
      //ImGui::Render();

      //draw_frame(image_tex);
      //draw_frame(img);
      glfwSwapBuffers(m_pWindow);
      glfwPollEvents();    
      }
    */
    //glDeleteTextures(1, &image_tex);
  };


  // Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
  GLuint PlanGUI::matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
	magFilter == GL_LINEAR_MIPMAP_NEAREST ||
	magFilter == GL_NEAREST_MIPMAP_LINEAR ||
	magFilter == GL_NEAREST_MIPMAP_NEAREST)
      {
	std::cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << std::endl;
	magFilter = GL_LINEAR;
      }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
      {
	inputColourFormat = GL_LUMINANCE;
      }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGB,            // Internal colour format to convert to
		 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
		 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
		 0,                 // Border width in pixels (can either be 1 or 0)
		 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_UNSIGNED_BYTE,  // Image data type
		 mat.ptr());        // The actual image data itself

    //NOTE: If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
	minFilter == GL_LINEAR_MIPMAP_NEAREST ||
	minFilter == GL_NEAREST_MIPMAP_LINEAR ||
	minFilter == GL_NEAREST_MIPMAP_NEAREST)
      {
#if 1
	glEnable(GL_TEXTURE_2D);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
#else  // For older OpenGL Api
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mat.cols, mat.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, mat.ptr());
	glEnable(GL_TEXTURE_2D);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glDisable(GL_TEXTURE_2D);
#endif

      }

    glDeleteTextures(1, &textureID);
    return textureID;
  };

  void PlanGUI::error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
  }

  void PlanGUI::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  }

  void PlanGUI::resize_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, viewportSize.width = new_width, viewportSize.height = new_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, new_width, new_height, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void PlanGUI::draw_frame(const cv::Mat &frame) {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

    glEnable(GL_TEXTURE_2D);
    GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   0);
    glTexCoord2i(0, 1); glVertex2i(0,   viewportSize.height);
    glTexCoord2i(1, 1); glVertex2i(viewportSize.width, viewportSize.height);
    glTexCoord2i(1, 0); glVertex2i(viewportSize.width, 0);
    glEnd();

    glDeleteTextures(1, &image_tex);
    glDisable(GL_TEXTURE_2D);
  }

  void PlanGUI::draw_frame(const GLuint &textureID) {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

    glEnable(GL_TEXTURE_2D);
    // Draw a quad 
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   0);
    glTexCoord2i(0, 1); glVertex2i(0,   viewportSize.height);
    glTexCoord2i(1, 1); glVertex2i(viewportSize.width, viewportSize.height);
    glTexCoord2i(1, 0); glVertex2i(viewportSize.width, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
  };
#if 0
  void PlanGUI::draw_frame(const cv::Mat &frame) {
    glEnable(GL_TEXTURE_2D);
    GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
    draw_frame(image_tex);
    glDisable(GL_TEXTURE_2D);
  }
#endif

  void PlanGUI::init_opengl(int w, int h) {
    glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT

    glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);

    glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
  }

}; //end of namespace plan
