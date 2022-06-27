using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UIThreadInvokeExample_WinForm
{
    public partial class Form1 : Form
    {
        private Button startButton;
        private Label displayLabel;

        public Form1()
        {
            InitializeComponent();

            this.startButton = new Button();
            this.startButton.Text = "Start";
            this.startButton.Location = new Point(10, 10);
            this.startButton.Size = new Size(100, 30);
            this.startButton.Click += startButton_Click;
            this.displayLabel = new Label();
            this.displayLabel.Text = "Click start to change text.";
            this.displayLabel.Location = new Point(10, 50);
            this.displayLabel.AutoSize = true;
            this.Controls.Add(this.startButton);
            this.Controls.Add(this.displayLabel);

            int threadId = Thread.CurrentThread.ManagedThreadId;
            Console.WriteLine("Form1 constructor thread id: " + threadId);
        }

        void startButton_Click(object sender, EventArgs e)
        {
            int threadId = Thread.CurrentThread.ManagedThreadId;
            Console.WriteLine("Button click event thread id: " + threadId);
            Thread thread1 = new Thread(RunWithSyncContext);
            var context = AsyncOperationManager.SynchronizationContext;
            thread1.Start(context);
            
            Thread thread2 = new Thread(RunWithControl);
            thread2.Start(displayLabel);

        }

        private void RunWithSyncContext(object context)
        {
            int threadId = Thread.CurrentThread.ManagedThreadId;
            Console.WriteLine("Thread1 id: " + threadId);
            Thread.Sleep(3000);
            SynchronizationContext syncContext = (SynchronizationContext)context;
            syncContext.Post((state) => 
            {
                int contextThreadId = Thread.CurrentThread.ManagedThreadId;
                Console.WriteLine("Context thread id: " + contextThreadId);
                displayLabel.Text = (string)state;
            }, "Thread1 finished.");
        }

        private void RunWithControl(object control)
        {
            int threadId = Thread.CurrentThread.ManagedThreadId;
            Console.WriteLine("Thread2 id: " + threadId);
            Thread.Sleep(5000);
            Label label = (Label)control;
            if (label.InvokeRequired)
            {
                label.Invoke(new Action<string>((state) => 
                {
                    int invokeThreadId = Thread.CurrentThread.ManagedThreadId;
                    Console.WriteLine("Invoke thread id: " + invokeThreadId);
                    displayLabel.Text = (string)state;
                }), "Thread2 finished.");
            }
                
        }
    }
}
