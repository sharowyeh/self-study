#pragma once

#include <opencv2/opencv.hpp>

// load target image to grayscale
cv::Mat load_template(std::string path);

// get the template points found in source
// for my personal usage, fixed TM_CCOEFF_NORMED and THRESH_TOZERO
std::vector<cv::Point> getTemplatePts(cv::Mat src, cv::Mat tpl, double thresh = 0.85f);

// try to find specific character in a gacha game screen
void findCuteIsabellaInGameScreen();