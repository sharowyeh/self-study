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

        public MainWindow()
        {
            InitializeComponent();

            deviceWatcherHelper = new DeviceWatcherHelper(resultCollection, this.Dispatcher);

            resultsListView.ItemsSource = resultCollection;
            selectorComboBox.ItemsSource = DeviceSelectorChoices.DevicePickerSelectors;
            selectorComboBox.SelectedIndex = 0;
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
