#include "win32_interact.h"

// about the DPI awareness, refer to 
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

	//TODO: checkout shellscalingapi.h
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

void streamWindowsDesktop()
{
	cv::Mat desktop;
	// NOTE: careful opencv highgui default is DPI unaware,  
	// set opencv window without flag WINDOW_AUTOSIZE, let image fit the window and resize it
	cv::namedWindow("desktop", cv::WINDOW_NORMAL);
	
	printf("press any key to leave\n");
	while (true) {
		hwnd2mat(GetDesktopWindow(), desktop);
		if (desktop.empty() == false) {
			cv::setWindowProperty("desktop", 
				cv::WindowPropertyFlags::WND_PROP_AUTOSIZE, 
				cv::WindowFlags::WINDOW_NORMAL);
			cv::imshow("desktop", desktop);

			auto key = cv::waitKey(10);
			if (key != -1)
				break;
		}
	}
}
