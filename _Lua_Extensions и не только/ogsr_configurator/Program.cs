using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Configurator
{
    static class Program
    {
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //try
           // {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Form1());
            //}
            //catch (Exception e)
            //{
                //MessageBox.Show(e.Message + "47782");
            //}
        }
    }
}
