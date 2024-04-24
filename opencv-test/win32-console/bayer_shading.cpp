#include "bayer_shading.h"

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// re-built opencv 4.6.0 with vs2022
#pragma comment(lib, "opencv_core460d.lib")
#pragma comment(lib, "opencv_imgcodecs460d.lib")
#pragma comment(lib, "opencv_imgproc460d.lib")
#pragma comment(lib, "opencv_highgui460d.lib")

using namespace cv;
using namespace std;

int extract_bayer_channels(int width, int height, std::string file, cv::Mat channels[4]) {

	//int width = 4096;
	//int height = 3072;
	unsigned short* buff = new unsigned short[(size_t)width * height];
	FILE* f = nullptr;
	if (fopen_s(&f, file.c_str(), "rb") || !f) {
		std::cout << "cannot read\n";
		return -1;
	}
	fread(buff, sizeof(buff[0]), width * height, f);
	fclose(f);
	Mat image(height, width, CV_16UC1, buff);
	if (!image.data)
	{
		std::cout << "cannot read\n";
		return -1;
	}
	// downscale from 10bit to a byte
	Mat raw8;
	convertScaleAbs(image, raw8, 0.25);
	//imshow("raw8", raw8);

	Size displaySize(width / 4, height / 4);
	Mat bgr;
	cvtColor(raw8, bgr, COLOR_BayerGRBG2BGR);
	std::cout << bgr.cols << "x" << bgr.rows << "\n";
	Mat show = bgr;
	resize(show, show, displaySize);
	imshow("show", show);

	// flags for GR line and BG line
	size_t gr_idx = 0, bg_idx = 0;
	// separate channels, we should process by image.data and separated to different channels
	for (size_t i = 0; i < width * height; i++) {
		auto data = image.at<ushort>(i);
		// to check GR or BG, (i / width) % 2 => 0 is GR, 1 is BG
		if ((i / width) % 2 == 0) {
			// to check Gr or R in GR line, i % 2 => 0 is Gr, 1 is R
			if (i % 2 == 0) {
				channels[0].at<ushort>(gr_idx) = data;
			}
			else {
				channels[1].at<ushort>(gr_idx) = data;
				gr_idx++;
			}
		}
		else {
			// to check B or Gb in BG line, i % 2 => 0 is B, 1 is Gb
			if (i % 2 == 0) {
				channels[2].at<ushort>(bg_idx) = data;
			}
			else {
				channels[3].at<ushort>(bg_idx) = data;
				bg_idx++;
			}
		}
	}
	assert(gr_idx == width * height / 4);
	assert(bg_idx == width * height / 4);

	waitKey(0);

	return 0;
}

int show_channels(Mat channels[4]) {
	int ch_width = channels[0].cols;
	int ch_height = channels[0].rows;

	Size displaySize(ch_width / 4, ch_height / 4);
	Mat raw8;
	convertScaleAbs(channels[0], raw8, 0.25);
	resize(raw8, raw8, displaySize);
	imshow("Gr", raw8);

	convertScaleAbs(channels[1], raw8, 0.25);
	resize(raw8, raw8, displaySize);
	imshow("R", raw8);

	convertScaleAbs(channels[2], raw8, 0.25);
	resize(raw8, raw8, displaySize);
	imshow("B", raw8);

	convertScaleAbs(channels[3], raw8, 0.25);
	resize(raw8, raw8, displaySize);
	imshow("Gb", raw8);

	waitKey(0);

	return 0;
}

int imshow_row10(const char* name, Mat raw10) {
	int ch_width = raw10.cols;
	int ch_height = raw10.rows;
	Size displaySize(ch_width / 4, ch_height / 4);
	Mat raw8;
	convertScaleAbs(raw10, raw8, 0.25);
	resize(raw8, raw8, displaySize);
	imshow(name, raw8);

	waitKey(0);

	return 0;
}

int grid_and_mean(Mat& dst, Mat src, int grid_cols, int grid_rows) {
	int grid_width = src.cols / grid_cols;
	int grid_height = src.rows / grid_rows;

	for (int c = 0; c < grid_cols; c++) {
		for (int r = 0; r < grid_rows; r++) {
			int grid_x = c * grid_width;
			int grid_y = r * grid_height;
			Rect rect = Rect(grid_x, grid_y, grid_width, grid_height);
			Mat roi(src, rect);
			Scalar val = cv::mean(roi);
			cv::rectangle(dst, rect, val, -1);
		}
	}

	return 0;
}

void bayer_shading_test() {
	int width = 4096;
	int height = 3072;
	
	int ch_width = width / 2;
	int ch_height = height / 2;

	Mat gr = Mat::zeros(ch_height, ch_width, CV_16UC1);
	Mat r = Mat::zeros(ch_height, ch_width, CV_16UC1);
	Mat b = Mat::zeros(ch_height, ch_width, CV_16UC1);
	Mat gb = Mat::zeros(ch_height, ch_width, CV_16UC1);
	Mat channels[4] = { gr, r, b, gb };
	// this sample raw is GRBG
	extract_bayer_channels(width, height, "../bayer.raw", channels);
	// show debug channels
	show_channels(channels);

	// grid each of channels, 41x31 or 99x75, greater grid numbers reducing interpolation computing efforts
	int grid_cols = 41;
	int grid_rows = 31;
	Mat gr_grid;
	gr.copyTo(gr_grid);
	grid_and_mean(gr_grid, gr, grid_cols, grid_rows);
	// show dbg grid image
	imshow_row10("Gr_GRID", gr_grid);

}