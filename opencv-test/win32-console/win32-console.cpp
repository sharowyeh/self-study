// win32-console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// re-built opencv 4.6.0 with vs2022
#pragma comment(lib, "opencv_core460d.lib")
#pragma comment(lib, "opencv_imgcodecs460d.lib")
#pragma comment(lib, "opencv_imgproc460d.lib")
#pragma comment(lib, "opencv_highgui460d.lib")
#pragma comment(lib, "opencv_videoio460d.lib")
#pragma comment(lib, "opencv_objdetect460d.lib")

//#pragma comment(lib, "opencv_world450d.lib")
// prevent using namespace cv here or header files, which will get
//   ACCESS_MASK is ambiguous between windows.h and cv::AccessFlag 

#include "facial_landmarks.h"
// for facial landmarks function using dlib
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#pragma comment(lib, "dlib19.24.99_debug_64bit_msvc1933.lib")

#include "imread_debug.h"
#include "vignetting_correction.h"
#include "win32_interact.h"
#include "match_template.h"

int main()
{
    std::cout << "Hello World!\n";

	//read_goddess_lenna();

	//normalization_debug();

	//streamWindowsDesktop();

	//findCuteIsabellaInGameScreen();

	//facial_landmarks_dlib();

	read_raw10_file();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
