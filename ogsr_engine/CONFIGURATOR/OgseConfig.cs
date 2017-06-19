using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Globalization;
using System.Reflection;
using System.Drawing.Design;
using System.Diagnostics;
using System.Resources;
using System.IO;

namespace Configurator
{

	enum E_PSI_ZONES
	{
        [LocalizedDescription("psyzones_disabled")]
		Off,

        [LocalizedDescription("psyzones_to_psyvybros")]
		On_psi_vibros,

        [LocalizedDescription("psyzones_to_vybros")]
		On_vibros,
	}

	enum E_JUMP
	{
		[Description("стандартный")]
		Standard,

		[Description("увеличенный")]
		High,

		[Description("высокий")]
		VeryHigh,
	}

    enum E_ACTOR_HIT
	{
        [LocalizedDescription("actor_hit_disabled")]
		Off,

        [LocalizedDescription("actor_hit_camera")]
		OnlyCameraShake,

        [LocalizedDescription("actor_hit_all")]
		All,
	}

    enum E_WEATHER
    {
        [LocalizedDescription("weather_default")]
        Default,

        [LocalizedDescription("weather_bright")]
        Bright,

        [LocalizedDescription("weather_rainy")]
        Rainy,
    }

    class OgseConfig : GlobalizedObject
	{
		#region Actor settings

		private int _ves = 60;
   /*     [LocalizedDisplayName("ves_name")]
        [LocalizedDescription("ves_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("ves_name", Description = "ves_desc", Category = "player")]
		[DefaultValue(60)]
		[TypeConverter(typeof(MyIntConverter))]
		public int ves
		{ 
			get
			{
				return _ves;
			}
			set
			{
				if (value < 40)
					value = 40;
				else if (value > 100)
					value = 100;
				_ves = value;
			}
		}

   /*     [LocalizedDisplayName("dinamicheski_hud_name")]
        [LocalizedDescription("dinamicheski_hud_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("dinamicheski_hud_name", Description = "dinamicheski_hud_desc", Category = "player")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(false)]
		public bool dinamicheski_hud { get; set; }

    /*    [LocalizedDisplayName("pnv_disabling_name")]
        [LocalizedDescription("pnv_disabling_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("pnv_disabling_name", Description = "pnv_disabling_desc", Category = "player")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(false)]
        public bool pnv_disabling { get; set; }

   /*     [LocalizedDisplayName("pnv_autostart_name")]
        [LocalizedDescription("pnv_autostart_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("pnv_autostart_name", Description = "pnv_autostart_desc", Category = "player")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(false)]
        public bool pnv_autostart { get; set; }

   /*     [LocalizedDisplayName("zalyap_hud_name")]
        [LocalizedDescription("zalyap_hud_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("zalyap_hud_name", Description = "zalyap_hud_desc", Category = "player")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(false)]
		public bool zalyap_hud { get; set; }

        //[DisplayName("Прыжок")]
        //[Description("")]
        //[Category("Игрок")]
        //[TypeConverter(typeof(EnumTypeConverter))]
        //[DefaultValue(E_JUMP.VeryHigh)]
        //public E_JUMP prigok { get; set; }

  /*      [LocalizedDisplayName("poboch_lek_name")]
        [LocalizedDescription("poboch_lek_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("poboch_lek_name", Description = "poboch_lek_desc", Category = "player")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(false)]
		public bool poboch_lek { get; set; }

    /*    [LocalizedDisplayName("medlen_lek_name")]
        [LocalizedDescription("medlen_lek_desc")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("medlen_lek_name", Description = "medlen_lek_desc", Category = "player")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(false)]
		public bool medlen_lek { get; set; }

     /*   [LocalizedDisplayName("plohoe_samochustv_name")]
        [LocalizedDescription("")]
        [LocalizedCategory("player")]*/
        [GlobalizedProperty("plohoe_samochustv_name", Description = "plohoe_samochustv_desc", Category = "player")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool plohoe_samochustv { get; set; }

        [GlobalizedProperty("ammo_on_belt_name", Description = "ammo_on_belt_desc", Category = "player")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool ammo_on_belt { get; set; }

        /*     [LocalizedDisplayName("popadanie_po_ger_name")]
             [LocalizedDescription("popadanie_po_ger_desc")]
             [LocalizedCategory("player")]*/
        [GlobalizedProperty("popadanie_po_ger_name", Description = "popadanie_po_ger_desc", Category = "player")]
		[TypeConverter(typeof(EnumTypeConverter))]
		[DefaultValue(E_ACTOR_HIT.All)]
		public E_ACTOR_HIT popadanie_po_ger { get; set; }

		private int _fov_volume = 75;
//        [LocalizedDisplayName("fov_volume_name")]
//        [LocalizedDescription("fov_volume_desc")]
//       [LocalizedCategory("player")]
        [GlobalizedProperty("fov_volume_name", Description = "fov_volume_desc", Category = "player")]
		[DefaultValue(75)]
		[TypeConverter(typeof(MyIntConverter))]
		public int fov_volume
		{ 
			get
			{
				return _fov_volume;
			}
			set
			{
				if (value < 5)
					value = 5;
				else if (value > 140)
					value = 140;
				_fov_volume = value;
			}
		}

		#endregion

		#region npc settings

    /*    [LocalizedDisplayName("son_name")]
        [LocalizedDescription("son_desc")]
		[Category("NPC")]*/
        [GlobalizedProperty("son_name", Description = "son_desc", Category = "NPC")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool son { get; set; }

     /*   [LocalizedDisplayName("dop_spawn_name")]
        [LocalizedDescription("dop_spawn_desc")]
		[Category("NPC")]*/
        [GlobalizedProperty("dop_spawn_name", Description = "dop_spawn_desc", Category = "NPC")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool dop_spawn { get; set; }

		private int _period_resp_new = 2;
   /*     [LocalizedDisplayName("period_resp_new_name")]
        [LocalizedDescription("period_resp_new_desc")]
		[Category("NPC")]*/
        [GlobalizedProperty("period_resp_new_name", Description = "period_resp_new_desc", Category = "NPC")]
		[DefaultValue(2)]
		[TypeConverter(typeof(MyIntConverter))]
		public int period_resp_new
		{ 
			get
			{
				return _period_resp_new;
			}
			set
			{
				if (value < 2)
					value = 2;
				else if (value > 24)
					value = 24;
				_period_resp_new = value;
			}
		}

		private int _period_resp_old = 2;
     /*   [LocalizedDisplayName("period_resp_old_name")]
        [LocalizedDescription("period_resp_old_desc")]
		[Category("NPC")]*/
        [GlobalizedProperty("period_resp_old_name", Description = "period_resp_old_desc", Category = "NPC")]
		[DefaultValue(2)]
		[TypeConverter(typeof(MyIntConverter))]
		public int period_resp_old
		{
			get
			{
				return _period_resp_old;
			}
			set
			{
				if (value < 0)
					value = 0;
				else if (value > 24)
					value = 24;
				_period_resp_old = value;
			}
		}

		private int _mnog_resp = 1;
    /*    [LocalizedDisplayName("mnog_resp_name")]
        [LocalizedDescription("")]
		[Category("NPC")]*/
        [GlobalizedProperty("mnog_resp_name", Description = "", Category = "NPC")]
		[DefaultValue(1)]
		[TypeConverter(typeof(MyIntConverter))]
		public int mnog_resp
		{
			get
			{
				return _mnog_resp;
			}
			set
			{
				if (value < 1)
					value = 1;
				else if (value > 5)
					value = 5;
				_mnog_resp = value;
			}
		}

        private int _suit_prob = 15;
        [GlobalizedProperty("suit_prob_name", Description = "suit_prob_desc", Category = "NPC")]
        [DefaultValue(1)]
        [TypeConverter(typeof(MyIntConverter))]
        public int suit_prob
        {
            get
            {
                return _suit_prob;
            }
            set
            {
                if (value < 1)
                    value = 1;
                else if (value > 25)
                    value = 25;
                _suit_prob = value;
            }
        }

        private int _exo_prob = 15;
        [GlobalizedProperty("exo_prob_name", Description = "exo_prob_desc", Category = "NPC")]
        [DefaultValue(1)]
        [TypeConverter(typeof(MyIntConverter))]
        public int exo_prob
        {
            get
            {
                return _exo_prob;
            }
            set
            {
                if (value < 1)
                    value = 1;
                else if (value > 15)
                    value = 15;
                _exo_prob = value;
            }
        }

		#endregion

		#region world settings

     /*   [LocalizedDisplayName("dynamic_news")]
        [LocalizedDescription("dynamic_news_desc")]
        [LocalizedCategory("dynamic_news")]*/
        [GlobalizedProperty("dynamic_news", Description = "dynamic_news_desc", Category = "dynamic_news")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool dinamic_news { get; set; }

		private int _chastota_news = 5;
   /*     [LocalizedDisplayName("news_freq_name")]
        [LocalizedDescription("news_freq_desc")]
        [LocalizedCategory("dynamic_news")]*/
        [GlobalizedProperty("news_freq_name", Description = "news_freq_desc", Category = "dynamic_news")]
		[DefaultValue(5)]
		[TypeConverter(typeof(MyIntConverter))]
		public int chastota_news
		{
			get
			{
				return _chastota_news;
			}
			set
			{
				if (value < 1)
					value = 1;
				else if (value > 5)
					value = 5;
				_chastota_news = value;
			}
		}

    /*    [LocalizedDisplayName("psy_surges_name")]
        [LocalizedDescription("psy_surges_desc")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("psy_surges_name", Description = "psy_surges_desc", Category = "world_settings")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool psi_vibros { get; set; }

   /*     [LocalizedDisplayName("psyzones_name")]
        [LocalizedDescription("psyzones_desc")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("psyzones_name", Description = "psyzones_desc", Category = "world_settings")]
		[TypeConverter(typeof(EnumTypeConverter))]
		[DefaultValue(E_PSI_ZONES.On_vibros)]
		public E_PSI_ZONES psi_zones { get; set; }

   /*     [LocalizedDisplayName("offline_alife_name")]
        [LocalizedDescription("offline_alife_desc")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("offline_alife_name", Description = "offline_alife_desc", Category = "world_settings")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(false)]
		public bool offline_alife { get; set; }

		private int _tajmer_chaes = 15;
   /*     [LocalizedDisplayName("chaes_timer_name")]
        [LocalizedDescription("")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("chaes_timer_name", Description = "", Category = "world_settings")]
		[DefaultValue(15)]
		[TypeConverter(typeof(MyIntConverter))]
		public int tajmer_chaes
		{ 
			get
			{
				return _tajmer_chaes;
			}
			set
			{
				if (value < 5)
					value = 5;
				else if (value > 35)
					value = 35;
				_tajmer_chaes = value;
			}
		}

		public int _period_vibros = 36;
    /*    [LocalizedDisplayName("surge_timer_name")]
        [LocalizedDescription("surge_timer_desc")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("surge_timer_name", Description = "surge_timer_desc", Category = "world_settings")]
		[DefaultValue(36)]
		[TypeConverter(typeof(MyIntConverter))]
		public int period_vibros
		{
			get
			{
				return _period_vibros;
			}
			set
			{
				if (value < 5)
					value = 5;
				else if (value > 72)
					value = 72;
				_period_vibros = value;
			}
		}

		private int _time_factor = 6;
   /*     [LocalizedDisplayName("time_factor_name")]
        [LocalizedDescription("time_factor_desc")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("time_factor_name", Description = "time_factor_desc", Category = "world_settings")]
		[DefaultValue(6)]
		[TypeConverter(typeof(MyIntConverter))]
		public int time_factor
		{
			get
			{
				return _time_factor;
			}
			set
			{
				if (value < 2)
					value = 2;
				else if (value > 30)
					value = 30;
				_time_factor = value;
			}
		}

   /*     [LocalizedDisplayName("time_start_name")]
		[Description("")]
        [LocalizedCategory("world_settings")]*/
        [GlobalizedProperty("time_start_name", Description = "", Category = "world_settings")]
		[DefaultValue(typeof(DateTime), "2012-05-01T06:00:00")]
		[Editor(typeof(NullEditor), typeof(UITypeEditor))]
		[TypeConverter(typeof(TimeConverter))]
		public DateTime start_time { get; set; }

		private int _verojatnost_tajnik = 35;

        [GlobalizedProperty("weather_type_name", Description = "weather_type_desc", Category = "world_settings")]
        [TypeConverter(typeof(EnumTypeConverter))]
        [DefaultValue(E_WEATHER.Default)]
        public E_WEATHER weather_type { get; set; }

        [GlobalizedProperty("ambient_music_name", Description = "ambient_music_desc", Category = "world_settings")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(false)]
        public bool ambient_music
        {
            get
            {
                return File.Exists(Environment.CurrentDirectory + "\\gamedata\\config\\game_maps_single.ltx");
            }
            set
            {
                if (Directory.Exists(Environment.CurrentDirectory + "\\gamedata\\config"))
                {
                    if (value)
                    {
                        File.Copy(Environment.CurrentDirectory + "\\gamedata\\config\\game_maps_single.mus", Environment.CurrentDirectory + "\\gamedata\\config\\game_maps_single.ltx");
                    }
                    else
                    {
                        File.Delete(Environment.CurrentDirectory + "\\gamedata\\config\\game_maps_single.ltx");
                    }
                }
            }
        }

    /*    [LocalizedDisplayName("stashes_name")]
        [LocalizedDescription("stashes_desc")]
        [LocalizedCategory("stashes_settings")]*/
        [GlobalizedProperty("stashes_name", Description = "stashes_desc", Category = "stashes_settings")]
		[DefaultValue(35)]
		[TypeConverter(typeof(MyIntConverter))]
		public int verojatnost_tajnik
		{
			get
			{
				return _verojatnost_tajnik;
			}
			set
			{
				if (value < 10)
					value = 10;
				else if (value > 100)
					value = 100;
				_verojatnost_tajnik = value;
			}
		}

  /*      [LocalizedDisplayName("random_stashes_name")]
        [LocalizedDescription("random_stashes_desc")]
        [LocalizedCategory("stashes_settings")]*/
        [GlobalizedProperty("random_stashes_name", Description = "random_stashes_desc", Category = "stashes_settings")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool sluchajnie_tajniki { get; set; }

   /*     [LocalizedDisplayName("show_stashes_name")]
        [LocalizedDescription("show_stashes_desc")]
        [LocalizedCategory("stashes_settings")]*/
        [GlobalizedProperty("show_stashes_name", Description = "show_stashes_desc", Category = "stashes_settings")]
		[TypeConverter(typeof(BooleanTypeConverter))]
		[DefaultValue(true)]
		public bool otobr_soderj_tajnikov { get; set; }

		#endregion
	}

	class OgseConfigSerializer
	{
        public static OgseConfigSerializer instance;

        public static OgseConfigSerializer Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new OgseConfigSerializer();
                }
                return instance;
            }
        }
        public void Load(OgseConfig cfg)
		{
			try
			{
				#region numeric
				cfg.ves = (int)GetDecimalFromIni("inventory", "max_weight", Data.RootAppPath);
				cfg.mnog_resp = (int)GetDecimalFromIni("options", "resp_index", Data.RootAppPath);
				cfg.verojatnost_tajnik = (int)(100 - GetDecimalFromIni("options", "treasure_dropout", Data.RootAppPath));
				cfg.chastota_news = (int)GetDecimalFromIni("options", "random_news_delta", Data.RootAppPath);
				cfg.period_vibros = (int)GetDecimalFromIni("options", "t_surge", Data.RootAppPath);
				cfg.period_resp_new = (int)GetDecimalFromIni("options", "t_spawn", Data.RootAppPath);
				cfg.period_resp_old = (int)GetDecimalFromIni("options", "t_common", Data.RootAppPath);
				cfg.tajmer_chaes = (int)GetDecimalFromIni("options", "aes_timer", Data.RootAppPath);
				cfg.time_factor = (int)GetDecimalFromIni("options", "s_time_factor", Data.RootAppPath);
				cfg.fov_volume = (int)GetDecimalFromIni("options", "fov_volume", Data.RootAppPath);
                cfg.suit_prob = (int)GetDecimalFromIni("options", "suit_probability", Data.RootAppPath);
                cfg.exo_prob = (int)GetDecimalFromIni("options", "exo_probability", Data.RootAppPath);
				#endregion

				#region params check

				cfg.sluchajnie_tajniki = SetCheckboxValueFromIni("options", "option_random_treasure");
				cfg.otobr_soderj_tajnikov = SetCheckboxValueFromIni("options", "treasure_need_text");
				cfg.son = SetCheckboxValueFromIni("options", "npc_sleep");
				cfg.dinamic_news = SetCheckboxValueFromIni("options", "random_news");
				cfg.dop_spawn = SetCheckboxValueFromIni("options", "additional_spawn");
				cfg.psi_vibros = SetCheckboxValueFromIni("options", "horror_flag");
				cfg.dinamicheski_hud = SetCheckboxValueFromIni("options", "suithud_enable");
				cfg.zalyap_hud = SetCheckboxValueFromIni("options", "blood_enable");
				cfg.plohoe_samochustv = SetCheckboxValueFromIni("options", "bleed_enable");
                cfg.ammo_on_belt = SetCheckboxValueFromIni("options", "ammunition_on_belt");
                cfg.poboch_lek = SetCheckboxValueFromIni("options", "poison_drugs");
				cfg.medlen_lek = SetCheckboxValueFromIni("options", "slow_drugs");
				cfg.offline_alife = SetCheckboxValueFromIni("options", "offline_alife");
                cfg.pnv_disabling = SetCheckboxValueFromIni("wpn_addon_setup", "cheat_nv_block");
                cfg.pnv_autostart = SetCheckboxValueFromIni("wpn_addon_setup", "cheat_nv_scope_autoenable");
				#endregion

				int value = GetIntValueFromIni("options", "psyzones_flag");
				switch(value)
				{
					case 1:
						cfg.psi_zones = E_PSI_ZONES.On_psi_vibros;
						break;
					case 2:
						cfg.psi_zones = E_PSI_ZONES.On_vibros;
						break;
					default:
						cfg.psi_zones = E_PSI_ZONES.Off;
						break;
				}

                value = GetIntValueFromIni("options", "weather_type");
                switch (value)
                {
                    case 1:
                        cfg.weather_type = E_WEATHER.Bright;
                        break;
                    case 2:
                        cfg.weather_type = E_WEATHER.Rainy;
                        break;
                    default:
                        cfg.weather_type = E_WEATHER.Default;
                        break;
                }

				value = GetIntValueFromIni("options", "oh_shit_im_hit");
				switch (value)
				{
					case 1:
						cfg.popadanie_po_ger = E_ACTOR_HIT.OnlyCameraShake;
						break;
					case 2:
						cfg.popadanie_po_ger = E_ACTOR_HIT.All;
						break;
					default:
						cfg.popadanie_po_ger = E_ACTOR_HIT.Off;
						break;
				}

				#region start time


				string StartTime = (new Utils.IniFile(Data.RootAppPath)).IniReadValue("alife", "start_time", "");
				string[] time = StartTime.Split(':');

				try
				{
					cfg.start_time = new DateTime(2012, 5, 1, Convert.ToInt32(time[0]), Convert.ToInt32(time[1]), Convert.ToInt32(time[2].Trim()));
				}
				catch (Exception)
				{
					//MessageBox.Show("Ошибка при попытке сконвертировать параметр:\nstart_time\n" + except.Message);
				}
				#endregion
			}
			catch (Exception)
			{
				//MessageBox.Show("Error 446\n" + e.Message);
			}
		}

		public void Save(OgseConfig cfg)
		{
			string actorLtx = Data.ActorLtxFilePath;

			#region numeric
			Utils.INI.WritePrivateProfileString("inventory", "max_weight", ConvertDecimalToString(cfg.ves), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("actor_condition", "max_walk_weight", ConvertDecimalToString(cfg.ves + 10), actorLtx);
			Utils.INI.WritePrivateProfileString("options", "resp_index", ConvertDecimalToString(cfg.mnog_resp), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "treasure_dropout", ConvertDecimalToString(100 - cfg.verojatnost_tajnik), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "random_news_delta", ConvertDecimalToString(cfg.chastota_news), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "t_surge", ConvertDecimalToString(cfg.period_vibros), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "t_spawn", ConvertDecimalToString(cfg.period_resp_new), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "t_common", ConvertDecimalToString(cfg.period_resp_old), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "aes_timer", ConvertDecimalToString(cfg.tajmer_chaes), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "s_time_factor", ConvertDecimalToString(cfg.time_factor), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("alife", "time_factor", ConvertDecimalToString(cfg.time_factor), Data.RootAppPath);
			Utils.INI.WritePrivateProfileString("options", "fov_volume", ConvertDecimalToString(cfg.fov_volume), Data.RootAppPath);
            Utils.INI.WritePrivateProfileString("options", "suit_probability", ConvertDecimalToString(cfg.suit_prob), Data.RootAppPath);
            Utils.INI.WritePrivateProfileString("options", "exo_probability", ConvertDecimalToString(cfg.exo_prob), Data.RootAppPath);
			#endregion

			#region params check

			WriteCheckboxValueToIni(cfg.sluchajnie_tajniki, "options", "option_random_treasure");
			WriteCheckboxValueToIni(cfg.otobr_soderj_tajnikov, "options", "treasure_need_text");
			WriteCheckboxValueToIni(cfg.son, "options", "npc_sleep");
			WriteCheckboxValueToIni(cfg.dinamic_news, "options", "random_news");
			WriteCheckboxValueToIni(cfg.dop_spawn, "options", "additional_spawn");
			WriteCheckboxValueToIni(cfg.psi_vibros, "options", "horror_flag");
			WriteCheckboxValueToIni(cfg.zalyap_hud, "options", "blood_enable");
			WriteCheckboxValueToIni(cfg.plohoe_samochustv, "options", "bleed_enable");
            WriteCheckboxValueToIni(cfg.ammo_on_belt, "options", "ammunition_on_belt");
            WriteCheckboxValueToIni(cfg.poboch_lek, "options", "poison_drugs");
			WriteCheckboxValueToIni(cfg.medlen_lek, "options", "slow_drugs");
            WriteBoolValueToIni(cfg.pnv_disabling, "wpn_addon_setup", "cheat_nv_block");
            WriteBoolValueToIni(cfg.pnv_autostart, "wpn_addon_setup", "cheat_nv_scope_autoenable");

			if (cfg.dinamicheski_hud == false)
			{
				Utils.INI.WritePrivateProfileString("options", "suithud_enable", "0", Data.RootAppPath);
				Utils.INI.WritePrivateProfileString("options", "blurs_enable", "0", Data.RootAppPath);
			}
			else
			{
				Utils.INI.WritePrivateProfileString("options", "suithud_enable", "1", Data.RootAppPath);
				Utils.INI.WritePrivateProfileString("options", "blurs_enable", "1", Data.RootAppPath);
			}

			string value = "0";
			if (cfg.psi_zones == E_PSI_ZONES.On_psi_vibros)
				value = "1";
			else if (cfg.psi_zones == E_PSI_ZONES.On_vibros)
				value = "2";
			else
				value = "0";
			Utils.INI.WritePrivateProfileString("options", "psyzones_flag", value, Data.RootAppPath);
            
            value = "0";
            if (cfg.weather_type == E_WEATHER.Bright)
                value = "1";
            else if (cfg.weather_type == E_WEATHER.Rainy)
                value = "2";
            else
                value = "0";
            Utils.INI.WritePrivateProfileString("options", "weather_type", value, Data.RootAppPath);
		
			value = "0";
			if (cfg.popadanie_po_ger == E_ACTOR_HIT.All)
				value = "2";
			else if (cfg.popadanie_po_ger == E_ACTOR_HIT.OnlyCameraShake)
				value = "1";
			else
				value = "0";
			Utils.INI.WritePrivateProfileString("options", "oh_shit_im_hit", value, Data.RootAppPath);

			WriteCheckboxValueToIni(cfg.offline_alife, "options", "offline_alife");

			#endregion

			#region start time
			int hour = cfg.start_time.Hour;
			string Hour = hour.ToString();

			int minute = cfg.start_time.Minute;
			string Minute = minute.ToString();

			if (hour < 10)
				Hour = "0" + Hour;
			if (minute < 10)
				Minute = "0" + Minute;


			Utils.INI.WritePrivateProfileString("alife", "start_time", String.Format("{0}:{1}:00", Hour, Minute), Data.RootAppPath);
			#endregion
		}

		private Decimal GetDecimalFromIni(string section, string key, string path)
		{
			string s = "0";
			Utils.INI.GetPrivateProfileString(section, key, "", s, 100, path);

			try
			{
				decimal result;
                if (Decimal.TryParse(s, NumberStyles.Any, CultureInfo.CurrentCulture, out result))
					return result;

				s = s.Replace('.', ',');
                if (Decimal.TryParse(s, NumberStyles.Any, CultureInfo.CurrentCulture, out result))
					return result;

				s = s.Replace(',', '.');
				return Decimal.Parse(s);
			}
			catch (Exception)
			{
				//MessageBox.Show("Cannot convert to decimal" + s);
				return 0;
			}
		}

		private bool SetCheckboxValueFromIni(string section, string key)
		{
			string value = "null";
			Utils.INI.GetPrivateProfileString(section, key, "", value, 100, Data.RootAppPath);

			value = value.Trim().ToLower();

			return value[0] == '1' || value.Substring(0, 3) == "yes" || value.Substring(0, 4) == "true";//при чтении с Utils.INI файла добавляются левые служебные символы в конец строки
		}

		private int GetIntValueFromIni(string section, string key)
		{
			string value = "null";
			Utils.INI.GetPrivateProfileString(section, key, "", value, 100, Data.RootAppPath);
			return Convert.ToInt32(value.Substring(0, 1));
		}

		private string ConvertDecimalToString(decimal input)
		{
			string rez = input.ToString();

			if (rez.Contains(","))
				return rez.Replace(',', '.');
			else
				return rez;
		}

		private void WriteCheckboxValueToIni(bool t1, string section, string key)
		{
			Utils.INI.WritePrivateProfileString(section, key, t1 ? "1" : "0", Data.RootAppPath);
		}
        private void WriteBoolValueToIni(bool t1, string section, string key)
        {
            Utils.INI.WritePrivateProfileString(section, key, t1 ? "true" : "false", Data.RootAppPath);
        }
	}

	#region type converters

	class BooleanTypeConverter : BooleanConverter
	{
		public override object ConvertTo(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value,
		  Type destType)
		{
            ResourceManager res = Form1.GetResourceManager();
            if (res != null)
                return (bool)value ? res.GetString("on") : res.GetString("off");
			return (bool)value ? "Вкл" : "Выкл";
		}

		public override object ConvertFrom(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value)
		{
            ResourceManager res = Form1.GetResourceManager();
            if (res != null)
                return (value as string).ToLower() == res.GetString("on").ToLower();
			return (value as string).ToLower() == "вкл";
		}
	}

	class MyIntConverter : Int32Converter
	{
		public override object ConvertTo(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value,
		  Type destType)
		{
			return value.ToString();
		}

		public override object ConvertFrom(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value)
		{
			int n = 0;
            if (!Int32.TryParse(value as string, NumberStyles.Any, CultureInfo.CurrentCulture, out n))
			{
				var gridItem = context as System.Windows.Forms.GridItem;
				if (gridItem != null)
					return gridItem.Value;
				else
					return null;
			}
			return n;
		}
	}

    class MyDoubleConverter : DoubleConverter
    {
        public override object ConvertTo(ITypeDescriptorContext context,
          CultureInfo culture,
          object value,
          Type destType)
        {
            return value.ToString();
        }

        public override object ConvertFrom(ITypeDescriptorContext context,
          CultureInfo culture,
          object value)
        {
            double n = 0.0;
            if (!Double.TryParse(value as string, NumberStyles.Any, CultureInfo.CurrentCulture, out n))
            {
                var gridItem = context as System.Windows.Forms.GridItem;
                if (gridItem != null)
                    return gridItem.Value;
                else
                    return null;
            }
            return n;
        }
    }

	/// <summary>
	/// TypeConverter для Enum, преобразовывающий Enum к строке с
	/// учетом атрибута Description
	/// </summary>
	class EnumTypeConverter : EnumConverter
	{
		private Type _enumType;
		/// <summary>Инициализирует экземпляр</summary>
		/// <param name="type">тип Enum</param>
		public EnumTypeConverter(Type type)
			: base(type)
		{
			_enumType = type;
		}

		public override bool CanConvertTo(ITypeDescriptorContext context,
		  Type destType)
		{
			return destType == typeof(string);
		}

		public override object ConvertTo(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value, Type destType)
		{
			FieldInfo fi = _enumType.GetField(Enum.GetName(_enumType, value));
			DescriptionAttribute dna =
			  (DescriptionAttribute)Attribute.GetCustomAttribute(
				fi, typeof(DescriptionAttribute));

			if (dna != null)
				return dna.Description;
			else
				return value.ToString();
		}

		public override bool CanConvertFrom(ITypeDescriptorContext context,
		  Type srcType)
		{
			return srcType == typeof(string);
		}

		public override object ConvertFrom(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value)
		{
			foreach (FieldInfo fi in _enumType.GetFields())
			{
				DescriptionAttribute dna =
				  (DescriptionAttribute)Attribute.GetCustomAttribute(
					fi, typeof(DescriptionAttribute));

				if ((dna != null) && ((string)value == dna.Description))
					return Enum.Parse(_enumType, fi.Name);
			}

			return Enum.Parse(_enumType, (string)value);
		}

	}

	class TimeConverter : DateTimeConverter
	{
		public override object ConvertTo(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value, Type destType)
		{
			return ((DateTime)value).ToString("HH:mm");
		}

		public override object ConvertFrom(ITypeDescriptorContext context,
		  CultureInfo culture,
		  object value)
		{
			DateTime dt = DateTime.Now;
            if (!DateTime.TryParse(value as string, out dt))
			{
				var gridItem = context as System.Windows.Forms.GridItem;
				if (gridItem != null)
					return gridItem.Value;
				else
					return null;
			}
			return new DateTime(2012, 05, 01, dt.Hour, dt.Minute, dt.Second);
		}
	}

	class NullEditor : UITypeEditor
	{
		public new UITypeEditorEditStyle GetEditStyle()
		{
			return UITypeEditorEditStyle.None;
		}
	}

	#endregion

}