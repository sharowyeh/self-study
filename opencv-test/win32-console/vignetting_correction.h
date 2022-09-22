#pragma once

#include <opencv2/opencv.hpp>

// to generate gradient 2d Mat as vignetting filter
// https://stackoverflow.com/questions/22654770/creating-vignette-filter-in-opencv

// generate gradient Mat with random pixels value
void generateRngGradient(cv::Mat& gradient, double radius = 1.0f, double power = 1.1f, double rng = 0.05f);

// generate gradient Mat by gaussian kernel
void generateGradient(cv::Mat& gradient, double sigma = 0.3f);

// apply inverse vignetting filter for camera vignetting correction 
void normalization_debug();
