﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using System.Windows.Threading;
using Windows.UI.Core;

namespace wpf_uap
{
    internal class DeviceWatcherHelper
    {
        public DeviceWatcherHelper(
            ObservableCollection<DeviceInformationDisplay> resultCollection,
            Dispatcher dispatcher)
        {
            this.resultCollection = resultCollection;
            this.dispatcher = dispatcher;
        }

        public delegate void DeviceChangedHandler(DeviceWatcher deviceWatcher, string id);
        public event DeviceChangedHandler DeviceChanged;

        public DeviceWatcher DeviceWatcher => deviceWatcher;
        public bool UpdateStatus = true;

        public void StartWatcher(DeviceWatcher deviceWatcher)
        {
            this.deviceWatcher = deviceWatcher;

            // Connect events to update our collection as the watcher report results.
            deviceWatcher.Added += Watcher_DeviceAdded;
            deviceWatcher.Updated += Watcher_DeviceUpdated;
            deviceWatcher.Removed += Watcher_DeviceRemoved;
            deviceWatcher.EnumerationCompleted += Watcher_EnumerationCompleted;
            deviceWatcher.Stopped += Watcher_Stopped;

            deviceWatcher.Start();
        }

        public void StopWatcher()
        {
            // Since the device watcher runs in the background, it is possible that
            // a notification is "in flight" at the time we stop the watcher.
            // In other words, it is possible for the watcher to become stopped while a
            // handler is running, or for a handler to run after the watcher has stopped.

            if (IsWatcherStarted(deviceWatcher))
            {
                // We do not null out the deviceWatcher yet because we want to receive
                // the Stopped event.
                deviceWatcher.Stop();
            }
        }

        public void Reset()
        {
            if (deviceWatcher != null)
            {
                StopWatcher();
                deviceWatcher = null;
            }
        }

        DeviceWatcher deviceWatcher = null;
        ObservableCollection<DeviceInformationDisplay> resultCollection;
        //TODO:NOTE: change Windows.UI.Core.CoreDispatcher to System.Windows.Threading.Dispatcher for .NET Core
        // CoreDispatcher.RunAsync will be replaced by Dispatcher.InvokeAsync with DispatcherPriority.Background or .Input
        Dispatcher dispatcher;

        static bool IsWatcherStarted(DeviceWatcher watcher)
        {
            return (watcher.Status == DeviceWatcherStatus.Started) ||
                (watcher.Status == DeviceWatcherStatus.EnumerationCompleted);
        }

        public bool IsWatcherRunning()
        {
            if (deviceWatcher == null)
            {
                return false;
            }

            DeviceWatcherStatus status = deviceWatcher.Status;
            return (status == DeviceWatcherStatus.Started) ||
                (status == DeviceWatcherStatus.EnumerationCompleted) ||
                (status == DeviceWatcherStatus.Stopped);
        }

        private async void Watcher_DeviceAdded(DeviceWatcher sender, DeviceInformation deviceInfo)
        {
            // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
            //await dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            await dispatcher.InvokeAsync(() =>
            {
                // Watcher may have stopped while we were waiting for our chance to run.
                if (IsWatcherStarted(sender))
                {
                    resultCollection.Add(new DeviceInformationDisplay(deviceInfo));
                    RaiseDeviceChanged(sender, deviceInfo.Id);
                }
            }, DispatcherPriority.Background);
        }

        private async void Watcher_DeviceUpdated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
        {
            // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
            //await dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            await dispatcher.InvokeAsync(() =>
            {
                // Watcher may have stopped while we were waiting for our chance to run.
                if (IsWatcherStarted(sender))
                {
                    // Find the corresponding updated DeviceInformation in the collection and pass the update object
                    // to the Update method of the existing DeviceInformation. This automatically updates the object
                    // for us.
                    foreach (DeviceInformationDisplay deviceInfoDisp in resultCollection)
                    {
                        if (deviceInfoDisp.Id == deviceInfoUpdate.Id)
                        {
                            deviceInfoDisp.Update(deviceInfoUpdate);
                            RaiseDeviceChanged(sender, deviceInfoUpdate.Id);
                            break;
                        }
                    }
                }
            }, DispatcherPriority.Background);
        }

        private async void Watcher_DeviceRemoved(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
        {
            // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
            //await dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            await dispatcher.InvokeAsync(() =>
            {
                // Watcher may have stopped while we were waiting for our chance to run.
                if (IsWatcherStarted(sender))
                {
                    // Find the corresponding DeviceInformation in the collection and remove it
                    foreach (DeviceInformationDisplay deviceInfoDisp in resultCollection)
                    {
                        if (deviceInfoDisp.Id == deviceInfoUpdate.Id)
                        {
                            resultCollection.Remove(deviceInfoDisp);
                            break;
                        }
                    }

                    RaiseDeviceChanged(sender, deviceInfoUpdate.Id);
                }
            }, DispatcherPriority.Background);
        }

        private async void Watcher_EnumerationCompleted(DeviceWatcher sender, object obj)
        {
            //await dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            await dispatcher.InvokeAsync(() =>
            {
                RaiseDeviceChanged(sender, string.Empty);
            }, DispatcherPriority.Background);
        }

        private async void Watcher_Stopped(DeviceWatcher sender, object obj)
        {
            //await dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            await dispatcher.InvokeAsync(() =>
            {
                RaiseDeviceChanged(sender, string.Empty);
            }, DispatcherPriority.Background);
        }

        private void RaiseDeviceChanged(DeviceWatcher sender, string id)
        {
            if (UpdateStatus)
            {
                // Display a default status message.
                string message = string.Empty;
                switch (sender.Status)
                {
                    case DeviceWatcherStatus.Started:
                        message = $"{resultCollection.Count} devices found.";
                        break;

                    case DeviceWatcherStatus.EnumerationCompleted:
                        message = $"{resultCollection.Count} devices found. Enumeration completed. Watching for updates...";
                        break;

                    case DeviceWatcherStatus.Stopped:
                        message = $"{resultCollection.Count} devices found. Watcher stopped.";
                        break;

                    case DeviceWatcherStatus.Aborted:
                        message = $"{resultCollection.Count} devices found. Watcher aborted.";
                        break;
                }

                if (!string.IsNullOrEmpty(message))
                {
                    Console.Write(message);
                    //MainPage.Current.NotifyUser(message, NotifyType.StatusMessage);
                }
            }

            DeviceChanged?.Invoke(sender, id);
        }
    }
}
