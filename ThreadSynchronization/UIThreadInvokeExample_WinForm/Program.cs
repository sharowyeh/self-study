using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UIThreadInvokeExample_WinForm
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //The context should be null before UI created.
            var context = SynchronizationContext.Current;
            if (context == null)
                Console.WriteLine("Context is null before implement Form control.");
            Form1 form = new Form1();
            context = SynchronizationContext.Current;
            if (context != null)
                Console.WriteLine("Context has been created.");
            Application.Run(form);
        }
    }
}
