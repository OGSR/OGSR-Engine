using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.IO;
using System.Threading;
using System.Diagnostics;
using System.ComponentModel;
using System.Net;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Collections;
using System.Runtime.InteropServices;

namespace Configurator
{
    public partial class Form1 : Form
    {
        /// <summary>
        /// Чтение строки с файла языка
        /// </summary>
        /// <param name="t1">Имя контрола</param>
        /// <param name="section">Секция</param>
        /// <returns>Возвращает строку</returns>
        public string read_string(string t1, string section)
        {
            string l = Properties.Settings.Default.lang; 
            string s = "asdas";
            Utils.INI.GetPrivateProfileString(section, t1, "sss", s, 1000, Application.StartupPath +"\\"+ l + ".lang");
            s = s.Replace("\n", "/n");
            return s;
        }
              
        public void load_leng()
        {
			if (!File.Exists("test.lang"))
				return;
            //watch.Start();
            List<string> lang = Utils.ReadData("test.lang");
            foreach (string t in lang)
            {
                try
                {
                    string[] temp1 = t.Split('=');
                    Control[] t1 = Controls.Find(temp1[0].Trim(), true);
                    string[] temp2 = temp1[1].Split('&');

                    if (temp2[0].Trim() != "...")
                    {
                        try
                        {
                            Label temp = t1[0] as Label;
                            temp.Text = temp2[0].Trim();
                        }
                        catch
                        {
                        }

                        try
                        {
                            CheckBox temp = t1[0] as CheckBox;
                            temp.Text = temp2[0].Trim();
                        }
                        catch
                        {
                        }

                        #region
                        try
                        {
                            TabPage temp = t1[0] as TabPage;
                            temp.Text = temp2[0].Trim();
                        }
                        catch
                        {
                        }
                        #endregion

                        #region
                        try
                        {
                            GroupBox temp = t1[0] as GroupBox;
                            temp.Text = temp2[0].Trim();
                        }
                        catch
                        {
                        }
                        #endregion

                        #region
                        try
                        {
                            Button temp = t1[0] as Button;
                            temp.Text = temp2[0].Trim();
                        }
                        catch
                        {
                        }
                        #endregion

                        #region
                        try
                        {
                            LinkLabel temp = t1[0] as LinkLabel;
                            temp.Text = temp2[0].Trim();
                        }
                        catch
                        {
                        }
                        #endregion

                    }

                    if (temp2[1].Trim() != "...")
                    {
                        string tt = temp2[1].Trim().Replace("|n", "\r\n");
                        toolTip1.SetToolTip(t1[0], tt);
                    }

                }
                catch
                {
                }
            }

        }


    }
}