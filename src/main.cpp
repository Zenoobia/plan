#include <iostream>
#include <unistd.h>

#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

using cv::ml::SVM;
using cv::Mat;

//#include <xlnt/xlnt.hpp>
#include <libconfig.h++>

using namespace libconfig;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <stdio.h>
//#include <tchar.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cctype>

using TCHAR = wchar_t;

string tesseract_ocr(string preprocessed_file);
string tesseract_preprocess(string source_file);
void toClipboard(string s);

#include <ctime>

bool extractDate(const std::string& s, int& d, int& m, int& y){
    std::istringstream is(s);
    char delimiter;
    if (is >> d >> delimiter >> m >> delimiter >> y) {
        struct tm t = {0};
        t.tm_mday = d;
        t.tm_mon = m - 1;
        t.tm_year = y - 1900;
        t.tm_isdst = -1;

        // normalize:
        time_t when = mktime(&t);
        const struct tm *norm = localtime(&when);
        // the actual date would be:
        // m = norm->tm_mon + 1;
        // d = norm->tm_mday;
        // y = norm->tm_year;
        // e.g. 29/02/2013 would become 01/03/2013

        // validate (is the normalized date still the same?):
        return (norm->tm_mday == d    &&
                norm->tm_mon  == m - 1 &&
                norm->tm_year == y - 1900);
    }
    return false;
}

bool findD(const std::string &str, int &d, int &y) {
  std::stringstream is(str);
  char delimiter;
  if (is >> d >> delimiter >> y || is >> delimiter >> d)
    {
      return true;
    }
  return false;
}

#include <cmath>
#include <limits>
#include <iomanip>
#include <type_traits>
#include <algorithm>
 
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}
cv::Mat pix8ToMat(Pix *pix8)
{
   cv::Mat mat(cv::Size(pix8->w, pix8->h), CV_8UC1);
    uint32_t *line = pix8->data;
    for (uint32_t y = 0; y < pix8->h; ++y) {
        for (uint32_t x = 0; x < pix8->w; ++x) {
            mat.at<uchar>(y, x) = GET_DATA_BYTE(line, x);
        }
        line += pix8->wpl;
    }
    return mat;
}

int main(int argc, char** argv)
{
  const std::string dataDir = "/home/jq/projects/plan/";
  char curDir[MAX_PATH];
  getcwd(curDir, MAX_PATH);
   
  std::string source_file = dataDir + argv[1];
  //check if file was passed as variable
  if (argc < 2){
    printf("Usage: %s source_image_filepath", argv[0]);
    source_file = dataDir + "data/side_1.png";
  }

  //check if file exists
  std::ifstream infile(source_file);
  if (!infile.good()){
    printf("File not found %s\n", source_file.c_str());
    return 1;
  }

  auto *api = new tesseract::TessBaseAPI();

  if(api->Init("/usr/share/tessdata", "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }
  //api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  //api->SetVariable("tessedit_char_blacklist", "!?@#$%&*()<>_+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  //api->SetVariable("tessedit_char_whitelist", ".,-0123456789");
  //api->SetVariable("classify_bln_numeric_mode", "1");

  Pix *img = pixRead(source_file.c_str());
  api->SetImage(img);
  api->Recognize(0);

  //auto rects = api->GetBoxText(0);
  int width = pixGetWidth(img);
  int height = pixGetHeight(img);
  //cv::Mat rgb = Mat::zeros(cv::Size(width, height), CV_8UC1);

  //Pix *bit8Pix = pixConvert1To8(NULL, img, 255, 0);
  //auto rgb = pix8ToMat(bit8Pix);
  cv::Mat rgb = pix8ToMat(img);
  cv::cvtColor(rgb, rgb, CV_GRAY2RGB); 
  //cv::Mat rgb{height, width,CV_8UC3, cv::Scalar(0, 0, 0)}; 

  //cv::rectangle(rgb, {0,0, 20, 10}, {0,0,200}, 1);
  //cv::rectangle(rgb, {0,0, 200, 80}, {0,90,0}, 1);

  tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
  tesseract::ResultIterator* ri = api->GetIterator();
  std::vector<cv::Mat> subImages;
  std::vector<std::string> resultWords;
  if (ri != 0) {
    do {
      const char *word = ri->GetUTF8Text(level);
      //auto word = ri->tesseract::LTRResultIterator::GetUTF8Text(level);
      float conf = ri->Confidence(level);
      int x1, y1, x2, y2;
      ri->BoundingBox(level, &x1, &y1, &x2, &y2);
      /*
        printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
        word, conf, x1, y1, x2, y2);
        resultWords.emplace_back(word);
      */
      auto rect = cv::Rect{x1, y1, x2-x1, y2-y1};
      const double PERCENTAGE_TEXT = 0.45;

      const uint32_t REGION_WIDTH_MIN = 20;/* constraints on region size */
      const uint32_t REGION_HEIGHT_MIN = 10;/* constraints on region size */
      //const double r = (double)countNonZero
      //if (r > PERCENTAGE_TEXT && /* assume at least 45% of the area is filled if it contains text */
      /*
        if( 
        rect.height > REGION_HEIGHT_MIN // constraints on region size 
        &&
        rect.width > REGION_WIDTH_MIN
        &&
        rect.height < 80 
        &&
        rect.width < 200 &&
        conf >=90.0) */
      //      if(rect.height < 80 &&
      //  conf >= 75.0)
      /*
      float desiredRatio = (float)(133.0/85.0);
      float ratio;
      if(rect.width <= rect.height)
        ratio = ((rect.width / rect.height));
      else
        ratio = ((rect.height / rect.width));
      if(almost_equal(ratio,  desiredRatio, 0))
      */
        // these two conditions alone are not very robust. better to use something 
        // like the number of significant peaks in a horizontal projection as a third condition 
        {
	  /*
          const int bufLen = 128;
          char digits[bufLen];
          for (size_t i = 0; i <= bufLen; i++) {
          const char c = word[i];
          //for(char& c : word) {
              if (isdigit(c) || c == '1' || c == '.' || c == '-' || c == '(' || c == ')') {
                digits[i]=c;
                int d,m,y;
                if(extractDate(&c, d, m, y)) {
                  std::cout << &c << " : " << d << "-" << m << "-" << y << std::endl;
                }
                int di, year;
                if(findD(&c, di, year)) {
                  std::cout << &c << " : " << di << "." << year << std::endl;
                }
              }
          }
          if (digits[0] != 0 || (digits[0] == '.' && digits[1] == 0)) {
            subImages.emplace_back(rgb, rect);
            cv::rectangle(rgb, rect, cv::Scalar(0, 255, 0), 1);
            cv::putText(rgb, digits, cv::Point(x1, y1), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,0,0), 1.0);
            //subImages.emplace_back(rgb, rect);
            resultWords.emplace_back(digits);
            std::cout << "Digits: " << digits << std::endl;
          }
	  */
            cv::rectangle(rgb, rect, cv::Scalar(0, 255, 0), 1);

        }
    } while (ri->Next(level));
  }
  /*
  int index = 0;
  for(;index < subImages.size(); index++) {
    if(!subImages.empty() && !subImages[index].empty()) {
      //imshow(resultWords[index],subImages[index]);
      imwrite(dataDir+"/output/"+std::string(resultWords[index] + ".tif"),subImages[index]);
    }
  }*/

#if 1
  imwrite("rgb.tif", rgb);
  system("gpicview rgb.tif");
#else
  imshow("rgb", rgb);
  cv::waitKey(0);
#endif

  pixDestroy(&img);
  api->End();

  //preprocess to convert to black white book-like text
  //string preprocessed_file = tesseract_preprocess(source_file);

  //ocr and display results
  //string ocr_result = tesseract_ocr(preprocessed_file);

  //std::cout << ocr_result.c_str();

  //toClipboard(ocr_result);

  return 1;

}

string tesseract_preprocess(string source_file){
        
  //char tempPath[128];
  char  *tempPath = getenv("TMPDIR");
  //GetTempPathA(128, tempPath);
  strcat(tempPath, "preprocess_ocr.bmp");

  char preprocessed_file[MAX_PATH];
  strcpy(preprocessed_file, tempPath);


  bool perform_negate = TRUE;
  l_float32 dark_bg_threshold = 0.5f; // From 0.0 to 1.0, with 0 being all white and 1 being all black 

  int perform_scale = 1;
  l_float32 scale_factor = 3.5f;

  int perform_unsharp_mask = 1;
  l_int32 usm_halfwidth = 5;
  l_float32 usm_fract = 2.5f;

  int perform_otsu_binarize = 1;
  l_int32 otsu_sx = 2000;
  l_int32 otsu_sy = 2000;
  l_int32 otsu_smoothx = 0;
  l_int32 otsu_smoothy = 0;
  l_float32 otsu_scorefract = 0.0f;


  l_int32 status = 1;
  l_float32 border_avg = 0.0f;
  PIX *pixs = NULL;
  char *ext = NULL;


  // Read in source image 
  pixs = pixRead(source_file.c_str());


  // Convert to grayscale 
  pixs = pixConvertRGBToGray(pixs, 0.0f, 0.0f, 0.0f);


  if (perform_negate)
    {
      PIX *otsu_pixs = NULL;

      status = pixOtsuAdaptiveThreshold(pixs, otsu_sx, otsu_sy, otsu_smoothx, otsu_smoothy, otsu_scorefract, NULL, &otsu_pixs);


      // Get the average intensity of the border pixels,
      // with average of 0.0 being completely white and 1.0 being completely black. 
      border_avg = pixAverageOnLine(otsu_pixs, 0, 0, otsu_pixs->w - 1, 0, 1);                               // Top 
      border_avg += pixAverageOnLine(otsu_pixs, 0, otsu_pixs->h - 1, otsu_pixs->w - 1, otsu_pixs->h - 1, 1); // Bottom 
      border_avg += pixAverageOnLine(otsu_pixs, 0, 0, 0, otsu_pixs->h - 1, 1);                               // Left 
      border_avg += pixAverageOnLine(otsu_pixs, otsu_pixs->w - 1, 0, otsu_pixs->w - 1, otsu_pixs->h - 1, 1); // Right 
      border_avg /= 4.0f;

      pixDestroy(&otsu_pixs);

      // If background is dark 
      if (border_avg > dark_bg_threshold)
        {
          // Negate image 
          pixInvert(pixs, pixs);
        
        }
    }

  if (perform_scale)
    {
      // Scale the image (linear interpolation) 
      pixs = pixScaleGrayLI(pixs, scale_factor, scale_factor);
    }

  if (perform_unsharp_mask)
    {
      // Apply unsharp mask 
      pixs = pixUnsharpMaskingGray(pixs, usm_halfwidth, usm_fract);
    }

  if (perform_otsu_binarize)
    {
      // Binarize 
      status = pixOtsuAdaptiveThreshold(pixs, otsu_sx, otsu_sy, otsu_smoothx, otsu_smoothy, otsu_scorefract, NULL, &pixs);
    }

        
  // Write the image to file 
  status = pixWriteImpliedFormat(preprocessed_file, pixs, 0, 0);
        

  string out(preprocessed_file);

  return out;

}

string tesseract_ocr(string preprocessed_file)
{
  char *outText;
  Pix *image = pixRead(preprocessed_file.c_str());
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

        
  //TCHAR CurDir[MAX_PATH];
  //GetCurrentDirectory(MAX_PATH, CurDir);

  char CurDir[MAX_PATH];
  getcwd(CurDir, MAX_PATH);

  if(api->Init(CurDir, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }
  api->SetPageSegMode(tesseract::PSM_AUTO_OSD); //PSM_SINGLE_BLOCK PSM_AUTO_OSD

        
  api->SetImage(image);

  outText = api->GetUTF8Text();
        
  //system("cls");
  system("clear");

  string out(outText);
  return out;
}

void toClipboard(const std::string s){
  /*
    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size());
    if (!hg){
    CloseClipboard();
    return;
    }
    memcpy(GlobalLock(hg), s.c_str(), s.size());
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
  */
}


std::vector<cv::Rect> detectLetters(cv::Mat img)
{
  std::vector<cv::Rect> boundRect;
  cv::Mat img_gray, img_sobel, img_threshold, element;
  cvtColor(img, img_gray, CV_BGR2GRAY);
  cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
  cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
  element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
  cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
  std::vector< std::vector< cv::Point> > contours;
  cv::findContours(img_threshold, contours, 0, 1); 
  std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
  for( int i = 0; i < contours.size(); i++ )
    if (contours[i].size()>100)
      { 
        cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
        cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
        if (appRect.width>appRect.height) 
          boundRect.push_back(appRect);
      }
  return boundRect;
};

int main_old()
{
#if 0
  Config cfg;
  string configFileName = "../data/config.conf";

  // Read the file. If there is an error, report it and exit.
  try
    {
      cfg.readFile(configFileName.c_str());
    }
  catch(const FileIOException &fioex)
    {
      std::cerr << "I/O error while reading file." << std::endl;
      return(EXIT_FAILURE);
    }

  catch(const ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
      return(EXIT_FAILURE);
    }

  // Get the store name.
  string excelFilePath;
  try
    {
      excelFilePath = cfg.lookup("excelFilePath").c_str();
      cout << "Excel File Path: " << excelFilePath << endl << endl;
    }
  catch(const SettingNotFoundException &nfex)
    {
      cerr << "No 'excelFilePath' setting in configuration file." << endl;
    }

  xlnt::workbook wb;
  if( access( excelFilePath.c_str(), F_OK ) != -1 ) { 
    wb.load(excelFilePath);

    auto ws = wb.active_sheet();
    std::clog << "Processing spread sheet" << std::endl;
    for (auto row : ws.rows(false))
      {
        for (auto cell : row)
          {
            std::clog << cell.to_string() << std::endl;
          }
      }
    std::clog << "Processing complete" << std::endl;
  } else {
    cerr << "Could not find Excel File( " << excelFilePath << " )" << endl;
  }
#endif
  auto svm = SVM::create();
  svm->load("../data/digits_svm.yml");

  /*
    cv::HOGDescriptor hog(
    cv::Size(20,20), //winSize
    cv::Size(8,8), //blocksize
    cv::Size(4,4), //blockStride,
    cv::Size(8,8), //cellSize,
    9, //nbins,
    1, //derivAper,
    -1, //winSigma,
    0, //histogramNormType,
    0.2, //L2HysThresh,
    0,//gammal correction,
    64,//nlevels=64
    1);

  */
  cv::Mat testResponse;
  auto img1 = cv::imread("../data/side_1.png", CV_LOAD_IMAGE_GRAYSCALE);
  auto img2 = cv::imread("../data/side_2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
  if(!img1.data) return -1;
  if(!img2.data) return -1;

  //Detect
  std::vector<cv::Rect> letterBBoxes1=detectLetters(img1);
  std::vector<cv::Rect> letterBBoxes2=detectLetters(img2);
  //Display
  for(int i=0; i< letterBBoxes1.size(); i++)
    cv::rectangle(img1,letterBBoxes1[i],cv::Scalar(0,255,0),3,8,0);
  cv::imshow( "imgOut1.jpg", img1);  
  for(int i=0; i< letterBBoxes2.size(); i++)
    cv::rectangle(img2,letterBBoxes2[i],cv::Scalar(0,255,0),3,8,0);
  cv::imshow( "imgOut2.jpg", img2);  

  //cv::imshow("test", testMat);
  //cv::imshow("test", testResponse);

  //cv::waitKey(0);

  return 0;
}

std::vector< float > get_svm_detector( const cv::Ptr< SVM >& svm )
{
  // get the support vectors
  Mat sv = svm->getSupportVectors();
  const int sv_total = sv.rows;
  // get the decision function
  Mat alpha;
  Mat svidx;
  double rho = svm->getDecisionFunction( 0, alpha, svidx );

  CV_Assert( alpha.total() == 1 && svidx.total() == 1 && sv_total == 1 );
  CV_Assert( (alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
             (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f) );
  CV_Assert( sv.type() == CV_32F );

  std::vector< float > hog_detector( sv.cols + 1 );
  memcpy( &hog_detector[0], sv.ptr(), sv.cols*sizeof( hog_detector[0] ) );
  hog_detector[sv.cols] = (float)-rho;
  return hog_detector;
}

#define INPUT_FILE              "../data/side_2.jpg"
#define OUTPUT_FOLDER_PATH      string("")
#define PERCENTAGE_TEXT .45 /* assume at least 45% of the area is filled if it contains text */
#define REGION_WIDTH_MIN 8/* constraints on region size */
#define REGION_HEIGHT_MIN 2 /* constraints on region size */

#if 0
int main(int argc, char *argv[])
{
  Mat large;
  if(argc >= 2) {
    large = cv::imread(argv[1]);
  } else {
    large = cv::imread(INPUT_FILE);
  }
  Mat rgb;
  // downsample and use it for processing
  pyrDown(large, rgb);
  Mat small;
  cvtColor(rgb, small, CV_BGR2GRAY);
  // morphological gradient
  Mat grad;
  Mat morphKernel = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
  morphologyEx(small, grad, cv::MORPH_GRADIENT, morphKernel);
  // binarize
  Mat bw;
  threshold(grad, bw, 0.0, 255.0, cv::THRESH_BINARY | cv::THRESH_OTSU);
  // connect horizontally oriented regions
  Mat connected;
  morphKernel = getStructuringElement(cv::MORPH_RECT, cv::Size(9, 1));
  morphologyEx(bw, connected, cv::MORPH_CLOSE, morphKernel);
  // find contours
  Mat mask = Mat::zeros(bw.size(), CV_8UC1);
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

  // list of sub-images
  std::vector<cv::Mat> subImages;
  // filter contours
  for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
    {
      cv::Rect rect = boundingRect(contours[idx]);
      Mat maskROI(mask, rect);
      maskROI = cv::Scalar(0, 0, 0);
      // fill the contour
      drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);
      // ratio of non-zero pixels in the filled region
      double r = (double)countNonZero(maskROI)/(rect.width*rect.height);

      if (r > PERCENTAGE_TEXT /* assume at least 45% of the area is filled if it contains text */
          && 
          (rect.height > REGION_HEIGHT_MIN /* constraints on region size */
           &&
           rect.width > REGION_WIDTH_MIN
           &&
           rect.height < 100
           &&
           rect.width < 150) 
          /* these two conditions alone are not very robust. better to use something 
             like the number of significant peaks in a horizontal projection as a third condition */
          )
        {
          rectangle(rgb, rect, cv::Scalar(0, 255, 0), 2);
          subImages.emplace_back(rgb,rect);
        }
    }

  //imwrite(OUTPUT_FOLDER_PATH + string("rgb.jpg"), rgb);
  //imshow("rgb", rgb);
  auto svm = SVM::load("../data/digits_svm.yml");

  //cv::Mat img = subImages[14];

  //auto img = cv::imread(INPUT_FILE);
  auto img = cv::imread("../data/side_1.png");

  const int img_width  = img.cols;
  const int img_height = img.rows;
  int block_size = 8;
  int bin_number = 9;


  cv::HOGDescriptor hog(
                        //cv::Size(img_width,img_height), //winSize
                        cv::Size(20, 20),
                        cv::Size(8,8), //blocksize
                        cv::Size(4,4), //blockStride,
                        cv::Size(1,1), //cellSize,
                        9, //nbins,
                        1, //derivAper,
                        -1, //winSigma,
                        0, //histogramNormType,
                        0.2, //L2HysThresh,
                        0,//gammal correction,
                        64,//nlevels=64
                        1);

  std::vector< cv::Rect > detections;
  std::vector< double > foundWeights;

  std::vector<float> testHOG;
  std::vector<float> descriptors;
  hog.compute(img, descriptors);
  /*
    testHOG = descriptors;

    int descriptor_size = testHOG.size();
    cv::Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);
    for(int i = 0;i<descriptor_size;i++) {
    testMat.at<float>(i) = testHOG[i];
    }
  */
  cv::Mat testResponse;

  svm->predict(img);
  imshow( "", img );
  
  imshow("", subImages[17]);
  cv::waitKey(0);

  return 0;
}
#endif
