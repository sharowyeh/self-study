using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace test_images_renderer_winform
{
    public partial class Form1 : Form
    {
        private BindingSource imageBindingSource = new BindingSource();
        private List<string> fileList = new List<string>();
        private Dictionary<int, Info> infoList = new Dictionary<int, Info>();

        public Form1()
        {
            InitializeComponent();

            imageBindingSource.DataSource = fileList;
            imageBindingSource.ListChanged += ImageBindingSource_ListChanged;
            listBox1.DataSource = imageBindingSource;
            listBox1.DrawMode = DrawMode.OwnerDrawVariable;
            listBox1.MeasureItem += ListBox1_MeasureItem;
            listBox1.DrawItem += listBox1_DrawItem;
            listBox1.SelectedIndexChanged += ListBox1_SelectedIndexChanged;
            button1.Click += Button1_Click;
            button2.Click += Button2_Click;
        }

        private static int s_exitcode = int.MinValue;

        private void Button2_Click(object sender, EventArgs e)
        {
            var procs = Process.GetProcessesByName("JFlash");
            if (procs.Length > 0)
            {
                MessageBox.Show("can not duplicate startup");
                return;
            }
            s_exitcode = int.MinValue;
            Task.Factory.StartNew(() =>
            {
                ProcessStartInfo psi = new ProcessStartInfo();
                psi.FileName = "C:\\Program Files\\SEGGER\\JLink\\JFlash.exe";
                psi.Arguments = $"-openprjwtf.prj -openwtf.fwbin -jflashlogwtflog.txt -auto -exit";
                psi.UseShellExecute = false;
                psi.CreateNoWindow = true;
                psi.RedirectStandardOutput = true;
                psi.RedirectStandardError = true;
                var p = Process.Start(psi);
                while (p.MainWindowHandle == IntPtr.Zero)
                {
                    System.Threading.Thread.Sleep(100);
                }
                //MessageBox.Show($"{p.MainWindowHandle.ToInt32():X}");
                // minimize
                ShowWindow(p.MainWindowHandle, 2);
                //this.Invoke(new Action(() => { this.Activate(); }));

                p.WaitForExit();
                // i just want to give you some dummy delay
                System.Threading.Thread.Sleep(1000);
                //MessageBox.Show($"{p.ExitCode}");
                s_exitcode = p.ExitCode;
            });

            while (s_exitcode == int.MinValue)
            {
                System.Threading.Thread.Sleep(100);
                // dirty workaround keep our UI receive behavior without jflash window impact
                this.Activate();
            }
            label1.Text = s_exitcode.ToString();
            //MessageBox.Show(taskRes.Result.ToString());
        }
        
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        private void Button1_Click(object sender, EventArgs e)
        {
            using (FolderBrowserDialog folderDialog = new FolderBrowserDialog())
            {
                // Can assign default selected directory path
                folderDialog.SelectedPath = AppDomain.CurrentDomain.BaseDirectory;

                if (folderDialog.ShowDialog() == DialogResult.OK)
                {
                    fileList.Clear();

                    string folderPath = folderDialog.SelectedPath;
                    // Retrieve file paths that match specific image extensions
                    string[] imageExtensions = { ".jpg", ".jpeg", ".png", ".bmp", ".gif" };
                    string[] filePaths = Directory.GetFiles(folderPath)
                                                  .Where(file => imageExtensions.Contains(Path.GetExtension(file).ToLower()))
                                                  .ToArray();

                    // Do something with the file paths, such as display them in a ListBox
                    fileList.AddRange(filePaths);
                    for (int i = 0; i < filePaths.Length; i++)
                    {
                        using (var image = Image.FromFile(filePaths[i]))
                        {
                            infoList[i] = new Info()
                            {
                                Width = image.Width,
                                Height = image.Height,
                                Aspect = (float)image.Width / (float)image.Height
                            }; 
                        }
                    }
                    imageBindingSource.ResetBindings(false);
                }
            }
        }

        private void ImageBindingSource_ListChanged(object sender, ListChangedEventArgs e)
        {
            listBox1.Invalidate();
        }

        private void ListBox1_MeasureItem(object sender, MeasureItemEventArgs e)
        {
            // Pre-allocate item position for scroll view
            if (infoList.ContainsKey(e.Index))
                e.ItemHeight = (int)(listBox1.Bounds.Width / infoList[e.Index].Aspect);
            else
                e.ItemHeight = listBox1.Bounds.Width; // given default square
        }

        private void listBox1_DrawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index >= 0 && e.Index < listBox1.Items.Count)
            {
                string imagePath = (string)listBox1.Items[e.Index];
                using (Image image = Image.FromFile(imagePath))
                {
                    float aspectRatio = (float)image.Width / (float)image.Height;
                    int imageWidth = e.Bounds.Width;
                    int imageHeight = (int)(imageWidth / aspectRatio);
                    e.Graphics.DrawImage(image, e.Bounds.X, e.Bounds.Y, imageWidth, imageHeight);
                }
            }
        }

        private void ListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            pictureBox1.ImageLocation = (string)listBox1.SelectedItem;
            // TODO: modify display or control size for image
        }
    }

    public class Info
    {
        public int Width { get; set; }
        public int Height { get; set; }
        public float Aspect { get; set; }
    }
}
