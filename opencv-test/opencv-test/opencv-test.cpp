// opencv-test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#pragma comment(lib, "opencv_world346d.lib")

using namespace cv;

int main()
{
    //std::cout << "Hello World!\n"; 
	Mat image;
	// The lenna image only contains 3 channels BGR
	image = imread("Lenna_test_image.png", CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return 0;
	}

	Mat ciexyz;
	cvtColor(image, ciexyz, COLOR_BGR2XYZ);
	std::cout << ciexyz.channels() << "\n";
	namedWindow("display window", CV_WINDOW_AUTOSIZE);
	rectangle(ciexyz, Rect(20, 20, 10, 10), Scalar(0.3, 0.3, 0.3));
	Vec3b data = image.at<Vec3b>(Point(25, 25));
	Vec3b data2 = ciexyz.at<Vec3b>(Point(25, 25));
	std::cout << data[0] << " " << data[1] << " " << data[2] << "\n";
	std::cout << data2[0] << " " << data2[1] << " " << data2[2] << "\n";
	imshow("display window", ciexyz);
	
	waitKey(0);
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
