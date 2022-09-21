#include "imread_debug.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#pragma comment(lib, "opencv_world450d.lib")

using namespace cv;

void read_goddess_lenna()
{

	Mat image;
	// The lenna image only contains 3 channels BGR
	image = imread("../Lenna_test_image.png", IMREAD_COLOR);
	if (!image.data)
	{
		std::cout << "cannot read\n";
	}

	Mat ciexyz;
	cvtColor(image, ciexyz, COLOR_BGR2XYZ);
	std::cout << ciexyz.channels() << "\n";
	namedWindow("lenna", WindowFlags::WINDOW_AUTOSIZE);
	rectangle(ciexyz, Rect(20, 20, 10, 10), Scalar(0.3, 0.3, 0.3));
	Vec3b data = image.at<Vec3b>(Point(25, 25));
	Vec3b data2 = ciexyz.at<Vec3b>(Point(25, 25));
	std::cout << data[0] << " " << data[1] << " " << data[2] << "\n";
	std::cout << data2[0] << " " << data2[1] << " " << data2[2] << "\n";
	imshow("lenna", ciexyz);

	waitKey(0);
}
