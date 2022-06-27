using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;
using Windows.Devices.Enumeration;
using System.IO;

namespace wpf_test
{
    /// <summary>
    /// BLE device model for WPF UI binding usage
    /// </summary>
    public class BluetoothDevice : INotifyPropertyChanged
    {
        public DeviceInformation DeviceInfo { get; private set; }
        public string Id => DeviceInfo.Id;
        public string Name => DeviceInfo.Name;
        public bool IsPaired => DeviceInfo.Pairing.IsPaired;
        public bool Connected /*=> (bool?)DeviceInfo.Properties["System.Device.Connected"] == true;*/
        {
            get
            {
                if (DeviceInfo.Properties.TryGetValue("System.Device.Connected", out object connected))
                {
                    return (bool)connected;
                }
                else
                {
                    return false;
                }
            }
        }
        /// <summary>
        /// Aep.IsConnected
        /// </summary>
        public bool IsConnected /*=> (bool?)DeviceInfo.Properties["System.Device.Aep.IsConnected"] == true;*/
        {
            get
            {
                if (DeviceInfo.Properties.TryGetValue("System.Device.Aep.IsConnected", out object connected))
                {
                    return (bool)connected;
                }
                else
                {
                    return false;
                }
            }
        }
        /// <summary>
        /// Aep.Bluetooth.Le.IsConnectable
        /// </summary>
        public bool IsConnectable /*=> (bool?)DeviceInfo.Properties["System.Devices.Aep.Bluetooth.Le.IsConnectable"] == true;*/
        {
            get
            {
                if (DeviceInfo.Properties.TryGetValue("System.Devices.Aep.Bluetooth.Le.IsConnectable", out object connected))
                {
                    return (bool)connected;
                }
                else
                {
                    return false;
                }
            }
        }
        public IReadOnlyDictionary<string, object> Properties => DeviceInfo.Properties;
        public BitmapImage GlyphBitmapImage { get; private set; }

        /// <summary>
        /// INotifyPropertyChanged event implementation
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;
        /// <summary>
        /// INotifyPropertyChanged event handler implementation
        /// </summary>
        /// <param name="name"></param>
        protected void OnPropertyChanged(string name)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

        public BluetoothDevice(DeviceInformation deviceInfo)
        {
            DeviceInfo = deviceInfo;
            UpdateGlyphBitmapImage();
        }

        public void Update(DeviceInformationUpdate deviceInfoUpdate)
        {
            DeviceInfo.Update(deviceInfoUpdate);

            OnPropertyChanged("Id");
            OnPropertyChanged("Name");
            OnPropertyChanged("DeviceInfo");
            OnPropertyChanged("IsPaired");
            OnPropertyChanged("Connected");
            OnPropertyChanged("IsConnected");
            OnPropertyChanged("IsConnectable");
            OnPropertyChanged("Properties");
            UpdateGlyphBitmapImage();
        }

        private async void UpdateGlyphBitmapImage()
        {
            if (GlyphBitmapImage == null)
                GlyphBitmapImage = new BitmapImage();

            try
            {
                DeviceThumbnail thumbnail = await DeviceInfo.GetThumbnailAsync();
                //MemoryStream ms = new MemoryStream();
                //int count = 0;
                //byte[] buffer = new byte[4196];
                //while ((count = thumbnail.AsStream().Read(buffer, 0, 4196)) > 0)
                //{
                //    ms.Write(buffer, 0, count);
                //}
                //ms.Position = 0;
                //GlyphBitmapImage.BeginInit();
                GlyphBitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                GlyphBitmapImage.StreamSource = thumbnail.AsStream();
                //GlyphBitmapImage.EndInit();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message + "\n" + e.StackTrace);
            }
            OnPropertyChanged("GlyphBitmapImage");
        }
    }
}
