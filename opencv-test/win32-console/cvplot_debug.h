#pragma once

// build-in plot module is too simple that need lots of own implementations
#include <opencv2/plot.hpp>
#pragma comment(lib, "opencv_plot460d.lib")
// try to use cvplot project from github
// need to clone https://github.com/Profactor/cv-plot.git to the project folder, 
// only use header files without compiling its source code
#define CVPLOT_HEADER_ONLY
#include <CvPlot/cvplot.h>
#include <CvPlot/core/Axes.h>
#include <CvPlot/drawables/LineBase.h>
#include <CvPlot/drawables/Series.h>
#include <CvPlot/drawables/PlaneBase.h>
#include <vector>
#include <math.h>

//DEBUG: try to design 2d colormap, due to cv-plot strict linebase to drawing plot, change while drawing mechanism
void show_plane_colormap() {
	// try 8x6 size to draw color map
	std::vector<double> v;
	cv::Mat m(60, 80, CV_64FC1);
	for (int i = 0; i < 60 * 80; i++) {
		v.push_back(((i * 2) - 4800.f) / 4800.f);
		m.at<double>(i / 80, i % 80) = ((i * 2) - 4800.f) / 4800.f;
	}
	// for linspec, use `p` represent plane chart
	auto axes = CvPlot::plane(m, "pk", "debug");
	axes.setMargins(40, 20, 20, 40);
	auto found = axes.find<CvPlot::Series>("debug");
	cv::Mat mat = axes.render(600, 600);
	cv::imshow("colormap", mat);
	cv::waitKey();
}

void show_line_plot() {
	std::vector<double> v{1.f, 2.f, 3.f, 1.f, 5.f, 2.f};
	// for linspec, refer to LineBase.ipp:
	//  `-` means solid type connecting all markers,
	//  `o` means circle marker type, exntend x/+ for cross/plus symbol
	//  color code only support b/g/r/c/y/m/k/w 8 types
	auto axes = CvPlot::plot(v, "+k");
	axes.xLabel("x");
	// default axes margin x:80,30 y:40,45
	//auto axes = CvPlot::plot(v, "-om");
	cv::Mat mat = axes.render(600, 800);
	cv::imshow("lineplot", mat);
	cv::waitKey();
}

void show_circle_plot() {
	double radius = 5.0;
	int nums = 60;
	// x,y for plot
	std::vector<double> xAxis(nums), yAxis(nums);
	for (int i = 0; i < nums; ++i) {
		double angle = 2 * CV_PI * i / nums;
		double x = radius * cos(angle);
		double y = radius * sin(angle);
		xAxis[i] = x;
		yAxis[i] = y;
	}
	auto axes = CvPlot::makePlotAxes();
	axes.create<CvPlot::Series>(xAxis, yAxis, "-g");
	axes.create<CvPlot::Series>(xAxis, "-b");
	auto ret = axes.find<CvPlot::Series>();
	cv::Mat mat = axes.render(300, 400);
	cv::imshow("circleplot", mat);
	cv::waitKey();
}
