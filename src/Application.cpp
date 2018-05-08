#include "Application.h"
#include "ROIUtils.h"

namespace pl {

ImGui_ImplGlfwGL3_Init(m_pGLFWwindow, true);
ImGui_ImplGlfwGL3_Init(GLFWwindow , true);

inline std::string Application::getFileExtension(const std::string &FileName) {
  if(FileName.find_last_of(".") != std::string::npos)
    return {FileName.substr(FileName.find_last_of(".")), true};
  return "";
}

static
bool extractPatchFromOpenCVImage( cv::Mat& src, cv::Mat& dest, int x, int y, double angle, int width, int height) {

  // Obtain The bounding box of the desired patch
  cv::RotatedRect patchROI(cv::Point2i{x,y}, cv::Size2i(width,height), angle);
  cv::Rect boundingRect = patchROI.boundingRect();

  // check if the bounding box fits inside the image
  if ( boundingRect.x >= 0 && boundingRect.y >= 0 &&
       (boundingRect.x+boundingRect.width) < src.cols &&
       (boundingRect.y+boundingRect.height) < src.rows ) {

    // crop out the bounding rectangle from the source image
    cv::Mat preCropImg = src(boundingRect);

    // the rotational center relative tot he pre-cropped image
    int cropMidX, cropMidY;
    cropMidX = boundingRect.width/2;
    cropMidY = boundingRect.height/2;

    // obtain the affine transform that maps the patch ROI in the image to the
    // dest patch image. The dest image will be an upright version.
    cv::Mat map_mat = cv::getRotationMatrix2D(cv::Point2f(cropMidX, cropMidY), angle, 1.0f);
    map_mat.at<double>(0,2) += static_cast<double>(width/2 - cropMidX);
    map_mat.at<double>(1,2) += static_cast<double>(height/2 - cropMidY);

    // rotate the pre-cropped image. The destination image will be
    // allocated by warpAffine()
    cv::warpAffine(preCropImg, dest, map_mat, cv::Size2i(width,height));

    return true;
  } // if
  else {
    return false;
  } // else
} // extract

cv::Mat Application::readPDFtoCV(const std::string& filename,int DPI, const bool forceMono) {
  p_document* mypdf = p_document::load_from_file(filename);

  if(mypdf == NULL) {
    std::cerr << "couldn't read pdf\n";
    return cv::Mat();
  }
  std::cout << "pdf has " << mypdf->pages() << " pages\n";
  page* mypage = mypdf->create_page(0);

  page_renderer renderer;
  renderer.set_render_hint(page_renderer::text_antialiasing);
  image myimage = renderer.render_page(mypage,DPI,DPI);
  std::cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";

  cv::Mat cvimg;
  if(myimage.format() == image::format_rgb24) {
    cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
    //cv::cvtColor(cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()),cvimg, CV_BGR2GRAY);
  } else if(myimage.format() == image::format_argb32) {
    //cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
    cv::cvtColor(cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()),cvimg, CV_BGR2GRAY);
  } else if(myimage.format() == image::format_mono) {
    //cv::Mat(myimage.height(),myimage.width(),CV_8UC1,myimage.data()).copyTo(cvimg);
  } else {
    std::cerr << "PDF format no good\n";
    return cv::Mat();
  }
  return cvimg;
}

void Application::initGLFW() {
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  m_pGLFWwindow = glfwCreateWindow(m_windowSize.width, m_windowSize.height, "datum", NULL, NULL);
  if (!m_pGLFWwindow) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }


  glfwSetKeyCallback(m_pGLFWwindow, key_callback);
  glfwMakeContextCurrent(m_pGLFWwindow);

  //glfwSwapInterval(1);

  //glfwSetWindowUserPointer(m_pGLFWwindow, this);

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::cout << "GLEW initialisation error: " << glewGetErrorString(err) << std::endl;
    exit(-1);
  }
  std::cout << "GLEW okay - using version: " << glewGetString(GLEW_VERSION) << std::endl;

  init_opengl(m_windowSize.width, m_windowSize.height);

  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  ImGui_ImplGlfwGL3_Init(m_pGLFWwindow, true);
  ImGui::StyleColorsDark();

};
Application::Application() {
  //cv::namedWindow( "floating window", WINDOW_AUTOSIZE );

};

Application::~Application() {
  if(m_pGLFWwindow != nullptr) {
    glDeleteTextures(1, &m_ImageTex);
    glfwDestroyWindow(m_pGLFWwindow);
    glfwTerminate();
    ImGui_ImplGlfwGL3_Shutdown();
  }
};

void Application::drawAllDocuments() {
#if 0 // #Drawing loaded documents
  {
    m_entityManager.for_each<Document>
        (
            [this](entity_t ent, Document d) {
              assert(d.valid);
              if (d.valid ) {
                ImGui::Begin(d.name.c_str());
                {
                  drawDocument(d);
                  const bool saveImgPressed = ImGui::Button("Save as new image");

                  if (saveImgPressed) cv:imwrite(basename(d.name) + ".tif", d.mat);
                  if (ImGui::TreeNode("Selection State: Multiple Selection")) {
                    //ShowHelpMarker("Hold CTRL and click to select multiple items.");
                    static bool selection[5] = { false, false, false, false, false };
                    for (int n = 0; n < 5; n++) {
                      char buf[32];
                      sprintf(buf, "Object %d", n);
                      if (ImGui::Selectable(buf, selection[n])) {
                        if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                          memset(selection, 0, sizeof(selection));
                        selection[n] ^= 1;
                      }
                    }
                    ImGui::TreePop();
                  }
                }
                ImGui::End();
              }
            });

  }
#endif
};

void Application::update() {

};

void Application::run() {
  m_clearColor = ImColor(114, 144, 154);
  m_chosenPath = "../data/BN42_small.png";
  //m_chosenPath = "../data/BN42.tif";
  //m_chosenPath = "../data/BN425A236.pdf";
  m_activeFolder = "/home/jq/Documents/Arkiv";

  m_files = getFilesInPath(m_activeFolder);

  while (!glfwWindowShouldClose(m_pGLFWwindow)) {
    update();

    initDraw();
    draw();
    finishDraw();

  }
}

void Application::initDraw() {
  glfwGetFramebufferSize(m_pGLFWwindow, &m_windowSize.width, &m_windowSize.height);
  glViewport(0, 0, m_windowSize.width, m_windowSize.height);
  glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplGlfwGL3_NewFrame();
};

void Application::draw() {
  drawBrowsingWindow();
  drawAllDocuments();

};

void Application::finishDraw() {
  ImGui::Render();
  ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(m_pGLFWwindow);
  //glfwPollEvents();
};

// the function draws all the squares in the image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
  for( size_t i = 0; i < squares.size(); i++ )
  {
    const Point* p = &squares[i][0];

    int n = (int)squares[i].size();
    //dont detect the border
    if (p-> x > 3 && p->y > 3)
      cv::polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    cv::putText(image, std::to_string(p->x), *p, 1, 1,  cv::Scalar(0,0,255), 2, LINE_AA);
  }

  //imshow("datum", image);
}

void Application::drawBrowsingWindow() {
  ImGui::Begin("Browsing Window");
  const bool browseButtonPressed = ImGui::Button("Browse for image");
  static ImGuiFs::Dialog dlg;                                                     // one per dialog (and must be static)
  const std::string selectedPath = dlg.chooseFileDialog(browseButtonPressed);             // see other dialog types and the full list of arguments for advanced usage
  m_chosenPath = (selectedPath.length() >0 ? selectedPath : m_chosenPath);

  if (m_chosenPath.length() >0 && (m_loadedPath.empty() || m_chosenPath != m_loadedPath)) {
    //if (strlen(dlg.getChosenPath())>0) {
    std::cout << getFileExtension(m_chosenPath) << std::endl;
    ImGui::Text("Chosen file: \"%s\"",m_chosenPath.c_str());

    if(loadDocument(m_chosenPath, 100)) {
      m_loadedPath = m_chosenPath;
      m_chosenPath = "";
    }
  }
  ImGui::End();
};

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
static cv::Mat GetKernel(int erosion_size)
{
  cv::Mat element = getStructuringElement(cv::MORPH_CROSS,
                                          cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                          cv::Point(erosion_size, erosion_size) );
  return element;
}


void Application::findSquares( const Mat& image, vector<vector<Point> >& squares , std::vector<cv::Rect> &rects)
{
  squares.clear();

  //s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

  // down-scale and upscale the image to filter out the noise
  //pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
  //pyrUp(pyr, timg, image.size());


  // blur will enhance edge detection
  Mat timg(image);

  medianBlur(image, timg, 1); //org 9
  GaussianBlur(image, timg,{0,0},0.5,0.5);

  //cv::resize(image, timg, {0, 0}, 2.0, 2.0, cv::INTER_LANCZOS4);


  cv::Mat dimg(timg);
  erode(timg,dimg,GetKernel(1));
  erode(dimg,timg,GetKernel(1));
  dilate(dimg,dimg,GetKernel(1));
  timg = dimg;

  int thresh = 50;
  int N = 5;

  //cv::threshold(timg, timg, thresh, 100,CV_THRESH_BINARY);

  //Mat gray0(timg.size(), CV_8U), gray;

  vector<vector<Point> > contours;

  for( int l = 0; l < N; l++ )
  {
    // hack: use Canny instead of zero threshold level.
    // Canny helps to catch squares with gradient shading
    if( l == 0 )
    {
      // apply Canny. Take the upper threshold from slider
      // and set the lower to 0 (which forces edges merging)
      Canny(timg, timg, 5, thresh, 5);
      // dilate canny output to remove potential
      // holes between edge segments
      dilate(timg, timg, Mat(), Point(-1,-1));
    }
    else
    {
      // apply threshold if l!=0:
      //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
      timg = timg >= (l+1)*255/N;
    }

    // find contours and store them all as a list
    findContours(timg, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<Point> approx;

    // test each contour
    for( size_t i = 0; i < contours.size(); i++ )
    {
      // approximate contour with accuracy proportional
      // to the contour perimeter
      approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

      // square contours should have 4 vertices after approximation
      // relatively large area (to filter out noisy contours)
      // and be convex.
      // Note: absolute value of an area is used because
      // area may be positive or negative - in accordance with the
      // contour orientation
      auto rect = boundingRect(contours[i]);
      if( approx.size() >= 1
#if 1
          &&
          fabs(contourArea(Mat(approx))) > 2000 &&
          //fabs(contourArea(Mat(approx))) < 3000 &&
          isContourConvex(Mat(approx))
#endif
#if 1
          &&
          (rect.width <= 200 && rect.height <= 200) &&
          (rect.width >= 50 && rect.height >= 50)
#endif
          )
      {
        double maxCosine = 0;

        for( int j = 2; j < 5; j++ )
        {
          // find the maximum cosine of the angle between joint edges
          double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
          maxCosine = MAX(maxCosine, cosine);
        }

        // if cosines of all angles are small
        // (all angles are ~90 degree) then write quandrange
        // vertices to resultant sequence
        if( maxCosine <= 0.9 ) { // Org: .3
          squares.push_back(approx);
          rects.push_back(rect);
        }
        std::cout << maxCosine << std::endl;
      }
    }
  }
};

static
      void getRectangles(const cv::Mat srcImg, cv::Mat &destImg) {
#if 0
      std::vector<std::vector<cv::Point> > contours;
      cv::Mat img = cv::Mat::zeros(srcImg.size(), CV_8UC1);
      cv::bitwise_not(srcImg, img);
      cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
      //cv::findContours(srcImg, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
      std::vector<cv::Point> approx;

      for( size_t i = 0; i < contours.size(); i++ )
        {
          // approximate contour with accuracy proportional
          // to the contour perimeter
          approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);

          cv::Rect rect = boundingRect(contours[i]);
          if(approx.size() >= 1 &&
             isContourConvex(cv::Mat(approx)))
            {
              cv::rectangle(destImg, rect, cv::Scalar(0,0,255));
            }
        }
#else
      //std::vector<std::vector<cv::Point> > squares;
      //findSquares(srcImg, squares);
      //drawSquares(destImg, squares);

#endif

    }

  void Application::cropRotatedRect(const cv::Mat &image, cv::Mat &imageDest, cv::RotatedRect &rr) {
    //cropRotatedRect(image, imageDest, rr.center, rr.size, rr.angle

    Mat M, rotated;
    // get angle and size from the bounding box
    float angle = rr.angle;
    Size rect_size = rr.size;



    // thanks to http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
    if (rr.angle < -45.) {
      angle += 90.0;
      swap(rect_size.width, rect_size.height);
    }

    if(rect_size.width < rect_size.height) {
      angle -= 90.0;
      swap(rect_size.width, rect_size.height);
    }

    // get the rotation matrix
    M = getRotationMatrix2D(rr.center, angle, 1.0);
    // perform the affine transformation
    warpAffine(image, rotated, M, image.size(), INTER_CUBIC);

    //imageDest = rotated;
    // crop the resulting image
    getRectSubPix(rotated, rect_size, rr.center, imageDest);
  }

  // Include center point of your rectangle, size of your rectangle and the degrees of rotation
  static void DrawRotatedRectangle(cv::Mat& image, cv::Point centerPoint, cv::Size rectangleSize, double rotationDegrees) {
    cv::Scalar color = cv::Scalar(255, 0, 0);

    // Create the rotated rectangle
    cv::RotatedRect rotatedRectangle(centerPoint, rectangleSize, rotationDegrees);

    // We take the edges that OpenCV calculated for us
    cv::Point2f vertices2f[4];
    rotatedRectangle.points(vertices2f);

    // Convert them so we can use them in a fillConvexPoly
    cv::Point vertices[4];
    for(int i = 0; i < 4; ++i){
      vertices[i] = vertices2f[i];
    }

    // Now we can fill the rotated rectangle with our specified color
    cv::fillConvexPoly(image,
                       vertices,
                       4,
                       color);
  }

  static inline
  void DrawRotatedRectangle(cv::Mat& image, cv::RotatedRect &rr) {
    DrawRotatedRectangle(image, rr.center, rr.size, rr.angle);
  }

#if 0
  bool Application::loadDocument(const std::string &path, const int DPI) {

    FrameType frameType = FRAME_TYPE_UNKNOWN;
    const std::string ext = getFileExtension(path);
    if(ext == "pdf") {
      const cv::Mat image = readPDFtoCV(path, DPI);
      frameType = FRAME_TYPE_PDF;
    }
    frameType = FRAME_TYPE_IMAGE; //TODO: Case when extension is other than a supported image
    const cv::Mat image = cv::imread(path, 0); // Force greyscale


    //cv::Mat imageDest = cv::Mat::zeros(image.size(), CV_32FC1);
    cv::Mat imageDest = image;
    processImage(image, imageDest);

    if(imageDest.empty())
      return false;
    //cv::resize(imageDest, imageDest, {0, 0}, 2.0, 2.0, cv::INTER_LANCZOS4);

    //TODO: maybe don't need GL calls here, maybe move to function matTotexture.
    glEnable(GL_TEXTURE_2D);
    // GLuint tex = matToTexture(image);
    GLuint tex = matToTexture(imageDest);
    glDisable(GL_TEXTURE_2D);

    auto ent = m_entityManager.create_entity(Document{
        image, path, tex,
          {0, 0,
              int(image.cols*.7),
              int(image.rows*.7)
              }});
    return (ent.get_status() == ep::entity_status::OK);
  }
#endif
  void Application::processImage(const cv::Mat &image, cv::Mat &imageDest) {
#if 0
    image.convertTo(imageDest, CV_8UC4);//CV_32FC1);
#else
    cvtColor(image, imageDest, CV_GRAY2BGR);
#endif

    // getRectangles(src, &dest);
    std::vector<std::vector<cv::Point> > squares;
    std::vector<cv::Rect> rects;
    findSquares(image, squares, rects);
    //drawSquares(imageDest, squares);

#if 0
    cv::Mat ex = cv::imread("/home/jq/Documents/Arkiv/Ex/Ansokan1.tif", 0);
    flannMatcher(ex,image, imageDest);

#endif
#if 0
    //cv::Mat matLines{image.cols, image.rows, image.data, CV_32FC1, 1};;
    image.convertTo(imageDest, CV_8UC4);//CV_32FC1);
    cv::Canny(image, image, 150, 200, 3);


    //cv::bitwise_not(image, image);
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(image, lines, 1, CV_PI / 180, 200, 10, 20);

    for (size_t i = 0; i < lines.size(); i++)
      {
        cv::Vec4i l = lines[i];
        cv::line(imageDest, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, 2);
      }
#endif
#if 1
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
            cropRotatedRect(imageDest, subImage, rr);

            //subImage = imageDest(r);
            //DrawRotatedRectangle(imageDest, rr);
          }
          //      cv::putText(image, std::to_string(p->x), *p, 1, 1,  cv::Scalar(0,0,255), 2, LINE_AA);
        }
      //( cv::Mat& src, cv::Mat& dest, int x, int y, double angle, int width, int height);
      if(!subImage.empty()) {
        std::cout << "saved image as: " << basename(m_chosenPath) + ".png" << std::endl;
        cv::imwrite(basename(m_chosenPath) + ".png", subImage);
        imageDest = subImage;
      } else {
        imageDest.release();
      }
    }
    //cv::imwrite("imageDest.png", imageDest);
#endif
  };

  // Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
  GLuint Application::matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);
    std::cout << "texture ID :" << textureID << std::endl;

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

    //use fast 4-byte alignment (default anyway) if possible, otherwise use 1-byte for Grayscale image
    // NOTE: Seems to use 4-bytes for PDF files loaded with readPDftocv()
    // required for PDF to align correctly
    glPixelStorei(GL_UNPACK_ALIGNMENT, (mat.step & 3) ? 1 : 4);

    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
      {
        inputColourFormat = GL_LUMINANCE;
      }

    //inputColourFormat = GL_LUMINANCE;
    //inputColourFormat = GL_RGB8;
    //std::cout << "mat channels = " << mat.channels() << std::endl;

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB8,//GL_LUMINANCE, //GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_NEAREST_MIPMAP_NEAREST)
      {
        glGenerateMipmap(GL_TEXTURE_2D);
      }

    return textureID;
  }

  void Application::error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
  }

  void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      return;
    }
    //_key = key;
  }

void Application::resizeWindow(int const new_width, int const new_height) {
  //TODO: Implement something maybe
}


void Application::resize_callback(GLFWwindow* window, int new_width, int new_height) {
  glViewport(0, 0, m_windowSize.width = new_width, m_windowSize.height = new_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, m_windowSize.width, m_windowSize.height, 0.0, 0.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  // TODO: Implement gui class
  //auto g = static_cast<Application *>(glfwGetWindowUserPointer(m_pGLFWwindow));
  //g->resizeWindow(new_width, new_height);
}

GLuint Application::createFrame(const cv::Mat &frame) {
  glEnable(GL_TEXTURE_2D);
  GLuint result = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
  glDisable(GL_TEXTURE_2D);
  return result;
};

GLuint Application::createFrame(const std::string &filePath) {

  const std::string ext = getFileExtension(filePath);
  std::cout << ext << std::endl;
  if(ext == "pdf") {
    return createFrame(readPDFtoCV(filePath));
  }
  return createFrame(cv::imread(filePath, 0));
  // TODO: Any unsupported image extension is an error
  //std::cerr << "Cannot load file :" << filePath << " (Unknown Extension) -> " << getFileExtension(filePath) << std::endl;

};

inline
        void Application::drawDocument(const Document &d) {
  ImGui::Image(GLUINT2TEX(d.frame.texture), ImVec2(d.frame.width, d.frame.height));
  //, ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
    };
    // Deprecated
#if 1
    void Application::drawFrame(const cv::Mat& frame) {

      // Clear color and depth buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

      glEnable(GL_TEXTURE_2D);
      GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

      ImGui::Image(GLUINT2TEX(image_tex), ImVec2(frame.cols, frame.rows));
#if 0
      /* Draw a quad */
      glBegin(GL_QUADS);
      glTexCoord2i(0, 0); glVertex2i(0,   0);
      glTexCoord2i(0, 1); glVertex2i(0,   m_windowSize.height);
      glTexCoord2i(1, 1); glVertex2i(m_windowSize.width, m_windowSize.height);
      glTexCoord2i(1, 0); glVertex2i(m_windowSize.width, 0);
      glEnd();

      glDeleteTextures(1, &image_tex);
#endif
      glDisable(GL_TEXTURE_2D);
    }
#endif

  void Application::drawFrame(const GLuint &tex) {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

#if 0
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    //GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

    // Draw a quad
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   0);
    glTexCoord2i(0, 1); glVertex2i(0,   m_windowSize.height);
    glTexCoord2i(1, 1); glVertex2i(m_windowSize.width, m_windowSize.height);
    glTexCoord2i(1, 0); glVertex2i(m_windowSize.width, 0);
    glEnd();

#else
    //match projection to window resolution (could be in reshape callback)
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
    //clear and draw quad with texture (could be in display callback)
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(100, 100);
    glTexCoord2i(0, 1); glVertex2i(100, 500);
    glTexCoord2i(1, 1); glVertex2i(500, 500);
    glTexCoord2i(1, 0); glVertex2i(500, 100);
    glEnd();
#endif
    glDisable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, 0);
    glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers
    //glDeleteTextures(1, &tex);
    glDisable(GL_TEXTURE_2D);
  }

  void Application::init_opengl(int w, int h) {
    glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT

    glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);

    glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
  }

  /*
    int Application::flannMatcher(cv::Mat &img_1, cv::Mat &img_2, cv::Mat &img_matches) {
    if( !img_1.data || !img_2.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    //SurfFeatureDetector detector( minHessian );
    Ptr<SURF> detector = SURF::create(minHessian);

    std::vector<KeyPoint> keypoints_1, keypoints_2;

    detector->detect( img_1, keypoints_1 );
    detector->detect( img_2, keypoints_2 );

    //-- Step 2: Calculate descriptors (feature vectors)
    //SurfDescriptorExtractor extractor;
    Ptr<SURF> extractor = SURF::create();

    Mat descriptors_1, descriptors_2;

    extractor->compute( img_1, keypoints_1, descriptors_1 );
    extractor->compute( img_2, keypoints_2, descriptors_2 );

    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );

    double max_dist = 0;
    double min_dist = 1;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_1.rows; i++ )
    { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
    //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
    //-- small)
    //-- PS.- radiusMatch can also be used here.
    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_1.rows; i++ )
    { if( matches[i].distance <= max(2*min_dist, 0.02) )
    { good_matches.push_back( matches[i]); }
    }

    //-- Draw only "good" matches
    //Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
    good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
    vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Show detected matches
    //imshow( "Good Matches", img_matches );

    for( int i = 0; i < (int)good_matches.size(); i++ )
    { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx );
    }



    };
  */
}
