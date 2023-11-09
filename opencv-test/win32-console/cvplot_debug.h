#pragma once

// build-in plot module is too simple that need lots of own implementations
#include <opencv2/plot.hpp>
#pragma comment(lib, "opencv_plot460d.lib")
// try to use cvplot project from github
// need to clone https://github.com/Profactor/cv-plot.git to the project folder, 
// only use header files without compiling its source code
#define CVPLOT_HEADER_ONLY
#include <CvPlot/cvplot.h>
#include <vector>
#include <math.h>

void show_line_plot() {
	std::vector<double> v{1.f, 2.f, 3.f, 1.f, 5.f, 2.f};
	auto axes = CvPlot::plot(v, "-o");
	cv::Mat mat = axes.render(300, 400);
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
	cv::Mat mat = axes.render(300, 400);
	cv::imshow("circleplot", mat);
	cv::waitKey();
}
