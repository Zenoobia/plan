#include <iomanip>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "emeocv/Directory.h"
#include "emeocv/ImageInput.h"
#include "emeocv/Config.h"
#include "emeocv/ImageProcessor.h"
#include "emeocv/KNearestOcr.h"
#include "emeocv/Plausi.h"

#include "emeocv/TextDetection.h"
#include "Range.hpp"

#include "Application.h"

static int delay = 1000;

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

cv::Mat pix8ToMat(Pix *pix8) // Used in createLabels
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

static void createLabels(ImageInput *pImageInput) {
  auto *api = new tesseract::TessBaseAPI();

  if(api->Init("/usr/share/tessdata", "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    return;
  }

  Pix *pPix = nullptr;

  //while(pImageInput->nextPix()) {
#if 1
  while(pImageInput->nextPix()) {
    pPix = pImageInput->getPix();
    api->SetImage(pPix);
    api->Recognize(0);
    const int kWidth = pixGetWidth(pPix);
    const int kHeight = pixGetWidth(pPix);

    cv::Mat rgb = pix8ToMat(pPix);
    cv::cvtColor(rgb, rgb, CV_GRAY2RGB);


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
#if 0
        const double PERCENTAGE_TEXT = 0.45;

        const uint32_t REGION_WIDTH_MIN = 20;/* constraints on region size */
        const uint32_t REGION_HEIGHT_MIN = 10;/* constraints on region size */
        if (
            // r > PERCENTAGE_TEXT && /* assume at least 45% of the area is filled if it contains text */

            rect.height > REGION_HEIGHT_MIN // constraints on region size
            &&
            rect.width > REGION_WIDTH_MIN
            && conf >=90.0)
          */
          //      if(rect.height < 80 &&
          //  conf >= 75.0)
          )
#endif
        if(conf >= 90.0)
          {
            float desiredRatio = (float)(133.0/85.0);
            float ratio;
            if(rect.width <= rect.height)
              ratio = ((rect.width / rect.height));
            else
              ratio = ((rect.height / rect.width));
            if(almost_equal(ratio,  desiredRatio, 0))

              // these two conditions alone are not very robust. better to use something
              // like the number of significant peaks in a horizontal projection as a third condition
              {

                const int bufLen = 128;
                char digits[bufLen];
                for (size_t i = 0; i <= bufLen; i++) {
                  const char c = word[i];
                  //for(char& c : word) {
                  if (isdigit(c) || c == '1' || c == '.' || c == '-' || c == '(' || c == ')') {
                    digits[i]=c;
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

              }
            cv::rectangle(rgb, rect, cv::Scalar(0, 255, 0), 1);
          }

    } while (ri->Next(level));

    imshow("rgb", rgb);

#if 0
    int index = 0;
    for(;index < subImages.size(); index++) {
      if(!subImages.empty() && !subImages[index].empty()) {
        //imshow(resultWords[index],subImages[index]);
        //imwrite("output/"+std::string(resultWords[index] + ".tif"),subImages[index]);
        pImageInput->saveImage(subImages[index], std::string{resultWords[index] + ".png"});
      }
    }
#endif
  }
#endif
    }
pixDestroy(&pPix);
  api->End();
};


static void testImages(ImageInput *pImageInput) {
  Config config;
  config.loadConfig();
  ImageProcessor proc(config);
  proc.debugWindow();

  //proc.debugDigits();
  //proc.debugEdges();

  while(pImageInput->nextImage()) {
    cv::Mat &img = pImageInput->getImage();
    proc.setInput(img);
    proc.process();

    int key = cv::waitKey(0);
    if (key == 'q') {
      std::cout << "Quit\n";
      break;
    }
  }
}

static void testOcr(ImageInput* pImageInput) {
  Config config;
  config.loadConfig();
  ImageProcessor proc(config);
  proc.debugWindow();
  proc.debugDigits();

  Plausi plausi;

  KNearestOcr ocr(config);
  if (! ocr.loadTrainingData()) {
    std::cout << "Failed to load OCR training data\n";
    return;
  }
  std::cout << "OCR training data loaded.\n";
  std::cout << "<q> to quit.\n";

  while (pImageInput->nextImage()) {
    proc.setInput(pImageInput->getImage());
    proc.process();
#if 0
    std::string result = ocr.recognize(proc.getOutput());
    std::cout << result;
    if (plausi.check(result, pImageInput->getTime())) {
      std::cout << "  " << std::fixed << std::setprecision(1) << plausi.getCheckedValue() << std::endl;
    } else {
      std::cout << "  -------" << std::endl;
    }
#endif
    int key = cv::waitKey(delay);
    if (key == 'q') {
      std::cout << "Quit\n";
      break;
    }
  }
}

#if 0  // Not working
static void learnOcr(ImageInput *pImageInput) {
  Config config;
  config.loadConfig();
  ImageProcessor proc(config);
  //proc.debugWindow();
  proc.debugDigits();

  KNearestOcr ocr(config);
  ocr.loadTrainingData();

#if 0

  int key = 0;

  while (pImageInput->nextImage()) {
    proc.setInput(pImageInput->getImage());
    proc.process();

    key = ocr.learn(proc.getDigits());
    std::cout << "Done" << std::endl;

    if (key == 'q' || key == 's' || key == 'w') {
      std::cout << "Quit\n";
      break;
    }

  }

  if (key != 'q') {
    std::cout << "Saving training data\n";
    ocr.saveTrainingData();
  }

#else
  std::cout << "Entering OCR training mode!\n";
  std::cout << "<0>..<9> to answer digit, <space> to ignore digit, <s> to save and quit, <q> to quit without saving.\n";

  int key = 0;

  while (pImageInput->nextImage()) {
    proc.setInput(pImageInput->getImage());
    proc.process();

    key = ocr.learn(proc.getOutput());
    std::cout << std::endl;

    if (key == 'q' || key == 's') {
      std::cout << "Quit\n";
      break;
    }
  }

  if (key != 'q') {
    std::cout << "Saving training data\n";
    ocr.saveTrainingData();
  }
#endif
}
#endif

static void ShowHelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}
#include <unordered_map>

bool operator<(cv::Point const& a, cv::Point const& b) {
  return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

bool operator<(cv::Rect const& a, cv::Rect const& b) {
  return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

bool operator==(cv::Rect const& a, cv::Rect const& b) {
  return (a.x == b.x && a.y == b.y);
}

bool operator==(ImVec2 const& a, ImVec2 const& b) {
  return (a.x == b.x && a.y == b.y);
}
ImVec2 operator+(ImVec2 const& a, ImVec2 const& b) {
  return ImVec2{a.x + b.x, a.y + b.y};
}

ImVec2 operator/(ImVec2 const& a, float const b) {return ImVec2{a.x / b, a.y / b};} 
ImVec2 operator/(ImVec2 const& a, ImVec2 const& b) {
  return ImVec2{a.x / b.x, a.y / b.y};
}
ImVec2 operator+=(ImVec2 &a, float const b) {return ImVec2{a.x + b, a.y + b};} 
ImVec2 operator+=(ImVec2 &a, ImVec2 const &b) {
  return ImVec2{a.x + b.y, a.y + b.y};
}
class App : public pl::Application {
 public:
  void draw() override {
  };

  inline
  void ImGuiDrawSubImage(cv::Mat const &img, cv::Rect const & rect, GLuint const textureID) {
    const ImVec2 imageSize{static_cast<float>(img.cols), static_cast<float>(img.rows)};
    const ImVec2 subImagePos{static_cast<float>(rect.x), static_cast<float>(rect.y)};
    const ImVec2 subImageSize{static_cast<float>(rect.width), static_cast<float>(rect.height)};

    const ImVec2 subImageUVPos{subImagePos / imageSize};
    const ImVec2 subImageUVSize{subImageSize / imageSize};

    //ImGui::SameLine();
    ImGui::Image(GLUINT2TEX(textureID),
                 subImageSize,
                 subImageUVPos,
                 subImageUVSize + subImageUVPos
                 );
  }

  void run() override {
    m_clearColor = ImColor(114, 144, 154);

    Config config;
    config.loadConfig();
    ImageProcessor imgProcessor(config);
    TextDetection textDetection(config, &imgProcessor);

    auto pImageInput = new DirectoryInput(Directory("../data/test", ".png"));

    pImageInput->nextImage(); 
    cv::Mat &img = pImageInput->getImage();
    imgProcessor.setInput(img);
    imgProcessor.process();
    textDetection.setInput(pImageInput);
    textDetection.process();

    KNearestOcr ocr(config);
    ocr.loadTrainingData();

    static GLuint tex = 0;
    while (!glfwWindowShouldClose(m_pGLFWwindow)) {
      initDraw(); 
      ImGui::Begin("");

      if(img.data && tex == 0) {
        tex = matToTexture(img);
      }

#if 1 // Draw img
      {
        ImVec2 imgSize{static_cast<float>(img.cols), static_cast<float>(img.rows)};
        ImVec2 imgPos{0.0, 0.0};

        ImGui::SetNextWindowSize(imgSize / 4);

        //ImGui::Image(GLUINT2TEX(tex), {static_cast<float>(img.cols), static_cast<float>(img.rows)});
        ImGui::Image(GLUINT2TEX(tex), {static_cast<float>(img.cols / 4), static_cast<float>(img.rows / 4)});
        ImGui::NewLine();
      }
#endif
#if 1 // Draw Symbols with context
      ImGui::End(); 
      ImGui::Begin("Symbols");
      for(TextLine &text : textDetection.getTextLines()) {
        for(Symbol &symbol : text.vSymbols) {
          //for(char const &character: symbol.vDetectedChars) {
          for(auto i : util::lang::indices(symbol.vDetectedChars)) {
            if(symbol.vConfidence[i] > 90.f) {
              auto &rect = symbol.vDetectedRects[i];
              auto &character = symbol.vDetectedChars[i];
              ImGuiDrawSubImage(img, rect, tex);
              ImGui::SameLine();
              if(ImGui::Button("Learn")) {
                ocr.learn(img, rect, character);
                symbol.vConfidence[i] = 1.f;
                symbol.vAccurate[i] = true;
              }
              //ImGui::Text("Symbol: %c , Confidence: %.2f", symbol.vDetectedChars[i], symbol.vConfidence[i]);
              ImGui::SameLine();
              //ImGui::Text("OCR Result: %c ---", ocr.recognize(img(rect)));
              ImGui::SameLine();
              ImGui::Text("Symbol: %c , Confidence: %.2f", symbol.vDetectedChars[i], symbol.vConfidence[i]);
            }

            /*
              if(symbol.confidence > 0.0f) {
              cv::Rect const rect = character.Rect;
              ImGuiDrawSubImage(img, rect, texSrc);
              ImGui::SameLine();ImGui::Text("Symbol: %c , Confidence: %.2f", character.Character, character.Confidence);
              }
            */
          }
        }
      }
#endif

      if(ImGui::IsKeyPressed('S')) {
        ocr.saveTrainingData();
        textDetection.saveData();
      } 

      if(ImGui::IsKeyPressed('X')) {
        glfwSetWindowShouldClose(m_pGLFWwindow, GLFW_TRUE);
        break;
      }

      ImGui::End();
      draw();
      finishDraw();
      glfwWaitEvents();
    }
  }


#if 0
  void run() override {
    m_clearColor = ImColor(114, 144, 154);
    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    //proc.debugWindow();
    proc.debugDigits();
    proc.debugEdges();

#if 0
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    if(api->Init("/usr/share/tessdata", "swe")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
    }


#endif

    //auto pImageInput = new DirectoryInput(Directory("/home/jq/Documents/Arkiv/Arkiv", ".png"));
    //auto pImageInput = new DirectoryInput(Directory("../data", ".png"));
    auto pImageInput = new DirectoryInput(Directory("../data/test", ".png"));

    pImageInput->nextImage();
    proc.setInput(pImageInput->getImage());
    cv::Mat imgSrc = pImageInput->getImage().clone();

    Pix *pPix = nullptr;
    //pPix = mat8ToPix(imgSrc); 

    proc.process();

    pPix = pImageInput->getPix();
    //pPix = mat8ToPix(pImageInput->getImage());


    static GLuint tex = 0;
    static GLuint texSrc = 0;

    static GLuint tmpTex = 0;
    while (!glfwWindowShouldClose(m_pGLFWwindow))//&& pImageInput->nextImage()) {
    {
      initDraw();
#if 1 // Draw

      cv::Mat &img = pImageInput->getImage();

      if(img.data && tex == 0) {
        tex = matToTexture(img);
        texSrc = matToTexture(imgSrc);
      }

      ImVec2 imgSize{static_cast<float>(img.cols), static_cast<float>(img.rows)};
      ImVec2 imgPos{0.0, 0.0};

      ImGui::Begin("");
#if 1 // Draw img
      ImGui::SetNextWindowSize(imgSize / 4);

      //ImGui::Image(GLUINT2TEX(tex), {static_cast<float>(img.cols), static_cast<float>(img.rows)});
      ImGui::Image(GLUINT2TEX(tex), {static_cast<float>(img.cols / 4), static_cast<float>(img.rows / 4)});
      ImGui::NewLine();
      ImGui::End();
#endif

#if 0 // Draw Symbols with context
      ImGui::Begin("Symbols");
      for(detected_text &text : proc._vDetectedText) {
        for(detected_char &character: text.Characters) {
          if(character.Confidence > 0.0f) {
            cv::Rect const rect = character.Rect;
            ImGuiDrawSubImage(img, rect, texSrc);
            ImGui::SameLine();ImGui::Text("Symbol: %c , Confidence: %.2f", character.Character, character.Confidence);
          }
        }
      }
#endif

      //static char inputChar = '  ';
      static char inputChar[2];

      ImGui::InputText("input text", inputChar, 2);
      ImGui::SameLine(); if (ImGui::Button("Button")) {
        std::cout << inputChar << std::endl;
        inputChar[0] = ' ';
      }

      ImGui::SameLine(); ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");

      if(ImGui::IsKeyPressed('X')) {
        glfwSetWindowShouldClose(m_pGLFWwindow, GLFW_TRUE);
        break;
      } else if (ImGui::IsKeyPressed('L') && pImageInput->nextImage()) {

        proc.setInput(img);
        imgSrc = pImageInput->getImage().clone();
        if(pPix != nullptr) pixDestroy(&pPix);
        pPix = pImageInput->getPix();

        proc.process();

        glDeleteTextures(1, &tex);
        glDeleteTextures(1, &texSrc);
        tex = 0;
        texSrc = 0;
      }

      ImGui::End();

      /*
        int key = cv::waitKey(0);
        if (key == 'q') {
        std::cout << "Quit\n";
        break;
        }
      */


#endif
      draw();
      finishDraw();
      glfwWaitEvents();
    }
    if(pPix != nullptr)
      pixDestroy(&pPix);

    glDeleteTextures(1, &tex);
    glDeleteTextures(1, &tmpTex);
  }
#endif
};



int main(int argc, char **argv) {

#if 1
  App app;
  app.initGLFW();
  app.run();
#else
  ImageInput *pImageInput = 0;
  int inputCount = 0;

  const char *directoryPath = "../data";
  //directoryPath = "/home/jq/Documents/Arkiv/Bits";
  directoryPath = "/home/jq/Documents/Arkiv/Arkiv";
  directoryPath = "/home/joaqim/Downloads/Arkiv/Arkiv";
  directoryPath = "../data";
  const char *directoryExtension = ".png";
  //directoryExtension = ".pdf";

  std::string outputDir = "output";
  std::string logLevel = "ERROR";

  char cmd = 0;
  int cmdCount = 0;
#if 0
  for (int index=0; index < argc; ++index)
  {
    char c = *argv[index];
    switch(c) {
      case 'd':
        index++;
        if (index < argc) {
          directoryPath = argv[index];
          inputCount++;
        }
        break;
      case 'e':
        index++;
        if(index < argc) {
          directoryExtension = argv[index];
          inputCount++;
        };
        break;
      case 't':
        cmd = c;
        cmdCount++;
        break;
      case 'l':
        cmd = c;
        cmdCount++;
        break;
      case 'o':
        if (index < argc) {
          cmd = c;
          cmdCount++;
          outputDir = argv[index];
          inputCount++;
        }
        break;
    }
  };
#endif

  pImageInput = new DirectoryInput(Directory(directoryPath, directoryExtension));
  pImageInput->setOutputDir(outputDir);
  testImages(pImageInput);
  //learnOcr(pImageInput);
  // testOcr(pImageInput);
#endif

#if 0
  switch (cmd) {
    case 'l':
      std::cout << "Finding letters in " << directoryExtension << " images in directory: "<< directoryPath << std::endl;
      std::cout << "Saving Labels in " << outputDir << std::endl;
      learnOcr(pImageInput);
      //createLabels(pImageInput);
      break;
    case 't':
      std::cout << "Testing Images in " << directoryPath << std::endl;
      testOcr(pImageInput);
      //      testImages(pImageInput);
      break;
    case 'a':
      break;
    case 'w':
      break;
    default:
      testOcr(pImageInput);
      break;
  }
#endif
  //  learnOcr(pImageInput);
  //testImages(pImageInput);
  exit(EXIT_SUCCESS);
};
