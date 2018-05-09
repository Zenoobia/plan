
#include "TextDetection.h"

TextDetection::TextDetection(Config const &config, ImageProcessor *pImageProcessor) : _config(config), _pImageProcessor(pImageProcessor) {
  _pTessApi = new tesseract::TessBaseAPI();
  // TODO: Find tessdata location in configure.ac
  //#define TESSDATA_PREFIX "/usr/share"
  //#define TESSDATA_PREFIX "/mingw64/share"
#define TESSDATA_PREFIX "../data"
#define TESSDATA_LANG "swe"

  if(_pTessApi->Init(TESSDATA_PREFIX"/tessdata", TESSDATA_LANG, tesseract::OEM_DEFAULT
                     )) {
    fprintf(stderr, "Could not initialize tesseract.\n");
  }
  //_pTessApi->SetVariable("tessedit_char_whitelist","0123456789.-_/\\");
  //_pTessApi->SetVariable("load_system_dawg", "F");

  //_pTessApi->SetVariable("load_freq_dawg", "F"); 
  //_pTessApi->SetVariable("user_words_suffix", "user-words"); 
  //_pTessApi->SetVariable("user_patterns_suffix", "user-patterns"); 
};

TextDetection::~TextDetection() noexcept {
  _pTessApi->End();
}

void TextDetection::setInput(ImageInput *imageInput) {
  _pImageInput = imageInput;
}

void TextDetection::saveData(std::string name) const noexcept {
  if(name == "")
    name = _pImageInput->getImageName();

  cv::FileStorage fs(name + ".yml", cv::FileStorage::WRITE);

  //fs << "detected_characters" << _vTextLines;

  //  fs << "TextLines" << _vTextLines;

  fs << "number_of_textlines" << (int)_vTextLines.size();
  fs << "TextLines" << "[";
  for(auto i : indices(_vTextLines)) {
    auto &textLine = _vTextLines[i];
    fs << "{";
    fs << "number_of_symbols" << (int)textLine.vSymbols.size();
    fs << "Symbols" << "[";
    for(auto const &symbol : textLine.vSymbols) {
      fs << "{";
      fs << "number_of_chars" << (int)symbol.vDetectedChars.size(); 
      fs << "Characters" << "[";
      for(auto j : indices(symbol.vDetectedChars)) {
        fs << "{";
        fs << "characters" << (char)symbol.vDetectedChars[j];
        fs << "confidence" << symbol.vConfidence[j];
        fs << "rects" << symbol.vDetectedRects[j];
        fs << "}"; 
      }
      fs << "]"; 
      fs << "}"; 
    }

    fs << "]";
    fs << "}";
  }
  fs << "]";
  fs.release();
}
bool TextDetection::loadData() noexcept {

  cv::FileStorage fs(_pImageInput->getImageName() + ".yml", cv::FileStorage::READ);
  if(fs.isOpened()) {
#if 1

    int nTextLines = fs["number_of_textlines"];
    _vTextLines.resize(nTextLines);
    cv::FileNode fileNode = fs ["TextLines"];

    int textLine{0};
    for(auto const &item : fileNode) {
      _vTextLines[textLine].vSymbols.resize((int)item["number_of_symbols"]);
      cv::FileNode symbolNode = item["Symbols"];
      int symbolIndex{0};
      for(auto const &symbolItem : symbolNode) {
        for(auto const &charItem : symbolItem["Characters"]) {
          cv::Rect rect;
          rect.x = (int)charItem["rects"][0];
          rect.y = (int)charItem["rects"][1];
          rect.width = (int)charItem["rects"][2];
          rect.height = (int)charItem["rects"][3];
          if(rect.width == 0 || rect.height == 0) {
            continue;
          }
          _vTextLines[textLine].vSymbols[symbolIndex].vDetectedRects.emplace_back(rect);



          std::cout << "Character: " << (char)(int)charItem["characters"] << std::endl;
          _vTextLines[textLine].vSymbols[symbolIndex].vDetectedChars.emplace_back((char)(int)charItem["characters"]);
          _vTextLines[textLine].vSymbols[symbolIndex].vConfidence.emplace_back((float)charItem["confidence"]);
            }
        symbolIndex++;
      }
      textLine++;
    }
    assert(textLine == nTextLines); 
    return true;

    //saveData("TestData");

    //_vTextLines.clear();
    //return false; 
#else
    //fs["TextLines"]  >> _vTextLines;
    int nTextLines = 0;
    fs["number_of_textlines"] >> nTextLines;
    _vTextLines.resize(nTextLines);

    cv::FileNode fileNode = fs["TextLines"];

    int textLineIndex = 0;
    for (auto const &item : fileNode) {
      cv::FileNode symbolNode  = item["Symbols"];
      int nSymbols = item["number_of_symbols"];
      _vTextLines[textLineIndex].vSymbols.resize(nSymbols);
      int symbolIndex = 0;
      for(auto const &symbolItem : symbolNode) {
        _vTextLines[textLineIndex].vSymbols[symbolIndex].read(symbolItem);
        symbolIndex++;
      }
      textLineIndex++;
    }

#endif
    fs.release();
  }
  return false;
}

std::vector<TextLine> TextDetection::getTextLines() const noexcept {
  return _vTextLines; 
}

void TextDetection::process(bool const loadCached) {
  _pTessApi->Clear();
  _pTessApi->SetImage(_pImageInput->getPix());
  _vTextLines.clear();

  if(loadCached && loadData()) {
    std::cout << "Found Cache for " << _pImageInput->getImageName() << std::endl;
    return;
  } else {
    auto digitRects = findCounterDigits(_pImageInput->getImage(), 0, 0);

    for(auto const &rect : digitRects) {
      _vTextLines.emplace_back(rect);
      //cv::rectangle(_pImageInput->getImage(), rect, cv::Scalar(255));
      //cv::rectangle(_img, rect, cv::Scalar(255));
    }



    // After filling with potential Rects
    for(auto &textLine : _vTextLines) {
      cv::rectangle(_img, textLine.rect, {255,0,0}, 1);
      std::vector<Symbol> detectedCharacters;

      cv::Rect const &rect = textLine.rect;
      _pTessApi->SetRectangle(rect.x, rect.y, rect.width, rect.height);
      _pTessApi->Recognize(0);
      tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;//tesseract::RIL_WORD;
      tesseract::ResultIterator* ri = _pTessApi->GetIterator();

      char *lastWord = nullptr;

      if (ri != 0) {
        do {
          char *word = ri->GetUTF8Text(level);
          float conf = ri->Confidence(level);

          //          if(word != 0) {
          if(conf >=90.0 && word != 0 && word != nullptr && word != lastWord) {
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            auto characterRect = cv::Rect{x1, y1, x2-x1, y2-y1};

            printf("symbol %s, conf %f", word, conf);
            //detectedCharacters.emplace_back(conf,word, ); 
            textLine.addSymbol(conf, *word, characterRect); 

            tesseract::ChoiceIterator ci(*ri);
            do {
              if(ci.GetUTF8Text() != word) {
                printf("\t- ");
                const char *choice = ci.GetUTF8Text();
                printf("%s conf: %f\n", choice, ci.Confidence());
                //      textLine.addSymbol(ci.Confidence(), *choice, characterRect); 
              }
            } while(ci.Next());
          }
          lastWord = word;
          delete[] word;
        } while (ri->Next(level));
      }
    }
                                    saveData();
  }
}

std::vector<cv::Rect> TextDetection::findTextLines() {

  std::vector<cv::Rect> boundRect;
  cv::Mat img_gray, img_sobel, img_threshold, element;
  // No need to convert to gray
  //cvtColor(_img, img_gray, CV_BGR2GRAY);
  img_gray = _img;

  cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
  cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);

  element = getStructuringElement(cv::MORPH_RECT, cv::Size(30, 30) );

  cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
  std::vector< std::vector< cv::Point> > contours;
  //cv::findContours(img_threshold, contours, 1, 0);
  cv::findContours(img_threshold, contours,
                   CV_RETR_EXTERNAL//CV_RETR_LIST CV_RETR_EXTERNAL
                   ,CV_CHAIN_APPROX_NONE);

  std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
  for( int i = 0; i < contours.size(); i++ )
     if (contours[i].size()>200)
     {
       cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
       cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
       //if (appRect.width>appRect.height)
       boundRect.push_back(appRect);
       cv::rectangle(_img, appRect, cv::Scalar{0,0,255}, 2);
     }
  return boundRect;
}


/**
 * Find bounding boxes that are aligned at y position.
 */
void TextDetection::findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
                                     std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result) {
  std::vector<cv::Rect>::const_iterator it = begin;
  cv::Rect start = *it;
  ++it;
  result.push_back(start);

  for (; it != end; ++it) {
    if (abs(start.y - it->y) < _config.getDigitYAlignment() && abs(start.height - it->height) < 5) {
      result.push_back(*it);
    }
  }
}

/**
 * Find bounding boxes that are aligned at y position.
 */
Rects TextDetection::findAlignedBoxes(Rects::const_iterator begin,
                                      Rects::const_iterator end) {
  Rects::const_iterator it = begin;
  cv::Rect start = *it;
  ++it;
  Rects result{0};
  result.push_back(start);

  for (; it != end; ++it) {
    if (abs(start.y - it->y) < _config.getDigitYAlignment() && abs(start.height - it->height) < 5) {
      result.push_back(*it);
    }
  }
  return result;
}

std::vector<cv::Rect> TextDetection::findCounterDigits(const cv::Mat &cImg, const int xOffset, const int yOffset) {
#if 1
      cv::Mat edges = _pImageProcessor->cannyEdges(cImg);

      cv::Mat img_ret = edges.clone();

      // find contours in whole image
      Contours contours, filteredContours;
      Rects boundingBoxes;
      cv::findContours(edges, contours,
                       CV_RETR_LIST// CV_RETR_{EXTERNAL,LIST}
                       , CV_CHAIN_APPROX_NONE);

      // filter contours by bounding rect size
      _pImageProcessor->filterContours(contours, boundingBoxes, filteredContours);

      //  std::cout << "number of filtered contours: " << filteredContours.size();

#if 0 // find bounding boxes that are aligned at y position //TODO: Not working
      Rects  alignedBoundingBoxes, tmpRes;
      for (Rects::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
        tmpRes.clear();
        tmpRes = findAlignedBoxes(ib, boundingBoxes.end());
        if (tmpRes.size() > alignedBoundingBoxes.size()) {
          alignedBoundingBoxes = tmpRes;
        }
      }
#else
  Rects alignedBoundingBoxes = boundingBoxes;
#endif
  //std::cout << "max number of alignedBoxes: " << alignedBoundingBoxes.size();

  // sort bounding boxes from left to right
  std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());

#if 0 // Not working
  // draw contours
  //cv::Mat cont = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
  // cv::drawContours(_img, cont, 1, cv::Scalar(255));
  //_img = cont; 

  cv::drawContours(_img, contours, 2, cv::Scalar(255));
#endif

  return alignedBoundingBoxes;
#endif
}


#if 0 // Deprecated
/**
 * Find and isolate the digits of the counter,
 */
void TextDetection::findCounterDigits() {
  // edge image
  cv::Mat edges = cannyEdges();
  if (_debugEdges) {
    //cv::imshow("edges", edges);
  }

  cv::Mat img_ret = edges.clone();

  // find contours in whole image
  std::vector<std::vector<cv::Point> > contours, filteredContours;
  std::vector<cv::Rect> boundingBoxes;
  cv::findContours(edges, contours,
                   CV_RETR_EXTERNAL//CV_RETR_LIST CV_RETR_EXTERNAL
                   ,CV_CHAIN_APPROX_NONE);

  // TODO: copy-pasta 
#if 0
  for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
  {
    double a=cv::contourArea( contours[i],false);  //  Find the area of contour
    if(a>largest_area){
      largest_area=a;
      largest_contour_index=i;                //Store the index of largest contour
      boundingBoxes.emplace_back(boundingRect(contours[i])); // Find the bounding rectangle for biggest contour
    } 
  }
#endif

  // filter contours by bounding rect size
  filterContours(contours, boundingBoxes, filteredContours);

  //std::cout << "number of filtered contours: " << filteredContours.size() << std::endl;

  // find bounding boxes that are aligned at y position
  std::vector<cv::Rect> alignedBoundingBoxes, tmpRes;
  for (std::vector<cv::Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
    tmpRes.clear();
    findAlignedBoxes(ib, boundingBoxes.end(), tmpRes);
    if (tmpRes.size() > alignedBoundingBoxes.size()) {
      alignedBoundingBoxes = tmpRes;
    }
  }
//std::cout << "max number of alignedBoxes: " << alignedBoundingBoxes.size() << std::endl;

// sort bounding boxes from left to right

//std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());

if (_debugEdges) {
  // draw contours
  cv::Mat cont = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
  cv::drawContours(cont, filteredContours, -1, cv::Scalar(255));
  //cv::imshow("contours", cont);
}

// cut out found rectangles from edged image
for (int i = 0; i < alignedBoundingBoxes.size(); ++i) {
  cv::Rect roi = alignedBoundingBoxes[i];
  _digits.push_back(img_ret(roi).clone());
  //NOTE: note relative to _img
  //_vDigitsRects.push_back(roi);
  //cv::rectangle(_img, roi, {0, 255, 0}, 2);
  }
}
#endif

