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

// for cross-platform, win32 needs crt secure function
static inline FILE* fopen_(const char* fn, const char* mod) {
#ifdef _WIN32
	FILE* f = nullptr;
	auto ret = fopen_s(&f, fn, mod);
	if (ret != 0)
		return nullptr;
	return f;
#else
	return fopen(fn, mod);
#endif
}

using namespace cv;
using namespace std;

int extract_bayer_channels(int width, int height, std::string file, cv::Mat channels[4]) {

	//int width = 4096;
	//int height = 3072;
	unsigned short* buff = new unsigned short[(size_t)width * height];
	FILE* f = nullptr;
	f = fopen_(file.c_str(), "rb");
	if (!f) {
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

	// remosaicing bayer to rgb
	Size displaySize(width / 4, height / 4);
	Mat bgr;
	cvtColor(raw8, bgr, COLOR_BayerGB2BGR);
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

int imshow_raw10(const char* name, Mat raw10, float ratio = 0.25, bool wait = false) {
	int ch_width = raw10.cols;
	int ch_height = raw10.rows;
	Size displaySize(ch_width * ratio, ch_height * ratio);
	Mat raw8;
	convertScaleAbs(raw10, raw8, 0.25); // 1/4
	resize(raw8, raw8, displaySize);
	imshow(name, raw8);

	if (wait) {
		waitKey(0);
	}

	return 0;
}

int show_channels(Mat channels[4]) {

	imshow_raw10("Gr", channels[0]);
	imshow_raw10("R", channels[1]);
	imshow_raw10("B", channels[2]);
	imshow_raw10("Gb", channels[3]);;

	waitKey(0);

	return 0;
}

// stretch grids fitting full resolution
int grid_and_mean_full(Mat src, int grid_cols, int grid_rows, std::vector<cv::Rect>& rois, Mat& means, std::string prefix = "", Mat* dst_opt = nullptr) {
	Mat dst;
	src.copyTo(dst);
	int calc_width = src.cols / grid_cols;
	int calc_height = src.rows / grid_rows;
	// ensure rois covered full resolution, mod number separately in each rois
	int mod_cols = src.cols % grid_cols;
	int mod_rows = src.rows % grid_rows;
	int step_cols = round((float)grid_cols / mod_cols);
	int step_rows = round((float)grid_rows / mod_rows);
	int offset_cols = 0, offset_rows = 0;
	for (int r = 0; r < grid_rows; r++) {
		// reset offset rows
		offset_cols = 0;
		// if this roi needs to contain additional size
		bool with_more_row = ((r % step_rows == 0 && offset_rows < mod_rows) ||
							  (grid_rows - r == mod_rows - offset_rows));
		for (int c = 0; c < grid_cols; c++) {
			int grid_x = offset_cols + c * calc_width;
			int grid_y = offset_rows + r * calc_height;
			// if this roi needs to contain additional size
			bool with_more_col = ((c % step_cols == 0 && offset_cols < mod_cols) ||
								  (grid_cols - c == mod_cols - offset_cols)); // because we round the step, must comsumed all mod numbers
			int grid_width = calc_width + (with_more_col ? 1 : 0);
			int grid_height = calc_height + (with_more_row ? 1 : 0);
			Rect rect = Rect(grid_x, grid_y, grid_width, grid_height);
			rois.push_back(rect);
			Mat roi(src, rect);
			/*if (r == grid_rows - 1 && c == grid_cols - 1) {
				imshow("last", roi);
			}*/
			Scalar val = cv::mean(roi); // 16UC1
			// subtract black level, raw10 is 64
			if (val[0] <= 64) {
				std::cerr << "roi(" << c << "," << r << ") mean val:" << val[0] << " less than black level" << std::endl;
				val[0] = 0;
			} else {
				val[0] -= 64;
			}
			cv::rectangle(dst, rect, val, -1);
			// little endian for raw10
			means.at<ushort>(r * grid_cols + c) = (ushort)round(val[0]);

			if (with_more_col) {
				offset_cols++;
			}
		}
		if (with_more_row) {
			offset_rows++;
		}
	}
	assert(offset_cols == mod_cols);
	assert(offset_rows == mod_rows);

	// show debug image
	imshow_raw10(string(prefix + "_GRID").c_str(), dst);
	imshow_raw10(string(prefix + "_MEANS").c_str(), means, 1);

	if (dst_opt) {
		dst.copyTo(*dst_opt);
	}

	waitKey(0);

	return 0;
}

int  gain_and_apply(Mat src, int grid_cols, int grid_rows, Mat means, std::string prefix = "", Mat* dst_opt = nullptr) {
	int ch_width = src.cols;
	int ch_height = src.rows;
	// check the center brightness of 41x31, target:20,15
	int target_col = grid_cols / 2 + 1;
	int target_row = grid_rows / 2 + 1;
	ushort target = means.at<ushort>(target_col * grid_rows + target_row);
	// baseline shifted 1024, incase the most brightness is not in center target (the gain mat will looks like raw11)
	Mat grid_gain = 1024 + target - means;
	imshow_raw10(string(prefix + "_GRID_GAIN").c_str(), grid_gain / 2, 1);

	// interpolate resize by opencv from gain table to color channel resolution
	Mat gain;
	// NOTE: with opencv default bilinear interpolation, aware the edges of result mat
	Mat temp;
	int edge_width = ch_width / grid_cols / 2;
	int edge_height = ch_height / grid_rows / 2;
	resize(grid_gain, temp, Size(ch_width, ch_height), 0, 0, INTER_LINEAR);
	// crop edges without applied interpolation
	temp = temp(Rect(edge_width, edge_height, ch_width - edge_width * 2, ch_height - edge_height * 2));
	resize(temp, gain, Size(ch_width, ch_height), 0, 0, INTER_LINEAR); // bilinear interpolation
	imshow_raw10(string(prefix + "_GAIN").c_str(), gain / 2);

	// apply gain to the origin
	Mat apply;
	src.copyTo(apply);
	apply = apply + gain - 1024;
	imshow_raw10(string(prefix + "_APPLY").c_str(), apply);

	if (dst_opt) {
		apply.copyTo(*dst_opt);
	}

	waitKey(0);

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

	std::string pattern[4] = { "Gr", "R", "B", "Gb" };

	// grid each of channels, 41x31 or 99x75, greater grid numbers reducing interpolation computing efforts
	int grid_cols = 41;
	int grid_rows = 31;
	vector<Rect> rois;
	//TODO: better using 32F for further calcuation
	Mat gr_means = Mat::zeros(grid_rows, grid_cols, CV_16UC1);
	Mat r_means = Mat::zeros(grid_rows, grid_cols, CV_16UC1);
	Mat b_means = Mat::zeros(grid_rows, grid_cols, CV_16UC1);
	Mat gb_means = Mat::zeros(grid_rows, grid_cols, CV_16UC1);
	Mat means[4] = { gr_means, r_means, b_means, gb_means };
	for (int i = 0; i < 4; i++) {
		grid_and_mean_full(channels[i], grid_cols, grid_rows, rois, means[i], pattern[i], nullptr);
	}
	
	// generate gain and apply to channels mat
	for (int i = 0; i < 4; i++) {
		gain_and_apply(channels[i], grid_cols, grid_rows, means[i], pattern[i], nullptr);
	}
	
}
