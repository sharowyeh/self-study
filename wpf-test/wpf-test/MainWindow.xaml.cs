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

namespace wpf_test
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private BluetoothDiscovery discovery;
        public MainWindow()
        {
            InitializeComponent();

            discovery = new BluetoothDiscovery(this);

            ResultsListView.ItemsSource = discovery.BluetoothDevices;
            StartButton.Click += (sender, e) =>
            {
                discovery.StartDeviceWatcher();
            };
            StopButton.Click += (sender, e) =>
            {
                discovery.StopDeviceWatcher();
            };
            PairButton.Click += (sender, e) =>
            {
                var device = ResultsListView.SelectedItem as BluetoothDevice;
                discovery.PairDevice(device.Id);
            };

            WndMsgButton.Click += (sender, e) =>
            {
                if (regMsgId == 0)
                {
                    MessageBox.Show("RegisterWindowMessage failed\n");
                    return;
                }

                Win32.PostMessage(new IntPtr(0xffff), (int)regMsgId, new IntPtr(DateTime.Now.Second), IntPtr.Zero);
            };
        }

        private static uint regMsgId = 0;

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);

            // register window message for dispatch messages between apps
            regMsgId = Win32.RegisterWindowMessage(Win32.WndMsgString);
            WndMsgButton.Content = $"MsgId=0x{regMsgId:X}\nPost";

            // add window message hook for sender
            System.Windows.Interop.HwndSource source = PresentationSource.FromVisual(this) as System.Windows.Interop.HwndSource;
            source.AddHook(WndProc);
        }

        private IntPtr WndProc(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            // all instances(apps) who register the same message string will get messages via WndProc
            if (regMsgId != 0 && msg == regMsgId)
            {
                this.Dispatcher.Invoke(() =>
                {
                    WndMsgButton.Content = $"MsgId=0x{regMsgId:X}\nwp=0x{wParam.ToInt32():X}";
                }, System.Windows.Threading.DispatcherPriority.ContextIdle);
            }

            return IntPtr.Zero;
        }
    }
}
