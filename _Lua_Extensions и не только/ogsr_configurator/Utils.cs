using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace Configurator
{
    public static class Utils
    {
        public static List<string> ReadData(string path)
        {
            List<string> dS = new List<string>();
            try
            {
                using (StreamReader f1 = new StreamReader(@path, Encoding.Default))
                {
                    while (f1.Peek() >= 0)
                    {
                        dS.Add(f1.ReadLine());
                    }
                    f1.Close();
                }
            }
            catch (Exception)
            {
                //MessageBox.Show(e.Message);
            }
            return dS;
        }

        public static bool WriteData(List<string> toFile, string path)
        {
            try
            {
                StreamWriter f1 = new StreamWriter(@path, false, Encoding.Default);
                foreach (string k in toFile)
                {
                    f1.WriteLine(k);
                }
                f1.Close();
                return true;
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString() + "45778");
            }

            return false;
        }

        /// <summary>
        /// Вспомогательная функция. Разделяет строку.
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static param read_params(string str)
        {
            param buf = new param();
            try
            {
                string[] st = str.Split(';');
                buf.name_of_param = st[0].Split('=')[0].Trim();
                if (buf.name_of_param == "start_time")
                {
                    buf.other = st[0].Split('=')[1];
                    return buf;
                }
                try
                {
                    buf.value = Convert.ToDecimal(st[0].Split('=')[1].Trim().Replace('.', ','));
                }
                catch
                {
                    buf.value_bool = Convert.ToBoolean(st[0].Split('=')[1].Trim());
                }
                buf.deskription = st[1];
                return buf;
            }
            catch
            { }
            return null;
        }

        /// <summary>
        /// структура для хранения строк с параметрами
        /// </summary>
        public class param
        {
            public string name_of_param;
            public Boolean value_bool;
            public decimal value;
            public string other, deskription;
        }

        /// <summary>
        /// Содержит функции для работы с INI-файлами
        /// </summary>
        public class INI
        {
            /// <summary>
            /// Чтение параметра с файла
            /// </summary>
            /// <param name="sSection">Секция</param>
            /// <param name="sKey">Ключ</param>
            /// <param name="sDefault">Возможно, стандартное значение...</param>
            /// <param name="sString">Значение</param>
            /// <param name="iSize">Размер в байтах, хз</param>
            /// <param name="sFile">Путь к файлу</param>
            /// <returns></returns>
            [DllImport("Kernel32.dll", CharSet = CharSet.Auto)]
            public static extern int GetPrivateProfileString(String sSection, String sKey, String sDefault, String sString, int iSize, String sFile);

            /// <summary>
            /// 
            /// </summary>
            /// <param name="sSection">Секция</param>
            /// <param name="sKey">Ключ</param>
            /// <param name="sString">Значение</param>
            /// <param name="sFile">Путь к файлу</param>
            /// <returns></returns>
            [DllImport("Kernel32.dll", CharSet = CharSet.Auto)]
            public static extern bool WritePrivateProfileString(String sSection, String sKey, String sString, String sFile);
        }

        public class Header
        {

        }

        public class IniFile
        {
            public string path;

            [DllImport("kernel32")]
            private static extern long WritePrivateProfileString(string section,
                string key, string val, string filePath);
            [DllImport("kernel32")]
            private static extern int GetPrivateProfileString(string section, string key,
                string def, StringBuilder retVal, int size, string filePath);

            /// <summary>
            /// INIFile Constructor.
            /// </summary>
            /// <PARAM name="INIPath"></PARAM>
            public IniFile(string INIPath)
            {
                path = INIPath;
            }

            /// <summary>
            /// Write Data to the INI File
            /// </summary>
            /// <PARAM name="Section"></PARAM>
            /// Section name
            /// <PARAM name="Key"></PARAM>
            /// Key Name
            /// <PARAM name="Value"></PARAM>
            /// Value Name
            public void IniWriteValue(string Section, string Key, string Value)
            {
                WritePrivateProfileString(Section, Key, Value, this.path);
            }

            /// <summary>
            /// Read Data Value From the Ini File
            /// </summary>
            /// <PARAM name="Section"></PARAM>
            /// <PARAM name="Key"></PARAM>
            /// <PARAM name="Path"></PARAM>
            /// <returns></returns>
            public string IniReadValue(string Section, string Key, string Default)
            {
                StringBuilder temp = new StringBuilder(255);
                int i = GetPrivateProfileString(Section, Key, Default, temp,
                                                255, this.path);
                return temp.ToString();

            }
        }
            
    }


    public static class OtherFunct
    {
        public static int load_old_ver(string t)
        {
            StreamReader f1 = new StreamReader(t + "ver.dat", Encoding.Default);
            int oldVer = Convert.ToInt32(f1.ReadLine().Split('.')[4].Trim());

            f1.Close();
            return oldVer;
        }

    }
}
