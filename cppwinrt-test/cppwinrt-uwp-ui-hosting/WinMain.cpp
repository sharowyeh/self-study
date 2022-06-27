#include "pch.h"

/* For UWP UI hosting, refer to:
 * https://www.nuget.org/packages/Microsoft.Toolkit.Win32.UI.SDK
 * and example:
 * https://docs.microsoft.com/en-us/windows/apps/desktop/modernize/using-the-xaml-hosting-api
 * the toolkit is preview version and looks like a deep hole
 */

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Composition;
using namespace winrt::Windows::UI::Xaml::Hosting;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HWND _hWnd;
HWND _childhWnd;
HINSTANCE _hInstance;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// The call to winrt::init_apartment initializes COM; by default, in a multithreaded apartment.
	init_apartment(apartment_type::single_threaded);

	_hInstance = hInstance;
	// Create Win32 window for hosting UWP controls
	const wchar_t szWindowClass[] = L"Win32WindowClass";
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = szWindowClass;
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);
	if (RegisterClassEx(&windowClass) == NULL) {
		::MessageBox(::GetDesktopWindow(), L"Class register failed!", L"Error", MB_OK);
		return 0;
	}

	_hWnd = CreateWindow(
		szWindowClass,
		L"Win32 Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (_hWnd == NULL) {
		::MessageBox(::GetDesktopWindow(), L"Create window failed!", L"Error", MB_OK);
		return 0;
	}

	//XAML Island section

	// Initialize the Xaml Framework's corewindow for current thread
	WindowsXamlManager winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();

	// This DesktopWindowXamlSource is the object that enables a non-UWP desktop application 
	// to host UWP controls in any UI element that is associated with a window handle (HWND).
	DesktopWindowXamlSource desktopSource;
	// Get handle to corewindow
	auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();
	// Parent the DesktopWindowXamlSource object to current window
	check_hresult(interop->AttachToWindow(_hWnd));

	// This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.  
	HWND hWndXamlIsland = nullptr;
	// Get the new child window's hwnd 
	interop->get_WindowHandle(&hWndXamlIsland);
	// Update the xaml island window size becuase initially is 0,0
	SetWindowPos(hWndXamlIsland, 0, 200, 100, 800, 200, SWP_SHOWWINDOW);

	//Creating the Xaml content
	Windows::UI::Xaml::Controls::Grid xamlContainer;
	xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::LightGray() });
	Windows::UI::Xaml::Controls::ListView xamlList;
	xamlContainer.Children().Append(xamlList);
	Uri uri(L"http://aka.ms/cppwinrt");
	Windows::UI::Xaml::Controls::TextBlock tb;
	tb.Text(uri.AbsoluteUri().c_str());
	tb.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
	tb.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
	tb.FontSize(48);

	xamlList.Items().Append(tb);
	xamlContainer.UpdateLayout();
	desktopSource.Content(xamlContainer);
	//End XAML Island section

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);

	//Message loop:
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT messageCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	wchar_t greeting[] = L"Hello World in Win32!";
	RECT rcClient;

	switch (messageCode)
	{
	case WM_PAINT:
		if (hWnd == _hWnd)
		{
			// project requires to include gdi32.lib in header file
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 300, 5, greeting, wcslen(greeting));
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//creating main window  
	case WM_CREATE:
		_childhWnd = CreateWindowEx(0, L"ChildWClass", NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, _hInstance, NULL);
		return 0;
		// main window changed size 
	case WM_SIZE:
		// Get the dimensions of the main window's client 
		// area, and enumerate the child windows. Pass the 
		// dimensions to the child windows during enumeration. 

		GetClientRect(hWnd, &rcClient);
		MoveWindow(_childhWnd, 200, 200, 400, 500, TRUE);
		ShowWindow(_childhWnd, SW_SHOW);


		return 0;

		// Process other messages. 


	default:
		return DefWindowProc(hWnd, messageCode, wParam, lParam);
		break;
	}

	return 0;
}
