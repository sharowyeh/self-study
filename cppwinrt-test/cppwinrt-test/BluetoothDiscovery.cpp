#include "pch.h"
#include "BluetoothDiscovery.h"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Core;

std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> m_knownDevices;

BluetoothDiscovery::BluetoothDiscovery()
{
}

BluetoothDiscovery::~BluetoothDiscovery()
{
}

void BluetoothDiscovery::echoHelloWorld() {
	winrt::Windows::Foundation::Uri uri(L"http://hello_world");
	OutputDebugStringW((uri.AbsoluteUri() + L"\n").c_str());
}

void BluetoothDiscovery::pairDevice() {

}

void BluetoothDiscovery::startDeviceWatcher() {
	// Additional properties we would like about the device.
		// Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx
	auto requestedProperties = single_threaded_vector<hstring>({ L"System.Devices.Aep.DeviceAddress", L"System.Devices.Aep.IsConnected", L"System.Devices.Aep.Bluetooth.Le.IsConnectable" });

	// BT_Code: Example showing paired and non-paired in a single query.
	hstring aqsAllBluetoothLEDevices = L"(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")";

	deviceWatcher =
		Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(
			aqsAllBluetoothLEDevices,
			requestedProperties,
			DeviceInformationKind::AssociationEndpoint);

	// Register event handlers before starting the watcher.
	/// Remove implicit this for winrt UI componments
	//deviceWatcherAddedToken = deviceWatcher.Added({ get_weak(), &bluetooth::DeviceWatcher_Added });
	deviceWatcherAddedToken = deviceWatcher.Added({ this, &BluetoothDiscovery::DeviceWatcher_Added });
	deviceWatcherUpdatedToken = deviceWatcher.Updated({ this, &BluetoothDiscovery::DeviceWatcher_Updated });
	deviceWatcherRemovedToken = deviceWatcher.Removed({ this, &BluetoothDiscovery::DeviceWatcher_Removed });
	deviceWatcherEnumerationCompletedToken = deviceWatcher.EnumerationCompleted({ this, &BluetoothDiscovery::DeviceWatcher_EnumerationCompleted });
	deviceWatcherStoppedToken = deviceWatcher.Stopped({ this, &BluetoothDiscovery::DeviceWatcher_Stopped });

	// Start over with an empty collection.
	m_knownDevices.clear();
	//m_knownDevices.Clear();

	// Start the watcher. Active enumeration is limited to approximately 30 seconds.
	// This limits power usage and reduces interference with other Bluetooth activities.
	// To monitor for the presence of Bluetooth LE devices for an extended period,
	// use the BluetoothLEAdvertisementWatcher runtime class. See the BluetoothAdvertisement
	// sample for an example.
	deviceWatcher.Start();
}

void BluetoothDiscovery::stopDeviceWatcher() {
	if (deviceWatcher != nullptr)
	{
		// Unregister the event handlers.
		deviceWatcher.Added(deviceWatcherAddedToken);
		deviceWatcher.Updated(deviceWatcherUpdatedToken);
		deviceWatcher.Removed(deviceWatcherRemovedToken);
		deviceWatcher.EnumerationCompleted(deviceWatcherEnumerationCompletedToken);
		deviceWatcher.Stopped(deviceWatcherStoppedToken);

		// Stop the watcher.
		deviceWatcher.Stop();
		deviceWatcher = nullptr;
	}
}

std::tuple<winrt::Windows::Devices::Enumeration::DeviceInformation, uint32_t> BluetoothDiscovery::FindBluetoothLEDevice(winrt::hstring const& id) {
	uint32_t size = m_knownDevices.size();
	//uint32_t size = m_knownDevices.Size();
	for (uint32_t index = 0; index < size; index++)
	{
		auto bleDevice = m_knownDevices.at(index).as<Windows::Devices::Enumeration::DeviceInformation>();
		//auto bleDevice = m_knownDevices.GetAt(index).as<Windows::Devices::Enumeration::DeviceInformation>();
		if (bleDevice.Id() == id)
		{
			return { bleDevice, index };
		}
	}
	return { nullptr, 0 - 1U };
}

std::vector<Windows::Devices::Enumeration::DeviceInformation>::iterator BluetoothDiscovery::FindUnknownDevices(winrt::hstring const& id) {
	return std::find_if(unknownDevices.begin(), unknownDevices.end(), [&](auto&& bleDeviceInfo)
	{
		return bleDeviceInfo.Id() == id;
	});
}

fire_and_forget BluetoothDiscovery::DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo) {
	// We must update the collection on the UI thread because the collection is databound to a UI element.
	/// Remove implicit this for winrt UI componments
	//auto lifetime = get_strong();
	//co_await resume_foreground(Dispatcher());

	OutputDebugStringW((L"Added ID:" + deviceInfo.Id() + L" NAME:" + deviceInfo.Name() + L"\n").c_str());

	// Protect against race condition if the task runs after the app stopped the deviceWatcher.
	if (sender == deviceWatcher)
	{
		// Make sure device isn't already present in the list.
		if (std::get<0>(FindBluetoothLEDevice(deviceInfo.Id())) == nullptr)
		{
			if (!deviceInfo.Name().empty())
			{
				// If device has a friendly name display it immediately.
				m_knownDevices.push_back(deviceInfo);
				//m_knownDevices.Append(deviceInfo);
			}
			else
			{
				// Add it to a list in case the name gets updated later. 
				unknownDevices.push_back(deviceInfo);
			}
		}
	}
	co_return;
}

fire_and_forget BluetoothDiscovery::DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate) {
	// We must update the collection on the UI thread because the collection is databound to a UI element.
	/// Remove implicit this for winrt UI componments
	//auto lifetime = get_strong();
	//co_await resume_foreground(Dispatcher());

	OutputDebugStringW((L"Updated " + deviceInfoUpdate.Id() + L"\n").c_str());

	// Protect against race condition if the task runs after the app stopped the deviceWatcher.
	if (sender == deviceWatcher)
	{
		DeviceInformation bleDevice = std::get<0>(FindBluetoothLEDevice(deviceInfoUpdate.Id()));
		if (bleDevice != nullptr)
		{
			// Device is already being displayed - update UX.
			bleDevice.Update(deviceInfoUpdate);
			co_return;
		}

		auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
		if (deviceInfo != unknownDevices.end())
		{
			deviceInfo->Update(deviceInfoUpdate);
			// If device has been updated with a friendly name it's no longer unknown.
			if (!deviceInfo->Name().empty())
			{
				m_knownDevices.push_back(*deviceInfo);
				//m_knownDevices.Append(*deviceInfo);
				unknownDevices.erase(deviceInfo);
			}
		}
	}
	co_return;
}

fire_and_forget BluetoothDiscovery::DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate) {
	// We must update the collection on the UI thread because the collection is databound to a UI element.
	/// Remove implicit this for winrt UI componments
	//auto lifetime = get_strong();
	//co_await resume_foreground(Dispatcher());

	OutputDebugStringW((L"Removed ID:" + deviceInfoUpdate.Id() + L"\n").c_str());

	// Protect against race condition if the task runs after the app stopped the deviceWatcher.
	if (sender == deviceWatcher)
	{
		// Find the corresponding DeviceInformation in the collection and remove it.
		auto[bleDevice, index] = FindBluetoothLEDevice(deviceInfoUpdate.Id());
		if (bleDevice != nullptr)
		{
			m_knownDevices.erase(m_knownDevices.begin() + index);
			//m_knownDevices.RemoveAt(index);
		}

		auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
		if (deviceInfo != unknownDevices.end())
		{
			unknownDevices.erase(deviceInfo);
		}
	}
	co_return;
}

fire_and_forget BluetoothDiscovery::DeviceWatcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&) {
	// Access this->deviceWatcher on the UI thread to avoid race conditions.
	/// Remove implicit this for winrt UI componments
	//auto lifetime = get_strong();
	//co_await resume_foreground(Dispatcher());

	// Protect against race condition if the task runs after the app stopped the deviceWatcher.
	if (sender == deviceWatcher)
	{

		OutputDebugStringW((winrt::to_hstring(m_knownDevices.size()) + L" known devices found. Enumeration completed.\n").c_str());
		::MessageBoxW(::GetDesktopWindow(), L"Enumeration completed", L"C++/WinRT Desktop Application", MB_OK);
		//rootPage.NotifyUser(to_hstring(m_knownDevices.Size()) + L" devices found. Enumeration completed.",
		//	NotifyType::StatusMessage);
	}
	co_return;
}

fire_and_forget BluetoothDiscovery::DeviceWatcher_Stopped(DeviceWatcher sender, Windows::Foundation::IInspectable const&) {
	// Access this->deviceWatcher on the UI thread to avoid race conditions.
	/// Remove implicit this for winrt UI componments
	//auto lifetime = get_strong();
	//co_await resume_foreground(Dispatcher());

	// Protect against race condition if the task runs after the app stopped the deviceWatcher.
	if (sender == deviceWatcher)
	{
		OutputDebugStringW(L"No longer watching for devices.\n");
		//rootPage.NotifyUser(L"No longer watching for devices.",
		//	sender.Status() == DeviceWatcherStatus::Aborted ? NotifyType::ErrorMessage : NotifyType::StatusMessage);
	}
	co_return;
}