#pragma once

// prevent using namespace cv in header file, which will get
//   ACCESS_MASK is ambiguous between windows.h and cv::AccessFlag 
#include <Windows.h>
#include <opencv2/opencv.hpp>

// transform Win32 GUI screen to cv::Mat(fmt:BGRA) by given window handle 
bool hwnd2mat(HWND hwnd, cv::Mat& src);

// try to retrieve frame from Windows desktop screen
void streamWindowsDesktop();

// headers for getcwd
#include <string>
#include <stdio.h> // FILENAME_MAX
#if _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif
// get current working directory
std::string workingDirectory();
