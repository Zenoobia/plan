#pragma once

#include <iostream>

#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>

#include <opencv2/imgproc/imgproc.hpp>

using poppler::document;
using poppler::page;
using poppler::image;
using poppler::page_renderer;


std::string basename( std::string const& pathname);

std::string removeExtension( std::string const& filename );

static std::vector<cv::String> ListDirectory(const cv::String path) noexcept;

//static cv::Mat readPDFtoCV(std::string const &filename, int const DPI=200);
cv::Mat readPDFtoCV(std::string const &filename);
