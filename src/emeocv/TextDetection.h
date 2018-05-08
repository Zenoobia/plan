#ifndef TEXTDETECTION_H_
#define TEXTDETECTION_H_

#include <vector>
#include <map>

#include <opencv2/imgproc/imgproc.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "ImageInput.h"
#include "Config.h"

#include "../Range.hpp"
using util::lang::indices;

#include "ImageProcessor.h"
/**
 * Functor to help sorting rectangles by their x-position.
 */
//bool operator<(cv::Rect const& a, cv::Rect const& b);


struct Symbol {

  Symbol() = default;
  //Symbol(std::map<float, char> const &detected) : mDetected(detected) {} 
  Symbol(std::vector<float> const confidences, std::vector<char> const characters, std::vector<cv::Rect> const &rects) {
    vConfidence = confidences;
    vDetectedChars = characters; 
    vDetectedRects = (rects); 
  }
  Symbol(float const confidence, char const character, cv::Rect const &rect) {
    vConfidence.emplace_back(confidence); 
    vDetectedChars.emplace_back(character); 
    vDetectedRects.emplace_back(rect); 
  }

  void write(cv::FileStorage& fs) const                        //Write serialization for this class
  {
  }

  void read(const cv::FileNode& node)                          //Read serialization for this class
  {
    int nCharacters = node["number_of_characters"];
    vDetectedChars.resize(nCharacters);
    vConfidence.resize(nCharacters);
    vDetectedRects.resize(nCharacters);

    for (auto i : util::lang::range(0, nCharacters)) {
      char character = 'A';
      float confidence = .99f;
      cv::Rect rect{10, 12, 92, 24};

      //node["characters"] >> character;
      //node["confidence"] >> confidence;
      //node["rects"] >> rect;

      vDetectedChars[i] = character;
      vConfidence[i] = confidence;
      vDetectedRects[i] = rect;

      //item["characters"] >> symbols[i].vDetectedChars[0];
      //item["confidence"] >> symbols[i].vConfidence[0];
      //item["rects"] >> symbols[i].vDetectedRects[0];
    }
  }

                          //std::map<float, char> mDetected;
  std::vector<float> vConfidence; 
  std::vector<char> vDetectedChars; 
  std::vector<cv::Rect> vDetectedRects; 
  std::vector<bool> vAccurate;
};

struct TextLine {
  TextLine() = default;
 TextLine(cv::Rect const &rect_) : rect(rect_) {}; 

  inline void addSymbol(Symbol const &symbol) {
    vSymbols.emplace_back(symbol); 
  }

  inline
  void addSymbol(float const &confidence, char const &character, cv::Rect const &rect) {
    vSymbols.push_back(Symbol{confidence, character, rect}); 
  } 
  std::vector<Symbol> vSymbols;
  cv::Rect rect; 
};

using Rects = std::vector<cv::Rect>;
using Contours = std::vector<std::vector<cv::Point>>;

class TextDetection {
public:
  //  TextDetection(Config const &config, ImageProcessor *pImageProcessor);
  TextDetection(Config const &config, ImageProcessor *pImageProcessor);
  ~TextDetection() noexcept; 

  void setInput(ImageInput *imageInput);
  std::vector<TextLine> getTextLines() const noexcept; 
  void process(bool const loadCached = true);

  std::vector<cv::Rect> findTextLines();
  
    std::vector<cv::Rect> findCounterDigits();

    std::vector<cv::Rect> findCounterDigits(const cv::Mat &cImg, const int xOffset=0, const int yOffset=0); 

    void findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
                          std::vector<cv::Rect>::const_iterator end,
                          std::vector<cv::Rect>& result);

  
    Rects findAlignedBoxes(Rects::const_iterator begin,
                           Rects::const_iterator end); 

    void saveData(std::string name = "") const noexcept;

    bool loadData() noexcept;
private:
    std::vector<TextLine> _vTextLines;

    cv::Mat _img;
    Pix *_pPix;
    tesseract::TessBaseAPI *_pTessApi;

    Config _config; 
    ImageInput *_pImageInput;
    ImageProcessor *_pImageProcessor;
  };


#endif // TEXTDETECTION_H_
