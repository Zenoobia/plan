#include "AppForm.h"

#include <opencv2/opencv.hpp>

using namespace licPlaterec;

int main(int argc, char **argv) {

  
  AppForm app;
  app.srcImg = app.readPDFtoCV("../data/BN42.pdf", 50);
  if(argc >= 2) {
    app.srcImg = app.readPDFtoCV(argv[1], 100);
  }
  app.run();

    cv::waitKey(0);
  
    exit(EXIT_SUCCESS); 
};
