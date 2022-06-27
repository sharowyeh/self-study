using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;

namespace wpf_test
{ 
    public class BluetoothDiscovery
    {
        private DeviceWatcher deviceWatcher = null;
        private readonly System.Windows.UIElement element = null;
        private bool isPairing = false;

        public ObservableCollection<BluetoothDevice> BluetoothDevices { get; } = new ObservableCollection<BluetoothDevice>();

        public ObservableCollection<DeviceInformation> KnownDevices { get; } = new ObservableCollection<DeviceInformation>();

        public ObservableCollection<DeviceInformation> UnknownDevices { get; } = new ObservableCollection<DeviceInformation>();

        public BluetoothDiscovery(System.Windows.UIElement element)
        {
            this.element = element;
        }

        public void StartDeviceWatcher()
        {
            StopDeviceWatcher();

            // Additional properties we would like about the device.
            // Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx
            string[] requestedProperties = {
                //"System.Devices.Aep.Bluetooth.Cod.Services.Audio", "System.Devices.Aep.Bluetooth.Cod.Services.Rendering",
                "System.Devices.Aep.DeviceAddress", "System.Devices.Aep.IsConnected", "System.Devices.Aep.Bluetooth.Le.IsConnectable" };

            // BT_Code: Example showing paired and non-paired in a single query.
            string aqsAllBluetoothLEDevices =
                        //Windows.Devices.Bluetooth.BluetoothDevice.GetDeviceSelectorFromPairingState(false);
            //string aqsAllBluetoothLEDevices = 
                        "(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")";

            deviceWatcher =
                    DeviceInformation.CreateWatcher(
                        aqsAllBluetoothLEDevices,
                        requestedProperties,
                        DeviceInformationKind.AssociationEndpoint);

            // Register event handlers before starting the watcher.
            deviceWatcher.Added += DeviceWatcher_Added;
            deviceWatcher.Updated += DeviceWatcher_Updated;
            deviceWatcher.Removed += DeviceWatcher_Removed;
            deviceWatcher.EnumerationCompleted += DeviceWatcher_EnumerationCompleted;
            deviceWatcher.Stopped += DeviceWatcher_Stopped;

            // Start over with an empty collection.
            KnownDevices.Clear();
            BluetoothDevices.Clear();

            // Start the watcher. Active enumeration is limited to approximately 30 seconds.
            // This limits power usage and reduces interference with other Bluetooth activities.
            // To monitor for the presence of Bluetooth LE devices for an extended period,
            // use the BluetoothLEAdvertisementWatcher runtime class. See the BluetoothAdvertisement
            // sample for an example.
            deviceWatcher.Start();
        }

        public void StopDeviceWatcher()
        {
            if (deviceWatcher != null)
            {
                // Unregister the event handlers.
                deviceWatcher.Added -= DeviceWatcher_Added;
                deviceWatcher.Updated -= DeviceWatcher_Updated;
                deviceWatcher.Removed -= DeviceWatcher_Removed;
                deviceWatcher.EnumerationCompleted -= DeviceWatcher_EnumerationCompleted;
                deviceWatcher.Stopped -= DeviceWatcher_Stopped;

                // Stop the watcher.
                deviceWatcher.Stop();
                deviceWatcher = null;
            }
        }

        public async void PairDevice(string id)
        {
            if (isPairing)
                return;

            var exists = FindBluetoothDevice(id);
            if (exists == null)
                return;

            isPairing = true;
            Console.WriteLine("Pairing device " + exists.Id);

            DevicePairingResult result = await exists.DeviceInfo.Pairing.PairAsync();
            Console.WriteLine("Pairing result " + result.Status.ToString());
            if (result.Status == DevicePairingResultStatus.Paired || result.Status == DevicePairingResultStatus.AlreadyPaired)
            { }
            isPairing = false;
        }

        private BluetoothDevice FindBluetoothDevice(string id)
        {
            foreach (var device in BluetoothDevices)
            {
                if (device.Id == id)
                {
                    return device;
                }
            }
            return null;
        }

        private DeviceInformation FindKnownDevice(string id)
        {
            foreach (var device in KnownDevices)
            {
                if (device.Id == id)
                {
                    return device;
                }
            }
            return null;
        }

        private DeviceInformation FindUnknownDevice(string id)
        {
            foreach (var device in UnknownDevices)
            {
                if (device.Id == id)
                {
                    return device;
                }
            }
            return null;
        }

        private void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo)
        {
            Console.WriteLine(string.Format("Added {0} Name: {1}", deviceInfo.Id, deviceInfo.Name));

            if (sender != deviceWatcher)
                return;

            element.Dispatcher.Invoke(() =>
            {
                var exists = FindBluetoothDevice(deviceInfo.Id);
                if (exists == null)
                {
                    if (deviceInfo.Name != string.Empty)
                    {
                        BluetoothDevices.Add(new BluetoothDevice(deviceInfo));
                        KnownDevices.Add(deviceInfo);
                    }
                    else
                    {
                        UnknownDevices.Add(deviceInfo);
                    }
                }
            }, System.Windows.Threading.DispatcherPriority.Normal);
        }

        private void DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
        {
            Console.WriteLine(string.Format("Updated {0} Name: {1}", deviceInfoUpdate.Id, ""));

            if (sender != deviceWatcher)
                return;
            element.Dispatcher.Invoke(() =>
            {
                var exists = FindBluetoothDevice(deviceInfoUpdate.Id);
                if (exists != null)
                {
                    exists.Update(deviceInfoUpdate);
                    var known = FindKnownDevice(deviceInfoUpdate.Id);
                    if (known != null)
                    {
                        known.Update(deviceInfoUpdate);
                    }
                    return;
                }

                var unknown = FindUnknownDevice(deviceInfoUpdate.Id);
                if (unknown != null)
                {
                    unknown.Update(deviceInfoUpdate);
                    if (unknown.Name != string.Empty)
                    {
                        BluetoothDevices.Add(new BluetoothDevice(unknown));
                        KnownDevices.Add(unknown);
                        UnknownDevices.Remove(unknown);
                    }
                }
            }, System.Windows.Threading.DispatcherPriority.Normal);
        }

        private void DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
        {
            Console.WriteLine(string.Format("Removed {0} Name: {1}", deviceInfoUpdate.Id, ""));

            if (sender != deviceWatcher)
                return;
            element.Dispatcher.Invoke(() =>
            {
                var exists = FindBluetoothDevice(deviceInfoUpdate.Id);
                if (exists != null)
                {
                    BluetoothDevices.Remove(exists);
                }
                var known = FindKnownDevice(deviceInfoUpdate.Id);
                if (known != null)
                {
                    KnownDevices.Remove(known);
                }
                var unknown = FindUnknownDevice(deviceInfoUpdate.Id);
                if (unknown != null)
                {
                    UnknownDevices.Remove(unknown);
                }
            }, System.Windows.Threading.DispatcherPriority.Normal);
        }

        private void DeviceWatcher_EnumerationCompleted(DeviceWatcher sender, object e)
        {
            Console.WriteLine("Enumeration completed");

            if (sender != deviceWatcher)
                return;

            System.Windows.MessageBox.Show("Enumeration completed");
        }

        private void DeviceWatcher_Stopped(DeviceWatcher sender, object e)
        {
            Console.WriteLine("Enumeration stopped");

            if (sender != deviceWatcher)
                return;

            System.Windows.MessageBox.Show("Enumeration stopped");
        }
    }
}
