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

void configFile(std::string filePath) {
	// NOTE: this sample is using xml, for json as well
	// fsw as file writer
	cv::FileStorage fsw(filePath, cv::FileStorage::WRITE | cv::FileStorage::APPEND | cv::FileStorage::FORMAT_XML);
	fsw << "Hello" << "World";
	fsw << "foo" << "bar";
	fsw << "number" << 123;
	fsw << "sub";
	// for the sub element, must come with { and }
	fsw << "{";
	fsw << "name" << "john";
	fsw << "last" << "doe";
	fsw << "subofsub";
	fsw << "{";
	fsw << "num" << 321;
	fsw << "str" << std::string("from string");
	fsw << "}";
	fsw << "}";
	fsw.release(); // must release to flush data to file
	// NOTE: cv::FileStorage didnt support update value to specific element, can only rewrite whole file

	// fsr as file reader
	cv::FileStorage fsr;
	fsr.open(filePath, cv::FileStorage::READ | cv::FileStorage::FORMAT_XML);
	auto size = fsr.root().size();
	std::string hello = (std::string)fsr["Hello"];
	std::string foo;
	int number;
	fsr["foo"] >> foo;
	fsr["number"] >> number;

	auto sub = fsr["sub"];
	std::string name;
	sub["name"] >> name;
	
	auto subofsub = sub["subofsub"]; // node of the next level
	int num;
	std::string str;
	subofsub["num"] >> num;
	subofsub["str"] >> str;

	fsr.release(); // file mode is read, whether done or not
}

// or move to this header
#include <iostream>
#include <fstream>

struct TestStruct {
	//std::string this_is_string; // if data struct contains dynamic ptr, all variables should output separately with fixed size
	unsigned short this_is_ushort;
	short this_is_short;
	int this_is_int;
	double this_is_double;
	char* this_is_char_array;
	unsigned char* this_is_uchar_array;
	const char* this_is_const_char_array;
};

void composeBin(std::string filePath) {
	// this will face the unsigned char* or std::string that did't have predefined length of array...

	// object initialization
	TestStruct data{};
	//data.this_is_string = "for string data";
	data.this_is_ushort = 123;
	data.this_is_short = 321;
	data.this_is_int = 456;
	data.this_is_double = 789.0;
	const size_t array_size = 10 * sizeof(char);
	std::array<char, array_size> dummy{};
	data.this_is_char_array = dummy.data();
	data.this_is_char_array[0] = 'a';
	data.this_is_char_array[1] = 'r';
	data.this_is_char_array[2] = 'r';
	data.this_is_char_array[3] = 'a';
	data.this_is_char_array[4] = 'y';
	data.this_is_char_array[5] = '\0';
	std::array<unsigned char, array_size> dummy2{};
	data.this_is_uchar_array = dummy2.data();
	data.this_is_uchar_array[0] = 0xff;
	data.this_is_uchar_array[1] = 0x01;
	data.this_is_uchar_array[2] = 0xfe;
	data.this_is_uchar_array[3] = 0x02;
	data.this_is_uchar_array[4] = 0xfc;
	data.this_is_uchar_array[5] = 0x03;
	auto dummy3 = std::string("string to const char");
	data.this_is_const_char_array = dummy3.c_str();

	// use fstream for struct read/write
	std::ofstream outFile(filePath.c_str(), std::ios::binary);
	if (outFile.is_open()) {
		// it only dumps primitive data type, ptr will dump address
		outFile.write(reinterpret_cast<const char*>(&data), sizeof(TestStruct));

		// assign the size of any * data
		outFile.write(reinterpret_cast<const char*>(&array_size), sizeof(size_t));
		// assign the array context
		outFile.write(reinterpret_cast<const char*>(data.this_is_char_array), array_size);
		// alt uchar array
		outFile.write(reinterpret_cast<const char*>(&array_size), sizeof(size_t));
		outFile.write(reinterpret_cast<const char*>(data.this_is_uchar_array), array_size);
		// alt const char from string
		outFile.write(reinterpret_cast<const char*>(data.this_is_const_char_array), dummy3.length() + 1);
		// flush
		outFile.close();
	}

}

// try in sub method return the reading struct
void subDecomposeBin(std::string filePath, TestStruct& dataRead, size_t& array_sizeRead) {

	char* alt_char_array;
	unsigned char* alt_uchar_array;
	std::ifstream inFile(filePath.c_str(), std::ios::binary | std::ios::in);
	if (inFile.is_open()) {
		// only read primitive data, all ptr type will get meaningless address from write process
		inFile.read(reinterpret_cast<char*>(&dataRead), sizeof(TestStruct));

		inFile.read(reinterpret_cast<char*>(&array_sizeRead), sizeof(size_t));
		assert(array_sizeRead > 0);
		// allocate ptr from read size
		dataRead.this_is_char_array = (char*)malloc(array_sizeRead);
		//alt_char_array = (char*)malloc(array_sizeRead);
		inFile.read(reinterpret_cast<char*>(dataRead.this_is_char_array), array_sizeRead);
		//memcpy_s(alt_char_array, array_sizeRead, dataRead.this_is_char_array, array_sizeRead);

		inFile.read(reinterpret_cast<char*>(&array_sizeRead), sizeof(size_t));
		assert(array_sizeRead > 0);
		dataRead.this_is_uchar_array = (unsigned char*)malloc(array_sizeRead);
		//alt_uchar_array = (unsigned char*)malloc(array_sizeRead);
		inFile.read(reinterpret_cast<char*>(dataRead.this_is_uchar_array), array_sizeRead);
		//memcpy_s(alt_uchar_array, array_sizeRead, dataRead.this_is_uchar_array, array_sizeRead);

		//char* str_array = (char*)malloc(21);
		dataRead.this_is_const_char_array = (char*)malloc(21);
		inFile.read((char*)dataRead.this_is_const_char_array, 21);
		//inFile.read(reinterpret_cast<char*>(str_array), 21);
		//dataRead.this_is_const_char_array = str_array;
		//delete str_array;

		inFile.close();
		printf_s("sub: %s\n", dataRead.this_is_char_array);
		//printf_s("%s\n", alt_char_array);
		printf_s("sub: ");
		for (auto i = 0; i < array_sizeRead; i++) {
			printf_s("%x ", dataRead.this_is_uchar_array[i]);
		}
		printf_s("\n");
		//printf_s("%x %x\n", alt_uchar_array[4], alt_uchar_array[5]);

		printf_s("sub: %s\n", dataRead.this_is_const_char_array);

		//delete alt_char_array;
		//delete alt_uchar_array;
	}
	
}

void decomposeBin(std::string filePath) {

	TestStruct dataRead{};

	size_t str_sizeRead;
	size_t array_sizeRead;

	// will go to sub method
	subDecomposeBin(filePath, dataRead, array_sizeRead);
	
	printf_s("%s\n", dataRead.this_is_char_array);
	//printf_s("%s\n", alt_char_array);

	for (auto i = 0; i < array_sizeRead; i++) {
		printf_s("%x ", dataRead.this_is_uchar_array[i]);
	}
	printf_s("\n");

	printf_s("%s\n", dataRead.this_is_const_char_array);
}

std::string workingDirectory()
{
	char tmp[FILENAME_MAX] = { 0 };
	if (!getcwd(tmp, sizeof(tmp))) {
		return std::string();
	}
	return std::string(tmp);
}
