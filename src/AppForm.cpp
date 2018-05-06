/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim $
   ======================================================================== */
#include "AppForm.h"

namespace licPlaterec {

  std::pair<std::string, bool> AppForm::getFileExtension(const std::string& FileName)
  {
    if(FileName.find_last_of(".") != std::string::npos)
      return {FileName.substr(FileName.find_last_of(".")+1), true};
    return {"", false};
  }
  cv::Mat AppForm::readPDFtoCV(const std::string& filename,int DPI) {
    document* mypdf = document::load_from_file(filename);

    if(mypdf == NULL) {
      cerr << "couldn't read pdf\n";
      return cv::Mat();
    }
    cout << "pdf has " << mypdf->pages() << " pages\n";
    page* mypage = mypdf->create_page(0);
 
    page_renderer renderer;
    renderer.set_render_hint(page_renderer::text_antialiasing);
    image myimage = renderer.render_page(mypage,DPI,DPI);
    cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";
 
    cv::Mat cvimg;
    if(myimage.format() == image::format_rgb24) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
    } else if(myimage.format() == image::format_argb32) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
    } else {
      cerr << "PDF format no good\n";
      return cv::Mat();
    }
    return cvimg;
    return cv::Mat();
  } 
  AppForm::AppForm() {
    //cv::namedWindow( "floating window", WINDOW_AUTOSIZE );

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
      exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_pGLFWwindow = glfwCreateWindow(m_window_width, m_window_height, "datum", NULL, NULL);
    if (!m_pGLFWwindow) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }


    glfwSetKeyCallback(m_pGLFWwindow, key_callback);
    glfwMakeContextCurrent(m_pGLFWwindow);
    //glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err)
      {
        cout << "GLEW initialisation error: " << glewGetErrorString(err) << endl;
        exit(-1);
      }
    cout << "GLEW okay - using version: " << glewGetString(GLEW_VERSION) << endl;

    init_opengl(m_window_width, m_window_height);

    ImGui_ImplGlfw_Init(m_pGLFWwindow, true);
    ImGui::StyleColorsDark();

  };

  AppForm::~AppForm() {
    glfwDestroyWindow(m_pGLFWwindow);
    glfwTerminate();
  };

  void AppForm::run() {

    SVMClass svm;
    if(!svm.srcImg.data )
      {
        svm.srcImg = imread("../test/0005.JPG");

        if(!svm.srcImg.data ) {
        cout <<  "Could not open or find the image: "<< "" << std::endl ;
        exit(EXIT_FAILURE);
        }
      }
    const std::string imgPath = "../data/license_plates";

    if (svm.TrainSVM("svm_test.txt",imgPath))
      {
        std::cout << "Training completed." << std::endl;
      } else {
      std::cerr << "ERROR" << std::endl;
      exit(EXIT_FAILURE);
    }

    /*
      if(svm.SVMPredict())
      {
      std::cout << "Processing completed." << std::endl;
      } else {
      std::cerr << "ERROR in SVMPredict() " << std::endl;
      exit(EXIT_FAILURE);
      }
    */

    //svm.trainDigits();
    svm.loadDigits();

    bool svm_process = false;


    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    cv::Mat testMat, testResponse;
    while (!glfwWindowShouldClose(m_pGLFWwindow)) {
 
      glfwGetFramebufferSize(m_pGLFWwindow, &m_window_width, &m_window_height);
      glViewport(0, 0, m_window_width, m_window_height);
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplGlfw_NewFrame();
      draw_frame(svm.srcImg);
      if(testResponse.data) draw_frame(testResponse);

      // 1. Show a simple window
      // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
      {
        /*
          static float f = 0.0f;
          ImGui::Text("Hello, world!");
          ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
          //ImGui::ColorEdit3("clear color", (float*)&clear_color);
          //if (ImGui::Button("Test Window")) show_test_window ^= 1;
          //if (ImGui::Button("Another Window")) show_another_window ^= 1;
          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        */
      }

      {
        const bool browseButtonPressed = ImGui::Button("Browse for image");
        static ImGuiFs::Dialog dlg;                                                     // one per dialog (and must be static)
        const char* chosenPath = dlg.chooseFileDialog(browseButtonPressed);             // see other dialog types and the full list of arguments for advanced usage

        if (strlen(chosenPath)>0) {
          // A path (chosenPath) has been chosen RIGHT NOW. However we can retrieve it later more comfortably using: dlg.getChosenPath()
          svm_process = true;
        }
        if (strlen(dlg.getChosenPath())>0) {
          ImGui::Text("Chosen file: \"%s\"",dlg.getChosenPath());
          ImGui::Text("Result from Processing: \"%s\"", svm.resultStr.c_str());
        }
        
        if(ImGui::Button("Process")) svm_process = true;

        if(svm_process) {

          static char myPath[ImGuiFs::MAX_PATH_BYTES];
          if (strlen(dlg.getChosenPath())>0) {
            strcpy(myPath,dlg.getChosenPath());

            std::string fileExt;
            bool isValid;

            tie(fileExt, isValid) = getFileExtension(myPath);
            if(isValid && fileExt == "pdf") {
              svm.srcImg = readPDFtoCV(myPath);

            } else {
            auto loadedImg = imread(myPath);
            if(!loadedImg.data )
              {
                cout <<  "Could not open or find the image: "<< myPath << std::endl ;
              } else {

              svm.srcImg = loadedImg;
              
            }
            }
          }

          svm.svmPredict(svm.m_pSVM, testResponse,svm.srcImg);
	    /*
          if(svm.SVMPredict())
            {
              std::cout << "Processing completed." << std::endl;
          ImGui::Text("Result from Processing: \"%s\"", svm.resultStr.c_str());
            } else {
            std::cerr << "ERROR in SVMPredict() " << std::endl;
            exit(EXIT_FAILURE);
          }
	    */
          svm_process = false;
        }
      }


      ImGui::Render();
      glfwSwapBuffers(m_pGLFWwindow);
      glfwPollEvents();
    }
  };
  
  // Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
  GLuint AppForm::matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
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
        cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
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

  void AppForm::error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
  }

  void AppForm::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  }

  void AppForm::resize_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, m_window_width = new_width, m_window_height = new_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, m_window_width, m_window_height, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void AppForm::draw_frame(const cv::Mat& frame) {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

    glEnable(GL_TEXTURE_2D);
    GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   0);
    glTexCoord2i(0, 1); glVertex2i(0,   m_window_height);
    glTexCoord2i(1, 1); glVertex2i(m_window_width, m_window_height);
    glTexCoord2i(1, 0); glVertex2i(m_window_width, 0);
    glEnd();

    glDeleteTextures(1, &image_tex);
    glDisable(GL_TEXTURE_2D);
  }

  void AppForm::init_opengl(int w, int h) {
    glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT

    glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);

    glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
  }

} // namespace libPlaterec

// SVMClass.cpp
std::vector<std::string> SVMClass::list_folder(std::string path)
{
  vector<string> folders;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
    {
      if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
          string folder_path = path + "/" + string(entry->d_name);
          folders.push_back(folder_path);
        }
    }
  closedir(dir);
  return folders;

}
vector<std::string> SVMClass::list_file(string folder_path)
{
  vector<string> files;
  DIR *dir = opendir(folder_path.c_str());
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
    {
      if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
          string file_path = folder_path + "/" + string(entry->d_name);
          files.push_back(file_path);
        }
    }
  closedir(dir);
  return files;
}


char SVMClass::character_recognition(Mat img_character)
{
  //Load SVM training file OpenCV 3.1
  Ptr<SVM> svmNew = SVM::create();
  svmNew = SVM::load("svm_test.txt");

  char c = '*';

  vector<float> feature = calculate_feature(img_character);
  // Open CV3.1
  Mat m = Mat(1, number_of_feature, CV_32FC1);  
  for (size_t i = 0; i < feature.size(); ++i)
    {
      float temp = feature[i];
      m.at<float>(0, i) = temp;
    }

  int ri = int(svmNew->predict(m)); // Open CV 3.1
  if (ri >= 0 && ri <= 9)
    c = (char)(ri + 48); //ma ascii 0 = 48
  if (ri >= 10 && ri < 18)
    c = (char)(ri + 55); //ma accii A = 5, --> tu A-H
  if (ri >= 18 && ri < 22)
    c = (char)(ri + 55 + 2); //K-N, bo I,J
  if (ri == 22) c = 'P';
  if (ri == 23) c = 'S';
  if (ri >= 24 && ri < 27)
    c = (char)(ri + 60); //T-V,  
  if (ri >= 27 && ri < 30)
    c = (char)(ri + 61); //X-Z

  return c;

}

bool SVMClass::TrainSVM(string savepath, string trainImgpath) {
  const int number_of_class = 30;
  const int number_of_sample = 10;
  const int number_of_feature = 32;

  //Train SVM OpenCV 3.1
  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::RBF);
  svm->setGamma(0.5);
  svm->setC(16);
  svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

  vector<string> folders = list_folder(trainImgpath);
  if (folders.size() <= 0)
    {
      //do something
      return false;
    }
  if (number_of_class != folders.size() || number_of_sample <= 0 || number_of_class <= 0)
    {
      //do something
      return false;
    }
  Mat src;
  Mat data = Mat(number_of_sample * number_of_class, number_of_feature, CV_32FC1);
  Mat label = Mat(number_of_sample * number_of_class, 1, CV_32SC1);
  int index = 0;
  for (size_t i = 0; i < folders.size(); ++i)
    {
      vector<string> files = list_file(folders.at(i));
      if (files.size() <= 0 || files.size() != number_of_sample)
        {
          return false;
        }
      string folder_path = folders.at(i);
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

bool SVMClass::SVMPredict() {
  vector<Mat> plates;
  vector<Mat> draw_character;
  vector<vector<Mat> > characters;
  vector<string> text_recognition;
  vector<double> process_time;
  void sort_character(vector<Mat>&);
  Mat image = srcImg;
  if(!image.data) return false;
  Mat gray, binary;
  vector<vector<cv::Point> > contours;
  vector<Vec4i> hierarchy;
  double t = (double)cvGetTickCount();
  cvtColor(image, gray, CV_BGR2GRAY);
  //imshow("gray", gray);
  adaptiveThreshold(gray, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 55, 5);
  //imshow("binary",binary);
  Mat or_binary = binary.clone();
  Mat element = getStructuringElement(MORPH_RECT, cv::Size(3,3));
  erode(binary, binary, element);
  dilate(binary, binary, element);
  findContours(binary, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  if (contours.size() <= 0) return false;
  for (size_t i = 0; i < contours.size(); ++i)
    {
      Rect r = boundingRect(contours.at(i));
      if (r.width > image.cols / 2 || r.height > image.cols / 2 || r.width < 120 || r.height < 20
          || (double)r.width / r.height > 4.5 || (double)r.width / r.height < 3.5)
        continue;
      Mat sub_binary = or_binary(r);
      Mat _plate = sub_binary.clone();
      vector<vector<cv::Point> > sub_contours;
      vector<Vec4i> sub_hierarchy;
      findContours(sub_binary, sub_contours, sub_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
      if (sub_contours.size() < 8) continue;
      int count = 0;
      vector<Mat> c;
      Mat sub_image = image(r);
      vector<Rect> r_characters;
      for (size_t j = 0; j < sub_contours.size(); ++j)
        {
          Rect sub_r = boundingRect(sub_contours.at(j));
          if (sub_r.height > r.height / 2 && sub_r.width < r.width / 8 && sub_r.width > 5 && r.width > 15 && sub_r.x > 5)
            {
              Mat cj = _plate(sub_r);
              double ratio = (double)count_pixel(cj) / (cj.cols*cj.rows);
              if (ratio > 0.2 && ratio < 0.7)
                {
                  r_characters.push_back(sub_r);
                  rectangle(sub_image, sub_r, Scalar(0, 0, 255), 2, 8, 0);
                }
            }
        }
      if (r_characters.size() >= 7)
        {
          // sap xep 
          for (int i = 0; i < r_characters.size() - 1; ++i)
            {
              for (int j = i + 1; j < r_characters.size(); ++j)
                {
                  Rect temp;
                  if (r_characters.at(j).x < r_characters.at(i).x)
                    {
                      temp = r_characters.at(j);
                      r_characters.at(j) = r_characters.at(i);
                      r_characters.at(i) = temp;
                    }
                }
            }
          for (int i = 0; i < r_characters.size(); ++i)
            {
              Mat cj = _plate(r_characters.at(i));
              c.push_back(cj);
            }
          characters.push_back(c);
          sub_binary = or_binary(r);
          plates.push_back(_plate);
          draw_character.push_back(sub_image);
        }
      rectangle(image, r, Scalar(0, 255, 0), 2, 8, 0);
    }

  //imshow("plate", image);
  //imshow("char", draw_character[0]);
  /*
  Mat plateImg, chaImg;
  convertScaleAbs(plates[0], plateImg);
  convertScaleAbs(draw_character[0], chaImg);
  resize(plateImg, plateImg, cv::Size(200, 50));
  resize(chaImg, chaImg, cv::Size(200, 50));
  */

  /*
    ptbPlate->Image = mat2bmp.Mat2Bimap(plateImg);
    ptbCha->Image = mat2bmp.Mat2Bimap(chaImg);
    ptbPlate->Refresh();
    ptbCha->Refresh();
  */

  // Plate recognition
  for (size_t i = 0; i < characters.size(); i++)
    {
      string result;
      for (size_t j = 0; j < characters.at(i).size(); ++j)
        {

          char cs = character_recognition(characters.at(i).at(j));
          result.push_back(cs);

        }
      text_recognition.push_back(result);
      resultStr = result;
    }
  return true;
}

string pathName = "../data/digits.png";
int SZ = 20;
float affineFlags = WARP_INVERSE_MAP|INTER_LINEAR;

Mat deskew(Mat& img){
    Moments m = moments(img);
    if(abs(m.mu02) < 1e-2){
        return img.clone();
    }
    float skew = m.mu11/m.mu02;
    Mat warpMat = (Mat_<float>(2,3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);
    Mat imgOut = Mat::zeros(img.rows, img.cols, img.type());
    warpAffine(img, imgOut, warpMat, imgOut.size(),affineFlags);

    return imgOut;
} 

void loadTrainTestLabel(string &pathName, vector<Mat> &trainCells, vector<Mat> &testCells,vector<int> &trainLabels, vector<int> &testLabels)
{

    Mat img = imread(pathName,CV_LOAD_IMAGE_GRAYSCALE);
    int ImgCount = 0;
    for(int i = 0; i < img.rows; i = i + SZ)
    {
        for(int j = 0; j < img.cols; j = j + SZ)
        {
            Mat digitImg = (img.colRange(j,j+SZ).rowRange(i,i+SZ)).clone();
            if(j < int(0.9*img.cols))
            {
                trainCells.push_back(digitImg);
            }
            else
            {
                testCells.push_back(digitImg);
            }
            ImgCount++;
        }
    }
    
    cout << "Image Count : " << ImgCount << endl;
    float digitClassNumber = 0;

    for(int z=0;z<int(0.9*ImgCount);z++){
        if(z % 450 == 0 && z != 0){
            digitClassNumber = digitClassNumber + 1;
            }
        trainLabels.push_back(digitClassNumber);
    }
    digitClassNumber = 0;
    for(int z=0;z<int(0.1*ImgCount);z++){
        if(z % 50 == 0 && z != 0){
            digitClassNumber = digitClassNumber + 1;
            }
        testLabels.push_back(digitClassNumber);
    }
}

void CreateDeskewedTrainTest(vector<Mat> &deskewedTrainCells,vector<Mat> &deskewedTestCells, vector<Mat> &trainCells, vector<Mat> &testCells){
    

    for(int i=0;i<trainCells.size();i++){

        Mat deskewedImg = deskew(trainCells[i]);
        deskewedTrainCells.push_back(deskewedImg);
    }

    for(int i=0;i<testCells.size();i++){

        Mat deskewedImg = deskew(testCells[i]);
        deskewedTestCells.push_back(deskewedImg);
    }
}


HOGDescriptor hog(
        Size(20,20), //winSize
        Size(8,8), //blocksize
        Size(4,4), //blockStride,
        Size(8,8), //cellSize,
                 9, //nbins,
                  1, //derivAper,
                 -1, //winSigma,
                  0, //histogramNormType,
                0.2, //L2HysThresh,
                  0,//gammal correction,
                  64,//nlevels=64
                  1);
void CreateTrainTestHOG(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, vector<Mat> &deskewedtrainCells, vector<Mat> &deskewedtestCells){

    for(int y=0;y<deskewedtrainCells.size();y++){
        vector<float> descriptors;
        hog.compute(deskewedtrainCells[y],descriptors);
        trainHOG.push_back(descriptors);
    }
   
    for(int y=0;y<deskewedtestCells.size();y++){
        
        vector<float> descriptors;
        hog.compute(deskewedtestCells[y],descriptors);
        testHOG.push_back(descriptors);
    } 
}
void ConvertVectortoMatrix(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, Mat &trainMat, Mat &testMat)
{

    int descriptor_size = trainHOG[0].size();
    
    for(int i = 0;i<trainHOG.size();i++){
        for(int j = 0;j<descriptor_size;j++){
           trainMat.at<float>(i,j) = trainHOG[i][j]; 
        }
    }
    for(int i = 0;i<testHOG.size();i++){
        for(int j = 0;j<descriptor_size;j++){
            testMat.at<float>(i,j) = testHOG[i][j]; 
        }
    }
}

void getSVMParams(SVM *svm)
{
    cout << "Kernel type     : " << svm->getKernelType() << endl;
    cout << "Type            : " << svm->getType() << endl;
    cout << "C               : " << svm->getC() << endl;
    cout << "Degree          : " << svm->getDegree() << endl;
    cout << "Nu              : " << svm->getNu() << endl;
    cout << "Gamma           : " << svm->getGamma() << endl;
}

Ptr<SVM> svmInit(float C, float gamma)
{
  Ptr<SVM> svm = SVM::create();
  svm->setGamma(gamma);
  svm->setC(C);
  svm->setKernel(SVM::RBF);
  svm->setType(SVM::C_SVC);

  return svm;
}

void svmTrain(Ptr<SVM> svm, Mat &trainMat, vector<int> &trainLabels)
{
  Ptr<TrainData> td = TrainData::create(trainMat, ml::ROW_SAMPLE, trainLabels);
  //svm->train(td);
  svm->trainAuto(td);
  svm->save("digits_svm.yml");
}

void SVMClass::svmPredict(Ptr<SVM> svm, Mat &testResponse, Mat &testMat )
{
  svm->predict(testMat);//, testResponse);
}

void SVMevaluate(Mat &testResponse, float &count, float &accuracy, vector<int> &testLabels)
{
  for(int i = 0; i < testResponse.rows; i++)
  {
    // cout << testResponse.at<float>(i,0) << " " << testLabels[i] << endl;
    if(testResponse.at<float>(i,0) == testLabels[i])
      count = count + 1;    
  }
  accuracy = (count/testResponse.rows)*100;
}

void SVMClass::trainDigits() {
    vector<Mat> trainCells;
    vector<Mat> testCells;
    vector<int> trainLabels;
    vector<int> testLabels;
    loadTrainTestLabel(pathName,trainCells,testCells,trainLabels,testLabels);
        
    vector<Mat> deskewedTrainCells;
    vector<Mat> deskewedTestCells;
    CreateDeskewedTrainTest(deskewedTrainCells,deskewedTestCells,trainCells,testCells);
    
    std::vector<std::vector<float> > trainHOG;
    std::vector<std::vector<float> > testHOG;
    CreateTrainTestHOG(trainHOG,testHOG,deskewedTrainCells,deskewedTestCells);

    int descriptor_size = trainHOG[0].size();
    cout << "Descriptor Size : " << descriptor_size << endl;
    
    Mat trainMat(trainHOG.size(),descriptor_size,CV_32FC1);
    Mat testMat(testHOG.size(),descriptor_size,CV_32FC1);
  
    ConvertVectortoMatrix(trainHOG,testHOG,trainMat,testMat);
    
    float C = 12.5, gamma = 0.5;

    Mat testResponse;
    m_pSVM = svmInit(C, gamma);

    ///////////  SVM Training  ////////////////
    svmTrain(m_pSVM, trainMat, trainLabels);

    ///////////  SVM Testing  ////////////////
    svmPredict(m_pSVM, testResponse, testMat); 

    ////////////// Find Accuracy   ///////////
    float count = 0;
    float accuracy = 0 ;
    getSVMParams(m_pSVM);
    SVMevaluate(testResponse, count, accuracy, testLabels);
    
    cout << "the accuracy is :" << accuracy << endl;
};

void SVMClass::loadDigits() {
  //m_pSVM->clear();
  m_pSVM->load("digits_svm.yml");
};
