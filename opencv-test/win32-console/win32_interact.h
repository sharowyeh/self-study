#pragma once

// prevent using namespace cv in header file, which will get
//   ACCESS_MASK is ambiguous between windows.h and cv::AccessFlag 
#include <Windows.h>
#include <opencv2/opencv.hpp>

// transform Win32 GUI screen to cv::Mat(fmt:BGRA) by given window handle 
bool hwnd2mat(HWND hwnd, cv::Mat& src);

// try to retrieve frame from Windows desktop screen
void streamWindowsDesktop();