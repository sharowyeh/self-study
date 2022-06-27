#pragma once

#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>

class BluetoothDiscovery
{
public:
	BluetoothDiscovery();
	~BluetoothDiscovery();

	void echoHelloWorld();
	void pairDevice();
	void startDeviceWatcher();
	void stopDeviceWatcher();

private:
	std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> m_knownDevices;
	std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> unknownDevices;
	winrt::Windows::Devices::Enumeration::DeviceWatcher deviceWatcher{ nullptr };
	winrt::event_token deviceWatcherAddedToken;
	winrt::event_token deviceWatcherUpdatedToken;
	winrt::event_token deviceWatcherRemovedToken;
	winrt::event_token deviceWatcherEnumerationCompletedToken;
	winrt::event_token deviceWatcherStoppedToken;

	std::tuple<winrt::Windows::Devices::Enumeration::DeviceInformation, uint32_t> FindBluetoothLEDevice(winrt::hstring const& id);
	std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>::iterator FindUnknownDevices(winrt::hstring const& id);

	winrt::fire_and_forget DeviceWatcher_Added(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformation deviceInfo);
	winrt::fire_and_forget DeviceWatcher_Updated(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
	winrt::fire_and_forget DeviceWatcher_Removed(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
	winrt::fire_and_forget DeviceWatcher_EnumerationCompleted(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&);
	winrt::fire_and_forget DeviceWatcher_Stopped(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&);
};