using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Devices.Bluetooth;

namespace wpf_uap
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private DeviceWatcherHelper deviceWatcherHelper;
        private ObservableCollection<DeviceInformationDisplay> resultCollection =
            new ObservableCollection<DeviceInformationDisplay>();

        private BluetoothLEDevice bluetoothLeDevice;

        public MainWindow()
        {
            InitializeComponent();

            deviceWatcherHelper = new DeviceWatcherHelper(resultCollection, this.Dispatcher);

            resultsListView.ItemsSource = resultCollection;
            selectorComboBox.ItemsSource = DeviceSelectorChoices.DevicePickerSelectors;
            selectorComboBox.SelectedIndex = 0;
            connectDeviceButton.Click += ConnectDeviceButton_Click;
            resultsListView.SelectionChanged += ResultsListView_SelectionChanged;
        }

        private void ResultsListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = resultsListView.SelectedItem as DeviceInformationDisplay;
            if (item == null)
                return;
            selectedDeviceTextBlock.Text = $"{item.Name}[{item.Id}] ";
            selectedDeviceTextBlock.Tag = item.Id;
        }

        private async void ConnectDeviceButton_Click(object sender, RoutedEventArgs e)
        {
            if (bluetoothLeDevice != null)
            {
                bluetoothLeDevice.Dispose();
                bluetoothLeDevice = null;
            }

            if (selectedDeviceTextBlock.Tag == null)
            {
                Console.WriteLine("nothing selected");
                return;
            }

            bluetoothLeDevice = await BluetoothLEDevice.FromIdAsync(selectedDeviceTextBlock.Tag as string);
            if (bluetoothLeDevice == null)
            {
                Console.WriteLine("connect failed");
                return;
            }

            MessageBox.Show($"{bluetoothLeDevice.Name} to pair");
            var result = await bluetoothLeDevice.DeviceInformation.Pairing.PairAsync();
            MessageBox.Show($"{result.ProtectionLevelUsed.ToString()}");
        }

        private void StartWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StartWatcher();
        }

        private void StopWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StopWatcher();
        }

        private void StartWatcher()
        {
            //TODO: will get
            //  Exception thrown: 'System.InvalidOperationException' in PresentationCore.dll
            //  while watcher getting device added and invoke UI dispatcher

            startWatcherButton.IsEnabled = false;
            resultCollection.Clear();

            // First get the device selector chosen by the UI.
            DeviceSelectorInfo deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;

            DeviceWatcher deviceWatcher;
            if (null == deviceSelectorInfo.Selector)
            {
                // If the a pre-canned device class selector was chosen, call the DeviceClass overload
                deviceWatcher = DeviceInformation.CreateWatcher(deviceSelectorInfo.DeviceClassSelector);
            }
            else if (deviceSelectorInfo.Kind == DeviceInformationKind.Unknown)
            {
                // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
                // Kind will be determined by the selector
                deviceWatcher = DeviceInformation.CreateWatcher(
                    deviceSelectorInfo.Selector,
                    null // don't request additional properties for this sample
                    );
            }
            else
            {
                // Kind is specified in the selector info
                deviceWatcher = DeviceInformation.CreateWatcher(
                    deviceSelectorInfo.Selector,
                    null, // don't request additional properties for this sample
                    deviceSelectorInfo.Kind);
            }

            Console.WriteLine("Starting Watcher...");
            //rootPage.NotifyUser("Starting Watcher...", NotifyType.StatusMessage);
            deviceWatcherHelper.StartWatcher(deviceWatcher);
            stopWatcherButton.IsEnabled = true;
        }

        private void StopWatcher()
        {
            stopWatcherButton.IsEnabled = false;
            deviceWatcherHelper.StopWatcher();
            startWatcherButton.IsEnabled = true;
        }
    }
}
