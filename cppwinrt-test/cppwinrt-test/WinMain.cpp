#include "pch.h"
#include "BluetoothDiscovery.h"

using namespace winrt;
using namespace Windows::Foundation;

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    init_apartment(apartment_type::single_threaded);
	BluetoothDiscovery discovery = BluetoothDiscovery();
	discovery.echoHelloWorld();
	discovery.startDeviceWatcher();
    //Uri uri(L"http://aka.ms/cppwinrt");
    //::MessageBoxW(::GetDesktopWindow(), uri.AbsoluteUri().c_str(), L"C++/WinRT Desktop Application", MB_OK);
	::MessageBoxW(::GetDesktopWindow(), L"Block main thread for bluetooth device descovering", L"C++/WinRT Desktop Application", MB_OK);
	discovery.stopDeviceWatcher();
	::MessageBoxW(::GetDesktopWindow(), L"Stopped bluetooth device descovering", L"C++/WinRT Desktop Application", MB_OK);
}
