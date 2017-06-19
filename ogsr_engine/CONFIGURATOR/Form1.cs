using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Net;
using System.Net.Mime;
using System.Net.Mail;
using System.IO;
using System.Threading;
using System.Diagnostics;
using System.Reflection;
using System.Globalization;
using System.Resources;

namespace Configurator
{
    enum E_LANG
    {
        [Description("rus")]
        RU,

        [Description("eng")]
        EN,

        [Description("fra")]
        FR,

        [Description("ger")]
        GR,
    }
	public partial class Form1 : Form
	{
		#region constatnts
		//string _lang = "rus";
		static public List<string> Parametrs;
		static public List<string> Actor;
        // for localization
        static public string lang = "ru-RU";
        static public ResourceManager res;
		#endregion

		public Form1()
		{
            // load program config
            try
            {
                load_lang_setting();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                this.Close();
            }
            // got language, initing
            Thread.CurrentThread.CurrentUICulture = new CultureInfo(lang);
            res = new ResourceManager("Configurator.Resources."+lang, Assembly.GetExecutingAssembly());

			InitializeComponent();
            FormConstruct();
            UpdateLangPics();
		}
        private void FormConstruct()
        {
            try
            {
                read_fsgame();
                load_other();
                set_image();
                load_list_saves();
 //               load_leng();

                #region About
                try
                {
                    about_mod.LoadFile("localization//" + lang + "//ogse_about_mod.rtf", RichTextBoxStreamType.RichText);
                }
                catch
                {
                    about_mod.Text = res.GetString("CANT_FIND_FILE") + " ogse_about_mod.rtf";
                }
                try
                {
                    team.LoadFile("localization//" + lang + "//ogse_about.rtf", RichTextBoxStreamType.RichText);
                }
                catch
                {
                    team.Text = res.GetString("CANT_FIND_FILE") + " ogse_about.rtf";
                }
                try
                {
                    features.LoadFile("localization//" + lang + "//ogse_features.rtf", RichTextBoxStreamType.RichText);
                }
                catch
                {
                    features.Text = res.GetString("CANT_FIND_FILE") + " ogse_features.rtf";
                }
                #endregion

                var cfg = new OgseConfig();
                OgseConfigSerializer.Instance.Load(cfg);
                this.propertyGrid1.SelectedObject = cfg;
                this.propertyGrid1.HelpVisible = true;
                var cfg2 = new OgseGraphics();
                OgseGraphicsSerializer.Instance.Load(Data.ShaderConfigPath, cfg2);
                this.propertyGrid2.SelectedObject = cfg2;
                string tmp = new string('\r', 29);
                Utils.INI.GetPrivateProfileString("mod_ver", "mod_ver", "", tmp, 30, Data.VersionPath);
                // test
                string test_for_morons = new string('\r', 29);
                if (Utils.INI.GetPrivateProfileString("mod_ver", "mod_ver", "", test_for_morons, 30, Data.RootAppPath) != 0)
                    File.Copy(Environment.CurrentDirectory + "\\ogse_config.ltx", Environment.CurrentDirectory + "\\gamedata\\config\\tuning\\ogse_config.ltx", true);
                // test end

                // защита от удаления/отсутствия локализации
                if (!File.Exists(Environment.CurrentDirectory + "\\gamedata.db_xlocale"))
                    File.Copy(Environment.CurrentDirectory + "\\localization\\" + lang + "\\gamedata.db_xlocale", Environment.CurrentDirectory + "\\gamedata.db_xlocale", true);

                // specific code for first launch after patch 2.07/8
                if (File.Exists(Environment.CurrentDirectory + "\\patch_2_08_cleanup"))
                {
                    OgseConfigSerializer.Instance.Save(this.propertyGrid1.SelectedObject as OgseConfig);
                    var cfg_new = new OgseConfig();
                    OgseConfigSerializer.Instance.Load(cfg_new);
                    this.propertyGrid1.SelectedObject = cfg_new;
                    File.Copy(Environment.CurrentDirectory + "\\localization\\" + lang + "\\gamedata.db_xlocale", Environment.CurrentDirectory + "\\gamedata.db_xlocale", true);
                    if (Directory.Exists(Data.LogsPath) && Directory.Exists(Data.ShaderCache))
                        Directory.Delete(Data.ShaderCache, true);
                    
                    // specific code for 2.10 revised
                    //gamedata/config/tuning
                    if (Directory.Exists(Environment.CurrentDirectory + "\\gamedata\\config\\tuning\\") && File.Exists(Environment.CurrentDirectory + "\\gamedata\\config\\tuning\\ogse_config.ltx"))
                        File.Copy(Environment.CurrentDirectory + "\\gamedata\\config\\tuning\\ogse_config.ltx", Environment.CurrentDirectory + "\\gamedata_new\\config\\tuning\\ogse_config.ltx", true);

                    // gamedata/shaders
                    if (Directory.Exists(Environment.CurrentDirectory + "\\gamedata\\shaders\\r2\\") && File.Exists(Environment.CurrentDirectory + "\\gamedata\\shaders\\r2\\configurator_defines.h"))
                        File.Copy(Environment.CurrentDirectory + "\\gamedata\\shaders\\r2\\configurator_defines.h", Environment.CurrentDirectory + "\\gamedata_new\\shaders\\r2\\configurator_defines.h", true);

                    Directory.Delete(Environment.CurrentDirectory + "\\gamedata\\", true);
                    Directory.Move(Environment.CurrentDirectory + "\\gamedata_new\\", Environment.CurrentDirectory + "\\gamedata\\");

                    if (!File.Exists(Environment.CurrentDirectory + "\\gamedata\\config\\tuning\\ogse_config.ltx"))
                        File.Copy(Environment.CurrentDirectory + "\\ogse_config.ltx", Environment.CurrentDirectory + "\\gamedata\\config\\tuning\\ogse_config.ltx", true);
                    if (!File.Exists(Environment.CurrentDirectory + "\\gamedata\\shaders\\r2\\configurator_defines.h"))
                        File.Copy(Environment.CurrentDirectory + "\\configurator_defines.h", Environment.CurrentDirectory + "\\gamedata\\shaders\\r2\\configurator_defines.h", true);

                    File.Delete(Environment.CurrentDirectory + "\\patch_2_08_cleanup");
                }

                ApplyButton.Enabled = false;
                this.button2.Enabled = false;
                this.button3.Enabled = false;
                if (Directory.Exists(Data.SavesDir))
                {
                    File.Delete(Data.SavesDir + "\\wreset");
                    this.button4.Enabled = true;
                }
                else
                    this.button4.Enabled = false;
                this.label6.Text = tmp;
                this.label7.Text = "";
                this.label8.Text = "";
                this.label9.Visible = false;
                this.progressBar2.Visible = false;
                this.richTextBox1.Enabled = false;

    //            find_upd();

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                this.Close();
            }
        }

		#region fsgame.ltx

		string TranslatePath(string src, Dictionary<string, string> envVars)
		{
			string result = src;
			while (true)
			{
				bool isReplaced = false;
				foreach (var el in envVars)
				{
					string s = result.Replace(el.Key, el.Value);
					if (s != result)
					{
						isReplaced = true;
						result = s;
						break;
					}
				}
				if (!isReplaced)
					break;
			}
			return Path.GetFullPath(result);
		}

		void read_fsgame()
		{
			var envVars = new Dictionary<string, string>
			{
			    {"$fs_root$", Environment.CurrentDirectory + Path.DirectorySeparatorChar}
			};

		    using (var f1 = new StreamReader(@"fsgame.ltx", Encoding.Default))
			{
				while (!f1.EndOfStream)
				{
					string line = f1.ReadLine();
					string[] str = line.Split(new char[] { '=', '|' });
					string name = str[0].Trim();
					string value = str[3].Trim();
					if (str.Length >= 5)
						value = Path.Combine(value, str[4].Trim());
					envVars.Add(name, value);
					switch (name)
					{
						case "$app_data_root$":
							Data.DocumentsDir = TranslatePath(value, envVars);
							break;
						case "$game_data$":
							Data.GameDataPath = TranslatePath(value, envVars);
							Data.ActorLtxFilePath = Path.Combine(Data.GameDataPath, "config\\creatures\\actor.ltx");
							break;
						case "$game_saves$":
							Data.SavesDir = TranslatePath(value, envVars);
							break;
						case "$logs$":
							Data.LogsPath = TranslatePath(value, envVars);
                            Data.ShaderCache = Path.Combine(Data.LogsPath, "shader_cache");
							break;
					}
				}
				Data.RootAppPath = Data.GameDataPath + @"config\\tuning\\ogse_config.ltx";
                Data.VersionPath = Data.GameDataPath + @"config\\tuning\\ogse_version.ltx";
                Data.ShaderConfigPath = Data.GameDataPath + @"shaders\\r2\\configurator_defines.h";
				f1.Close();
			}
		}

		#endregion

		#region Other parametrs

		private void set_image()
		{
			var t = DateTime.Now;
			if ((t.Hour > 8) && (t.Hour < 20))
			{
				pictureBox_day.Visible = true;
				pictureBox_night.Visible = false;
			} else
			{
				pictureBox_day.Visible = false;
				pictureBox_night.Visible = true;
			}
		}

		#endregion

		#region Настройки параметров запуска

		void load_other()
		{
			FileInfo launch = new FileInfo(Application.StartupPath+ @"\\launch.ltx");
			FileInfo launchBuc = new FileInfo(Data.GameDataPath + "buc\\launch.ltx");
			if (!launch.Exists)
			{
				launchBuc.CopyTo(launch.FullName);
			}

			List<string> launchParamsList = new List<string>();
			try
			{
				StreamReader f1 = new StreamReader(@"launch.ltx", Encoding.Default);
				while (f1.Peek() >= 0)
				{
					launchParamsList.Add(f1.ReadLine());
				}
				f1.Close();
			}
			catch (Exception e)
			{
				MessageBox.Show(e.Message+"458");
				return;
			}
			string[] k1 = launchParamsList[1].Split('=');
			string[] k2 = launchParamsList[2].Split('=');
			string[] k3 = launchParamsList[3].Split('=');
			string[] k4 = launchParamsList[4].Split('=');
			string[] k5 = launchParamsList[5].Split('=');
            string[] k6 = launchParamsList[6].Split('=');
			string k7 = launchParamsList[7];
			string[] k8 = new string[] {"disasm", "0"};
			if (launchParamsList.Count > 8)
				k8 = launchParamsList[8].Split('=');

            lang = k1[1];
			dsound.Checked = int.Parse(k2[1]) != 0;
			nointro.Checked = int.Parse(k3[1]) != 0;
			ss_tga.Checked = int.Parse(k4[1]) != 0;
			noprefetch.Checked = int.Parse(k5[1]) != 0;
			center_screen.Checked = int.Parse(k6[1]) != 0;
			switch (k7)
			{
				case "-smap0":
					sh_quality.Value = 0;
					break;
				case "-smap2560":
					sh_quality.Value = 1;
					break;
				case "-smap3072":
					sh_quality.Value = 2;
					break;
				case "-smap4096":
					sh_quality.Value = 3;
					break;
                case "-smap8192":
			        sh_quality.Value = 4;
					break;
			}
			disasm.Checked = int.Parse(k8[1]) != 0;
            reload_string();
		}
        void load_lang_setting()
        {
            FileInfo launch = new FileInfo(Application.StartupPath + @"\\launch.ltx");
            FileInfo launchBuc = new FileInfo(Data.GameDataPath + "buc\\launch.ltx");
            if (!launch.Exists)
            {
                launchBuc.CopyTo(launch.FullName);
            }

            List<string> launchParamsList = new List<string>();
            try
            {
                StreamReader f1 = new StreamReader(@"launch.ltx", Encoding.Default);
                while (f1.Peek() >= 0)
                {
                    launchParamsList.Add(f1.ReadLine());
                }
                f1.Close();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message + "458");
                return;
            }
            string[] k1 = launchParamsList[1].Split('=');
            lang = k1[1];
        }

		void save_other()
		{
			var launchParamsList = new List<string>();
			launchParamsList.Add("[launch]");

			string ShQuality;
			switch (sh_quality.Value)
			{
				case 1:
					ShQuality = "-smap2560";
					break;
				case 2:
					ShQuality = "-smap3072";
					break;
				case 3:
					ShQuality = "-smap4096";
                    break;
                case 4:
                    ShQuality = "-smap8192";
                    break;
				default:
					ShQuality = "-smap0";
					break;
			}
            launchParamsList.Add("lang="+lang);
			launchParamsList.Add(dsound.Checked ? "dsound=1" : "dsound=0");
			launchParamsList.Add(nointro.Checked ? "nointro=1" : "nointro=0");
			launchParamsList.Add(ss_tga.Checked ? "ss_tga=1" : "ss_tga=0");
			launchParamsList.Add(noprefetch.Checked ? "noprefetch=1" : "noprefetch=0");
			launchParamsList.Add(center_screen.Checked ? "center_screen=1" : "center_screen=0");
			launchParamsList.Add(ShQuality);
			launchParamsList.Add(disasm.Checked ? "disasm=1" : "disasm=0");

			try
			{
				var f1 = new StreamWriter(@"launch.ltx", false, Encoding.Default);
				foreach (string t in launchParamsList)
				{
					f1.WriteLine(t);
				}

				f1.Close();
			}
			catch (Exception e)
			{
				MessageBox.Show(e.Message+"471");
			}
		}

		private void noprefetch_CheckedChanged(object sender, EventArgs e)
		{
			reload_string();
			ApplyButton.Enabled = true;
		}

		private void sh_quality_ValueChanged(object sender, EventArgs e)
		{
			reload_string();
			ApplyButton.Enabled = true;
		}

		#endregion

		#region логика кнопок

		private void OgseLinkLinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			Process.Start("http://forum.ogse.ru/index.php");
		}

		private void ExitButtonClick(object sender, EventArgs e)
		{
			Close();
		}

		private void ApplyButtonClick(object sender, EventArgs e)
        {
            ApplyButton.Enabled = false;
            
            try
            {
                SaveSettings();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
		}

		private void StartGameClick(object sender, EventArgs e)
		{
			SaveAndStartGame();
		}

		private void SaveAndStartGame()
		{
		    try
		    {
                SaveSettings();
		    }
		    catch (Exception ex)
		    {
		        MessageBox.Show(ex.ToString());
		    }
			

			//если открыта вкладка с сохранениями, то запускаем сразу загрузку сейва
			if (tabpage.SelectedTab == tabPage12)
			{
				if (SavesList.SelectedItems.Count != 0)
				{
					start_string.Text += " -start server(" + SavesList.SelectedItems[0].Text + "/single/alife/load)  ";
				}
			}
			StartGame();
		}

		private void StartGame()
		{
		    ExecuteString(start_string.Text);
			Close();
		}

		//по двойному клику по сейву запускаем игру
		private void SavesListDoubleClick(object sender, EventArgs e)
		{
			SaveAndStartGame();
		}

		#endregion

		public void ExecuteString(string param)
		{
			var p = new Process {StartInfo = new ProcessStartInfo(@"bin\\XR_3DA.exe", param)};
			p.Start();
		}

		private void SaveSettings()
		{
			save_other();
			OgseConfigSerializer.Instance.Save(propertyGrid1.SelectedObject as OgseConfig);
            OgseGraphicsSerializer.Instance.Save(propertyGrid2.SelectedObject as OgseGraphics);
		}

		private void reload_string()
		{
			start_string.Text = "";

			if (dsound.Checked) start_string.Text += "-dsound";
			if (nointro.Checked) start_string.Text += " -nointro";
			if (ss_tga.Checked ) start_string.Text += " -ss_tga";
			if (center_screen.Checked) start_string.Text += " -center_screen";
			if (noprefetch.Checked) start_string.Text += " -noprefetch";
            if (disasm.Checked)
            {
                if (Directory.Exists(Data.LogsPath) && Directory.Exists(Data.ShaderCache))
                    Directory.Delete(Data.ShaderCache, true);
                start_string.Text += " -disasm";
            }

			switch (sh_quality.Value)
			{
				case 1:
					start_string.Text = start_string.Text + " -smap2560";
					break;
				case 2:
					start_string.Text = start_string.Text + " -smap3072";
					break;
				case 3:
					start_string.Text = start_string.Text + " -smap4096";
                    break;
                case 4:
                    start_string.Text = start_string.Text + " -smap8192";
                    break;
				default:
					start_string.Text = start_string.Text + "";
					break;
			}
		}

		//Пересмотреть этот модуль. В таком виде УЖЕ стал нерабочим, т.к. нет используемых там средст для отправки логов.
		#region Лог
		readonly SmtpClient client = new SmtpClient("smtp.mail.ru", 25);

		void send_mess()
		{

			client.Credentials = new NetworkCredential("ogseLogs@mail.ru", "Ls5sfau2Dk");

			MailMessage message = new MailMessage();
			message.From = new MailAddress("ogseLogs@mail.ru");
			message.To.Add(new MailAddress("ogseLogs@mail.ru"));
			message.Subject ="A "+name.Text ;
			message.Body = text.Text;
			message.SubjectEncoding = Encoding.UTF8;
			message.BodyEncoding = Encoding.UTF8;

			#region log


			if (load_log.Checked == true)
			{
				//Прикрепляем файл
				try
				{
					// Добавляем информацию для файла
					Attachment att = new Attachment(Data.LogsPath + "_ogse.log");
					att.TransferEncoding = TransferEncoding.QuotedPrintable;
					message.Attachments.Add(att);
				}
				catch
				{
					try
					{
						Attachment att = new Attachment(@"C:\Documents and Settings\All Users\Документы\STALKER-SHOC\logs\" + "_ogse.log");
						att.TransferEncoding = TransferEncoding.QuotedPrintable;
						message.Attachments.Add(att);
					}
					catch { }
				}
				message.Priority = MailPriority.High;
			}
			#endregion

			#region files
			if (file1.Text != "")
			{
				try
				{
					Attachment t = new Attachment(file1.Text);
					t.TransferEncoding = TransferEncoding.QuotedPrintable;
					message.Attachments.Add(t);
				}
				catch
				{
				}
			}
			#endregion

			try
			{
				if (text.Text == "")
				{
					MessageBox.Show("Пожалуйста, введите описание события.");
					return;
				}
				string userState = "test_message";
				client.SendCompleted += new SendCompletedEventHandler(SendCompletedCallback);

				client.SendAsync(message, userState);
			}
			catch (Exception exception)
			{
				MessageBox.Show(exception.Message + "4421");
			}
		}

		private void SendCompletedCallback(object sender, AsyncCompletedEventArgs e)
		{
			String token = (string)e.UserState;

			if (e.Cancelled)
			{
				MessageBox.Show("Отправка отменена");
			}
			if (e.Error != null)
			{
				MessageBox.Show("Ошибка отправки\n" + e.Error);
			}
			else
			{
			text.Text = "";
			}
		}

		private void button8_Click(object sender, EventArgs e)
		{
			try
			{
				client.SendAsyncCancel();
				MessageBox.Show("Отправка отменена");
			}
			catch { }
		}

		private void button5_Click(object sender, EventArgs e)
		{
			send_mess();
		}

		private void Button_Click(object sender, EventArgs e)
		{
			try
			{
				var f = new OpenFileDialog
				{
					InitialDirectory = Data.SavesDir,
					Filter = "Файлы сохранений (*.sav)|*.sav",
					RestoreDirectory = true
				};

				if (f.ShowDialog() == DialogResult.OK)
				{
					try
					{
						file1.Text = f.FileName;
					}
					catch (Exception ex)
					{
						MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
					}
				}
			}
			catch
			{
				var f = new OpenFileDialog
				{
					InitialDirectory = Data.SavesDir,
					Filter = "Файлы сохранений (*.sav)|*.sav",
					RestoreDirectory = true
				};

				if (f.ShowDialog() == DialogResult.OK)
				{
					try
					{
						file1.Text = f.FileName;
					}
					catch (Exception ex)
					{
						MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
					}
				}
			}
		}

		private void saves_SelectedIndexChanged(object sender, EventArgs e)
		{
			try
			{
				FileInfo fi = new FileInfo(Data.SavesDir + saves.SelectedItem.ToString());
				if (fi.Exists)
					file1.Text = fi.ToString();
				else
				{
					fi = new FileInfo(@"C:\Documents and Settings\All Users\Документы\STALKER-SHOC\savedgames\" + saves.SelectedItem.ToString());
					if (fi.Exists)
						file1.Text = fi.ToString();
				}


			}
			catch { }
		}

		void load_list_saves()
		{
			SavesList.Columns[0].Width = SavesList.ClientSize.Width - SavesList.Columns[1].Width;
			try
			{
				DirectoryInfo di = new DirectoryInfo(Data.SavesDir);
				FileInfo[] fi = di.GetFiles("*.sav");
				var sorted = fi.OrderByDescending(x => x.LastWriteTime);
				foreach (FileInfo t in sorted)
				{
					saves.Items.Add(t);
					string str = t.Name.Replace(".sav", "");
					SavesList.Items.Add(new ListViewItem(new string[] { str, t.LastWriteTime.ToString() }));
				}
			}
			catch
			{
				//MessageBox.Show("Не найдена папка с сохранениями по пути C:\\Documents and Settings\\All Users\\Документы\\STALKER-SHOC\\savedgames\\");
			}
		}

		#endregion

		#region Update
        string old_ver = new string('\r', 30);
        string new_ver;
        string update_link;
		string[] data;
        string filename;
        string description_ru;
        string description_en;
		WebClient load = new WebClient();

		void find_upd()
		{
            Utils.INI.GetPrivateProfileString("mod_ver", "mod_ver", "", old_ver, 100, Data.VersionPath);
            old_ver = old_ver.Replace('\0', '\r');
			WebClient user = new WebClient();
			try
			{
                user.Credentials = new NetworkCredential();
                data = Encoding.UTF8.GetString(user.DownloadData("http://ogsr-mod.org:750/new_version.txt")).Split('\n');
			}
			catch
			{
                MessageBox.Show(res.GetString("no_internet"));
				return;
			}
            new_ver = data[1].Split('=')[1] + '\r';
            filename = data[2].Split('=')[1];
            update_link = data[3].Split('=')[1];
            description_ru = data[4].Split('=')[1];
            description_en = data[5].Split('=')[1];
            this.label7.Text = new_ver;
			if (old_ver != new_ver)
			{
                this.button2.Enabled = true;
                this.label8.Text = res.GetString("update_available");
				this.richTextBox1.Enabled = true;
                if (lang == "ru-RU")
                    this.richTextBox1.Text = Encoding.UTF8.GetString(user.DownloadData("http://ogsr-mod.org:750/patch_description.txt"));
                else
                    this.richTextBox1.Text = Encoding.UTF8.GetString(user.DownloadData("http://ogsr-mod.org:750/patch_description_en.txt"));
			}
			else
			{
                this.label8.Text = res.GetString("no_update");
                this.label8.ForeColor = System.Drawing.Color.Green;
			}

		}

		//Файл загружен
		public void Handler(Object sender,AsyncCompletedEventArgs e)
		{
			this.button3.Enabled = true;
            this.button2.Enabled = true;
            this.button3.Click += button3_Click;
            MessageBox.Show(res.GetString("patch_downloaded"));
		}

		public void progress(Object sender, DownloadProgressChangedEventArgs e)
		{
			progressBar2.Value = e.ProgressPercentage;
		}

		#endregion

		private void SavesList_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (SavesList.SelectedItems.Count == 0)
			{
				savePicture.Image = null;
				return;
			}
			string picPath = Path.Combine(Data.SavesDir, Path.ChangeExtension(SavesList.SelectedItems[0].Text, ".dds"));
			if (File.Exists(picPath))
			{
				using (BinaryReader r = new BinaryReader(new FileStream(picPath, FileMode.Open)))
				{
					byte[] data = r.ReadBytes((int)r.BaseStream.Length);
					var img = new KUtility.DDSImage(data);
					savePicture.Image = img.images[0];
				}
			}
			else
			{
				savePicture.Image = null;
			}
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			if (Properties.Settings.Default.Inited)
			{
				RestorePos();
			}
            RestoreGridSplitterPos(this.propertyGrid1, Properties.Settings.Default.GridSplitterPos, Properties.Settings.Default.GridCommentHeight);
            RestoreGridSplitterPos(this.propertyGrid2, Properties.Settings.Default.GridSplitterPos2, Properties.Settings.Default.GridCommentHeight2);
		}

		private void Form1_FormClosing(object sender, FormClosingEventArgs e)
		{
			SavePos();
            int pos, height;
            SaveGridSplitterPos(this.propertyGrid1, out pos, out height);
            Properties.Settings.Default.GridSplitterPos = pos;
            Properties.Settings.Default.GridCommentHeight = height;
            SaveGridSplitterPos(this.propertyGrid2, out pos, out height);
            Properties.Settings.Default.GridSplitterPos2 = pos;
            Properties.Settings.Default.GridCommentHeight2 = height;
			Properties.Settings.Default.Inited = true;
			Properties.Settings.Default.Save();
		}
		#region App settings save/restore
		/// <summary>
		/// Сохранение положения разделителя в гриде
		/// </summary>
		private void SaveGridSplitterPos(PropertyGrid grid, out int pos, out int height)
		{
            Type type = grid.GetType();
			FieldInfo field = type.GetField("gridView",
			  BindingFlags.NonPublic | BindingFlags.Instance);

            object valGrid = field.GetValue(grid);
			Type gridType = valGrid.GetType();
			pos = (int)gridType.InvokeMember(
			  "GetLabelWidth",
			  BindingFlags.Public | BindingFlags.InvokeMethod | BindingFlags.Instance,
			  null,
			  valGrid, new object[] { });

            height = 71;
            foreach (Control control in grid.Controls)
				if (control.GetType().Name == "DocComment")
				{
                    height = control.Height;
					break;
				}
		}

		/// <summary>
		/// Восстановление положения разделителя в гриде
		/// </summary>
        private void RestoreGridSplitterPos(PropertyGrid grid, int pos, int height)
		{
			try
			{
                Type type = grid.GetType();
				FieldInfo field = type.GetField("gridView",
				  BindingFlags.NonPublic | BindingFlags.Instance);

                object valGrid = field.GetValue(grid);
				Type gridType = valGrid.GetType();
				gridType.InvokeMember("MoveSplitterTo",
				  BindingFlags.NonPublic | BindingFlags.InvokeMethod
					| BindingFlags.Instance,
				  null,
				  valGrid, new object[] { pos });

                foreach (Control control in grid.Controls)
					if (control.GetType().Name == "DocComment")
					{
						FieldInfo fieldInfo = control.GetType().BaseType.GetField("userSized",
						  BindingFlags.Instance |
						  BindingFlags.NonPublic);
						fieldInfo.SetValue(control, true);
                        control.Height = height;
						break;
					}
			}
			catch
			{

			}
		}

		private void SavePos()
		{
			if (WindowState == FormWindowState.Maximized)
			{
				Properties.Settings.Default.Location = RestoreBounds.Location;
				Properties.Settings.Default.Size = RestoreBounds.Size;
				Properties.Settings.Default.Maximized = true;
			}
			else if (WindowState == FormWindowState.Normal)
			{
				Properties.Settings.Default.Location = Location;
				Properties.Settings.Default.Size = Size;
				Properties.Settings.Default.Maximized = false;
			}
			else
			{
				Properties.Settings.Default.Location = RestoreBounds.Location;
				Properties.Settings.Default.Size = RestoreBounds.Size;
				Properties.Settings.Default.Maximized = false;
			}
			Properties.Settings.Default.CurrentPage = tabpage.SelectedIndex;
		}

		private void RestorePos()
		{
			if (Properties.Settings.Default.Maximized)
			{
				WindowState = FormWindowState.Maximized;
				Location = Properties.Settings.Default.Location;
				Size = Properties.Settings.Default.Size;
			}
			else
			{
				Location = Properties.Settings.Default.Location;
				Size = Properties.Settings.Default.Size;
			}
			tabpage.SelectedIndex = Properties.Settings.Default.CurrentPage;
		}
		#endregion

		private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
		{
			ApplyButton.Enabled = true;
		}

        private void propertyGrid2_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            ApplyButton.Enabled = true;
            OgseGraphicsSerializer.Instance.need_to_rewrite = true;
        }

		private void tabpage_Selected(object sender, TabControlEventArgs e)
		{
			if (e.TabPage == tabPage11)
				team.Select();
			else if (e.TabPage == tabPage10)
				about_mod.Select();
			else if (e.TabPage == tabPage1)
				features.Select();
		}

        private void ChangeLocale(E_LANG _l)
        {
            // delete old locale
            File.Delete(Environment.CurrentDirectory + "\\gamedata.db_xlocale");

            // copy new locale
            try
            {
             //   if (_l != E_LANG.RU)
                    File.Copy(Environment.CurrentDirectory + "\\localization\\" + lang + "\\gamedata.db_xlocale", Environment.CurrentDirectory + "\\gamedata.db_xlocale");
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

            // save locale
            var type = typeof(E_LANG);
            var memInfo = type.GetMember(_l.ToString());
            var attributes = memInfo[0].GetCustomAttributes(typeof(DescriptionAttribute), false);
            var description = ((DescriptionAttribute)attributes[0]).Description;
            Utils.INI.WritePrivateProfileString("string_table", "language", description, Data.RootAppPath);
            if ((_l == E_LANG.FR) || (_l == E_LANG.GR))
                Utils.INI.WritePrivateProfileString("string_table", "font_prefix", "_west", Data.RootAppPath);
            else
                Utils.INI.WritePrivateProfileString("string_table", "font_prefix", ";_west", Data.RootAppPath);
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            if (lang != "ru-RU")
                ResetLang("ru-RU");
            ChangeLocale(E_LANG.RU);
            this.lang_pic_ru.Invalidate();
        }

        private void pictureBox2_Click(object sender, EventArgs e)
        {
            if (lang != "en-US")
                ResetLang("en-US");
            ChangeLocale(E_LANG.EN);
            this.lang_pic_en.Invalidate();
        }

        private void pictureBox3_Click(object sender, EventArgs e)
        {
            if (lang != "fr-FR")
                ResetLang("fr-FR");
            ChangeLocale(E_LANG.FR);
            this.lang_pic_fr.Invalidate();
        }
        public void UpdateLangPics()
        {
            foreach (Control cont in this.Controls)
            {
                int pos = cont.Name.IndexOf("lang_pic_");
                if (pos != -1)
                {
                    string _l = cont.Name.Substring(pos + 9, 2);
                    if (_l == "ru")
                    {
                        if (lang == "ru-RU")
                            ((PictureBox)cont).Image = global::Configurator.Properties.Resources.ru;
                        else
                            ((PictureBox)cont).Image = global::Configurator.Properties.Resources.ru_disabled;
                    }
                    else if (_l == "en")
                    {
                        if (lang == "en-US")
                            ((PictureBox)cont).Image = global::Configurator.Properties.Resources.en;
                        else
                            ((PictureBox)cont).Image = global::Configurator.Properties.Resources.en_disabled;
                    }
                    else if (_l == "fr")
                    {
                        if (lang == "fr-FR")
                            ((PictureBox)cont).Image = global::Configurator.Properties.Resources.fr;
                        else
                            ((PictureBox)cont).Image = global::Configurator.Properties.Resources.fr_disabled;
                    }
                }
            }
        }
        public void ResetLang(string _lang)
        {
            try
            {
                // прежде всего, сохраним активную вкладку
                int act_tab = this.tabpage.SelectedIndex;
                lang = _lang;
                SaveSettings();
                // got language, initing
                Thread.CurrentThread.CurrentUICulture = new CultureInfo(lang);
                res = new ResourceManager("Configurator.Resources." + lang, Assembly.GetExecutingAssembly());
                // чистим контролы
                this.Controls.Clear();
                // инитим заново
                InitializeComponent();
                // восстановим активную вкладку
                this.tabpage.SelectedIndex = act_tab;
                // настраиваем контролы
                FormConstruct();
                // сбрасываем все флажки и выставляем хороший на активный
                UpdateLangPics();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }
        public static ResourceManager GetResourceManager()
        {
            if (lang == "ru-RU")
                return Resources.ru_RU.ResourceManager;
            else if (lang == "en-US")
                return Resources.en_US.ResourceManager;
            else if (lang == "fr-FR")
                return Resources.fr_FR.ResourceManager;
            return null;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            find_upd();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (load.IsBusy)
            {
                MessageBox.Show(res.GetString("wait_for_download"));
            }
            else
            {
                saveFileDialog1.FileName = filename.Substring(0, filename.Length);
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    filename = saveFileDialog1.FileName;
                    this.label9.Visible = true;
                    this.progressBar2.Visible = true;
                    load.DownloadFileCompleted += new AsyncCompletedEventHandler(Handler);
                    load.DownloadProgressChanged += new DownloadProgressChangedEventHandler(progress);
                    this.button1.Enabled = false;
                    this.button2.Enabled = false;
                    try
                    {
                        load.DownloadFileAsync(new Uri(update_link), filename);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Process.Start(filename);
            this.Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(Data.SavesDir))
            {
                File.Create(Data.SavesDir + "\\wreset");
                this.button4.Enabled = false;
            }
        }
	}
}
