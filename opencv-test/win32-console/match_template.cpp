#include "match_template.h"

using namespace cv;
using namespace std;

Mat load_template(string path)
{
	// read to grayscale
	return imread(path, 0);
}

std::vector<cv::Point> getTemplatePts(cv::Mat src, cv::Mat tpl, double thresh)
{
	if (src.channels() == 3)
		cvtColor(src, src, COLOR_BGR2GRAY);
	if (src.channels() == 4)
		cvtColor(src, src, COLOR_BGRA2GRAY);
	Mat res_isabella;
	matchTemplate(src, tpl, res_isabella, TM_CCOEFF_NORMED);
	imshow("res_isabella", res_isabella);
	threshold(res_isabella, res_isabella, thresh, 1.0f, THRESH_TOZERO);
	vector<Point> maxima;
	findNonZero(res_isabella, maxima);
	for (auto it = maxima.begin(); it != maxima.end(); it++) {
		rectangle(src, Rect(it->x, it->y, tpl.cols, tpl.rows), Scalar(0), 1);
		imshow("draw", src);
	}
	return maxima;
}

void findCuteIsabellaInGameScreen()
{
	// read to grayscale
	Mat isabella_tpl = imread("../isabella-crop3.png", 0);
	imshow("isabella", isabella_tpl);

	Mat image = imread("../match_template_screenshot.png");
	imshow("screen", image);

	auto pts = getTemplatePts(image, isabella_tpl);
	if (pts.size() > 0) {
		printf("found her!\n");
	}
	
	waitKey(0);
}
