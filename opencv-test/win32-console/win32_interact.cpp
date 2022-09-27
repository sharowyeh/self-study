#include "win32_interact.h"

//   this test function is Windows desktop GUI highly relevant, 
//   about the DPI awareness, refer to 
//   https://learn.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows
//   https://learn.microsoft.com/en-us/windows/win32/api/shellscalingapi/

bool hwnd2mat(HWND hwnd, cv::Mat& src)
{
	// code reference from https://stackoverflow.com/questions/34466993/opencv-desktop-capture

	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	//cv::Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	// NOTE: also can adjust resolution with DPI scaling for dedvice context
	//       which applied system or per-monitor DPI 
	srcheight = windowsize.bottom;// * 1.25f;
	srcwidth = windowsize.right;// * 1.25f;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	//return src;
	return true;
}

bool isMouseDrag = false;
cv::Rect drawRect = cv::Rect(0, 0, 0, 0);
cv::Point originPt = cv::Point(0, 0);
char drawText[256] = { 0 };

void onMouseEventHandler(int evt, int x, int y, int flags, void* userdata)
{
	int dx, dy, dw, dh = 0;
	switch (evt) {
	case cv::EVENT_LBUTTONDOWN:
		isMouseDrag = true;
		originPt = cv::Point(x, y);
		break;
	case cv::EVENT_LBUTTONUP:
		isMouseDrag = false;
		break;
	case cv::EVENT_MOUSEMOVE:
		if (isMouseDrag) {
			// for any drag direction
			dx = std::min(x, originPt.x);
			dy = std::min(y, originPt.y);
			dw = std::abs(x - originPt.x);
			dh = std::abs(y - originPt.y);
			drawRect = cv::Rect(dx, dy, dw, dh);
		}
		break;
	}
}

void streamWindowsDesktop()
{
	// make process DPI awareness, refer to head of this file for desc
	// for Vista or older version, same as system aware
	//SetProcessDPIAware();
	// for Win10 1607 or later version, also checkout per monitor DPI
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	cv::Mat desktop;
	// NOTE: careful opencv highgui default is DPI unaware,  
	// set opencv window without flag WINDOW_AUTOSIZE, let image fit the window and resize it
	cv::namedWindow("desktop", cv::WINDOW_NORMAL);
	cv::setMouseCallback("desktop", onMouseEventHandler, NULL);
	
	printf("press any key to leave\n");
	while (true) {
		hwnd2mat(GetDesktopWindow(), desktop);
		if (desktop.empty() == false) {
			cv::setWindowProperty("desktop", 
				cv::WindowPropertyFlags::WND_PROP_AUTOSIZE, 
				cv::WindowFlags::WINDOW_NORMAL);
			if (drawRect.area() > 0) {
				cv::rectangle(desktop, drawRect, cv::Scalar(0, 255, 0));
				sprintf_s(drawText, "x,y:(%d,%d);w,h:(%d,%d)", 
					drawRect.x, drawRect.y, drawRect.width, drawRect.height);
				cv::putText(desktop, drawText, 
					cv::Point(drawRect.x, drawRect.y - 8), // upper than rectangle
					cv::FONT_HERSHEY_SIMPLEX, 0.5f, 
					cv::Scalar(0, 255, 0));
			}
			cv::imshow("desktop", desktop);

			auto key = cv::waitKey(10);
			if (key != -1)
				break;
		}
	}
}
