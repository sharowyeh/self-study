using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
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

namespace UIThreadInvokeExample_WPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private TextBlock txtBlock;
        private Button btnCreateWindow1;
        private Button btnCreateWindow2;

        public MainWindow()
        {
            InitializeComponent();
            int currentThreadId = Thread.CurrentThread.ManagedThreadId;
            int AppDispatcherThreadId = Application.Current.Dispatcher.Thread.ManagedThreadId;
            txtBlock = new TextBlock();
            txtBlock.Text = "Current thread ID: " + currentThreadId.ToString() + "\r\n" + "App dispatcher thread ID: " + AppDispatcherThreadId.ToString();
            ((Grid)this.Content).Children.Add(txtBlock);
            btnCreateWindow1 = new Button();
            btnCreateWindow1.Height = 28;
            btnCreateWindow1.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            btnCreateWindow1.Margin = new Thickness(0, 60, 0, 0);
            btnCreateWindow1.Content = "Create window in current thread";
            btnCreateWindow1.Click += btnCreateWindow_UseCurrentThread;
            ((Grid)this.Content).Children.Add(btnCreateWindow1);
            btnCreateWindow2 = new Button();
            btnCreateWindow2.Height = 28;
            btnCreateWindow2.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            btnCreateWindow2.Margin = new Thickness(0, 90, 0, 0);
            btnCreateWindow2.Content = "Create window in new thread";
            btnCreateWindow2.Click += btnCreateWindow_UseNewThread;
            ((Grid)this.Content).Children.Add(btnCreateWindow2);
        }

        private void btnCreateWindow_UseCurrentThread(object sender, RoutedEventArgs e)
        {
            this.Hide();
            MainWindow window = new MainWindow();
            window.Show();
            window.Closed += (obj, arg) =>
            {
                this.Show();
            };
        }

        private void btnCreateWindow_UseNewThread(object sender, RoutedEventArgs e)
        {
            this.Hide();
            Thread thread = new Thread(() =>
            {
                MainWindow window = new MainWindow();
                window.Show();
                window.Closed += (obj, arg) =>
                {
                    //Application.Current.Dispatcher.Invoke(() => { this.Show(); });
                    //System.Windows.Threading.Dispatcher.CurrentDispatcher.Invoke(() => { this.Show(); });
                    
                    this.Dispatcher.Invoke(() => { this.Show(); });

                    window.Dispatcher.InvokeShutdown();
                };
                System.Windows.Threading.Dispatcher.Run();
            });
            thread.IsBackground = true;
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
        }

    }
}
