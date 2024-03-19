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

// for create raw10 shading file
#include "vignetting_correction.h"

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
	unsigned short* buff = new unsigned short[(size_t)width * height];
	FILE* f = nullptr;
	if (fopen_s(&f, "../iso_real2.raw", "rb") || !f) {
		std::cout << "cannot read\n";
		return;
	}
	fread(buff, sizeof(buff[0]), width * height, f);
	fclose(f);
	Mat image(height, width, CV_16UC1, buff);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return;
	}
	// downscale from 10bit to a byte
	Mat raw8;
	convertScaleAbs(image, raw8, 0.25);
	imshow("raw8", raw8);

	Size displaySize(width / 4, height / 4);
	Mat bgr;
	cvtColor(raw8, bgr, COLOR_BayerRGGB2BGR);
	std::cout << bgr.cols << "x" << bgr.rows << "\n";
	Mat show = bgr;
	resize(show, show, displaySize);
	imshow("show", show);

	waitKey(0);
}

void crop_raw10_file() {
	int new_width = 4000;
	int new_height = 3000;
	Mat new_img(new_height, new_width, CV_16UC1);

	int width = 4096;
	int height = 3072;
	unsigned short* buff = new unsigned short[(size_t)width * height];
	FILE* f = nullptr;
	if (fopen_s(&f, "../iso.raw", "rb") || !f) {
		std::cout << "cannot read\n";
		return;
	}
	fread(buff, sizeof(buff[0]), width * height, f);
	fclose(f);
	Mat image(height, width, CV_16UC1, buff);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return;
	}

	if (new_width > width || new_height > height) {
		std::cout << "new size can not larger than origin\n";
		return;
	}
	if ((new_width - width) % 2 != 0 || (new_height - height) % 2 != 0) {
		std::cout << "size not odd, bayer pattern will be incorrect\n";
		return;
	}

	int offset_width = (width - new_width) / 2;
	int offset_height = (height - new_height) / 2;

	for (int row = offset_height; row < offset_height + new_height; row++) {
		for (int col = offset_width; col < offset_width + new_width; col++) {
			auto pixel = image.at<ushort>(row, col);
			new_img.at<ushort>(row - offset_height, col - offset_width) = pixel;
		}
	}

	// store raw10 via fopen from cv::Mat
	FILE* fout = nullptr;
	if (fopen_s(&fout, "../resize.raw", "wb") || !fout) {
		std::cout << "adjust output raw opens failed" << std::endl;
		return;
	}
	// write byte from cv::Mat even is ushort per pixel
	const uchar* ptr = new_img.data;
	size_t size = new_img.total() * new_img.elemSize();
	fwrite(ptr, 1, size, fout);
	fclose(fout);

	// downscale from 10bit to a byte
	Mat raw8;
	convertScaleAbs(new_img, raw8, 0.25);

	Size displaySize(new_width / 4, new_height / 4);
	Mat bgr;
	cvtColor(raw8, bgr, COLOR_BayerBGGR2BGR);
	std::cout << bgr.cols << "x" << bgr.rows << "\n";
	Mat show = bgr;
	cv::resize(show, show, displaySize);
	imshow("show", show);

	waitKey(0);
}

void increase_raw10_file() {
	int width = 4096;
	int height = 3072;
	unsigned short* buff = new unsigned short[(size_t)width * height];
	FILE* f = nullptr;
	if (fopen_s(&f, "../CalibRaw_G156.raw", "rb") || !f) {
		std::cout << "cannot read\n";
		return;
	}
	fread(buff, sizeof(buff[0]), width * height, f);
	fclose(f);
	Mat image(height, width, CV_16UC1, buff);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return;
	}
	
	srand(time(0));

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			auto pixel = image.at<ushort>(row, col);
			pixel += 1 + rand() % 8; // increase number to raw10 pixel
			image.at<ushort>(row, col) = pixel;
		}
	}

	// store raw10 via fopen from cv::Mat
	FILE* fout = nullptr;
	if (fopen_s(&fout, "../CalibRaw_G156_8.raw", "wb") || !fout) {
		std::cout << "adjust output raw opens failed" << std::endl;
		return;
	}
	// write byte from cv::Mat even is ushort per pixel
	const uchar* ptr = image.data;
	size_t size = image.total() * image.elemSize();
	fwrite(ptr, 1, size, fout);
	fclose(fout);

	// downscale from 10bit to a byte
	Mat raw8;
	convertScaleAbs(image, raw8, 0.25);
	
	Size displaySize(width / 4, height / 4);
	Mat bgr;
	cvtColor(raw8, bgr, COLOR_BayerRGGB2BGR);
	std::cout << bgr.cols << "x" << bgr.rows << "\n";
	Mat show = bgr;
	resize(show, show, displaySize);
	imshow("show", show);

	waitKey(0);
}

// create a simulated shading image by vignetting
void create_raw10_shading() {
	int width = 4096;
	int height = 3072;
	// use generateRngGradient(), refer to normalization_debug()
	cv::Mat filter = cv::Mat(height, width, CV_64FC1, cv::Scalar(1));
	generateGradient(filter, 0.3f);
	generateRngGradient(filter, 1.1f, 0.9f, 0.05f);
	//cv::imshow("filter", filter);

	Size displaySize(width / 4, height / 4);
	cv::Mat display;
	resize(filter, display, displaySize);
	cv::imshow("filter", display);

	// ref need alpha(ratio) and beta(shift) to specify contained 155~55(raw8) variable vignetting
	double pred = 150.0f; // for G channel (maximum = 150+100/2 = 200
	double range = 100.0f;
	cv::Mat ref8 = cv::Mat(height, width, CV_8UC1, cv::Scalar(pred));
	// use convert, alpha is scale, beta is shift
	filter.convertTo(ref8, CV_8UC1, range, pred - range / 2);

	cv::Mat display2;
	resize(ref8, display2, displaySize);
	cv::imshow("ref", display2);

	// raw10
	double pred10 = 420.0f; // for G channel (maximum = 420+400/2 = 620)
	double range10 = 400.0f;
	cv::Mat ref10 = cv::Mat(height, width, CV_16UC1, cv::Scalar(pred10));
	filter.convertTo(ref10, CV_16UC1, range10, pred10 - range10 / 2);

	// to remosaic bayer RGGB, refer from 5100k light, R/Gb/B align to Gr ratio=1
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (row % 2 == 0) {
				// RG
				if (col % 2 == 0) {
					// R
					auto r = ref10.at<ushort>(row, col);
					r = (ushort)((double)r * 0.556); // R/Gr
					ref10.at<ushort>(row, col) = r;
				}
				else {
					// Gr (referral ratio=1)
				}
			}
			else {
				// GB
				if (col % 2 == 0) {
					// Gb
					auto gb = ref10.at<ushort>(row, col);
					gb = (ushort)((double)gb * 0.997);
					ref10.at<ushort>(row, col) = gb;
				}
				else {
					// B
					auto b = ref10.at<ushort>(row, col);
					b = (ushort)((double)b * 0.591); // B/Gr
					ref10.at<ushort>(row, col) = b;
				}

			}
		}
	}

	// store raw10 via fopen from cv::Mat
	FILE* fout = nullptr;
	if (fopen_s(&fout, "../ref10.raw", "wb") || !fout) {
		std::cout << "adjust output raw opens failed" << std::endl;
		return;
	}
	// write byte from cv::Mat even is ushort per pixel
	const uchar* ptr = ref10.data;
	size_t size = ref10.total() * ref10.elemSize();
	fwrite(ptr, 1, size, fout);
	fclose(fout);

	// downscale from 10bit to a byte
	Mat raw8, display3;
	convertScaleAbs(ref10, raw8, 0.25);
	resize(raw8, display3, displaySize);
	cv::imshow("raw8", display3);

	waitKey(0);
}

void change_raw10_bayer() {
	int width = 4096;
	int height = 3072;
	unsigned short* buff = new unsigned short[(size_t)width * height];
	FILE* f = nullptr;
	if (fopen_s(&f, "../real.raw", "rb") || !f) {
		std::cout << "cannot read\n";
		return;
	}
	fread(buff, sizeof(buff[0]), width * height, f);
	fclose(f);
	Mat image(height, width, CV_16UC1, buff);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return;
	}

	// for raw10 bayer pixel swapping BGGR to GRBG
	ushort* ptrs = image.ptr<ushort>();
	for (size_t i = 0; i < height; i += 2) {
		auto bg_start = i * width;
		auto gr_start = (i + 1) * width;
		std::swap_ranges(ptrs + bg_start, ptrs + bg_start + width, ptrs + gr_start);
	}

	// store raw10 via fopen from cv::Mat
	FILE* fout = nullptr;
	if (fopen_s(&fout, "../bayer.raw", "wb") || !fout) {
		std::cout << "adjust output raw opens failed" << std::endl;
		return;
	}
	// write byte from cv::Mat even is ushort per pixel
	const uchar* ptr = image.data;
	size_t size = image.total() * image.elemSize();
	fwrite(ptr, 1, size, fout);
	fclose(fout);
}
