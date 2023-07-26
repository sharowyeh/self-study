#include "imread_debug.h"

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
//#pragma comment(lib, "opencv_world450d.lib")

using namespace cv;

void read_goddess_lenna()
{

	Mat image;
	// The lenna image only contains 3 channels BGR
	image = imread("../Lenna_test_image.png", IMREAD_COLOR);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return;
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

void read_raw10_file()
{
	// can't use imread to read raw10bit
	int width = 4096;
	int height = 3072;
	short* buff = new short[(size_t)width * height];
	FILE* f = nullptr;
	if (fopen_s(&f, "../raw10.raw", "rb") || !f) {
		std::cout << "cannot read\n";
		return;
	}
	fread(buff, sizeof(buff[0]), width * height, f);
	fclose(f);
	Mat image(height, width, CV_16SC1, buff);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return;
	}
	Mat raw8;
	// downscale from 10bit to a byte
	convertScaleAbs(image, raw8, 0.25);
	imshow("raw8", raw8);

	Mat bgr;
	cvtColor(raw8, bgr, COLOR_BayerRGGB2BGR);
	std::cout << bgr.cols << "x" << bgr.rows << "\n";
	Mat show = bgr;
	resize(show, show, Size(show.cols / 8, show.rows / 8));
	imshow("show", show);

	waitKey(0);
}
