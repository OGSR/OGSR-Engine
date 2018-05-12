using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Globalization;
using System.Reflection;
using System.Drawing.Design;
using System.Diagnostics;
using System.Resources;
using System.Text.RegularExpressions;

namespace Configurator
{
    class OgseGraphics : GlobalizedObject
    {
        [GlobalizedProperty("ss_dust_name", Description = "ss_dust_desc", Category = "shafts")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool ss_dust { get; set; }

        [GlobalizedProperty("ss_volume_dust_name", Description = "ss_volume_dust_desc", Category = "shafts")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool ss_volume_dust { get; set; }

        [GlobalizedProperty("moonroad_name", Description = "moonroad_desc", Category = "various")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool moonroad { get; set; }

        [GlobalizedProperty("long_flare_name", Description = "long_flare_desc", Category = "various")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool long_flare { get; set; }

        [GlobalizedProperty("indirect_bounce_name", Description = "indirect_bounce_desc", Category = "various")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool indirect_bounce { get; set; }

        [GlobalizedProperty("vignette_name", Description = "vignette_desc", Category = "dof")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool vignette { get; set; }

        private double _vignette_int = 0.0;
        [GlobalizedProperty("vignette_int_name", Description = "vignette_int_desc", Category = "dof")]
        [DefaultValue(0.0)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double vignette_int
        {
            get
            {
                return _vignette_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _vignette_int = value;
            }
        }

        [GlobalizedProperty("chrom_aberr_name", Description = "chrom_aberr_desc", Category = "dof")]
        [TypeConverter(typeof(BooleanTypeConverter))]
        [DefaultValue(true)]
        public bool chrom_aberr { get; set; }

        private double _vol_max_int = 10;
        [GlobalizedProperty("vol_max_int_name", Description = "vol_max_int_desc", Category = "shafts")]
        [DefaultValue(10)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double vol_max_int
        {
            get
            {
                return _vol_max_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 1.0;
                _vol_max_int = value;
            }
        }

        private double _vol_light_max_int = 15;
        [GlobalizedProperty("vol_light_max_int_name", Description = "vol_light_max_int_desc", Category = "shafts")]
        [DefaultValue(15)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double vol_light_max_int
        {
            get
            {
                return _vol_light_max_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _vol_light_max_int = value;
            }
        }

        private double _hbao_int = 70;
        [GlobalizedProperty("hbao_int_name", Description = "hbao_int_desc", Category = "various")]
        [DefaultValue(70)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double hbao_int
        {
            get
            {
                return _hbao_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _hbao_int = value;
            }
        }

        private double _ssdo_int = 75;
        [GlobalizedProperty("ssdo_int_name", Description = "ssdo_int_desc", Category = "various")]
        [DefaultValue(75)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double ssdo_int
        {
            get
            {
                return _ssdo_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _ssdo_int = value;
            }
        }

        private double _ddof_gain = 1.0;
        [GlobalizedProperty("ddof_gain_name", Description = "ddof_gain_desc", Category = "dof")]
        [DefaultValue(1.0)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double ddof_gain
        {
            get
            {
                return _ddof_gain;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 4)
                    value = 4;
                _ddof_gain = value;
            }
        }

        private double _zdof_int = 60;
        [GlobalizedProperty("zdof_int_name", Description = "zdof_int_desc", Category = "dof")]
        [DefaultValue(60)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double zdof_int
        {
            get
            {
                return _zdof_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _zdof_int = value;
            }
        }

        private double _rdof_int = 40;
        [GlobalizedProperty("rdof_int_name", Description = "rdof_int_desc", Category = "dof")]
        [DefaultValue(40)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double rdof_int
        {
            get
            {
                return _rdof_int;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _rdof_int = value;
            }
        }

        private double _puddles_grow = 50;
        [GlobalizedProperty("puddles_grow_name", Description = "puddles_grow_desc", Category = "various")]
        [DefaultValue(50)]
        [TypeConverter(typeof(MyDoubleConverter))]
        public double puddles_grow
        {
            get
            {
                return _puddles_grow;
            }
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 100)
                    value = 100;
                _puddles_grow = value;
            }
        }
    }
    // сделаем-ка его синглтоном
    class OgseGraphicsSerializer
    {
        private List<string> raw_file;
        string path;
        public bool need_to_rewrite = false;

        public static OgseGraphicsSerializer instance;

        public static OgseGraphicsSerializer Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new OgseGraphicsSerializer();
                }
                return instance;
            }
        }
        public void Write()
        {
            if (need_to_rewrite)
            {
                Utils.WriteData(raw_file, path);
                if (Directory.Exists(Data.LogsPath) && Directory.Exists(Data.ShaderCache))
                    Directory.Delete(Data.ShaderCache, true);
            }
        }

        public void Load(string _path, OgseGraphics cfg)
        {
            path = _path;
            raw_file = Utils.ReadData(path);
            try
            {
                #region numeric
                cfg.vol_max_int = GetDoubleFromHeader("SS_VOL_MAX_INTENSITY") * 100;
                cfg.vol_light_max_int = GetDoubleFromHeader("VOL_LIGHT_MAX_INTENSITY") * 1000;
                cfg.ssdo_int = GetDoubleFromHeader("SSDO_BLEND_FACTOR") * 50;
                cfg.hbao_int = GetDoubleFromHeader("HBAO_BLEND_FACTOR") * 100;
                cfg.vignette_int = (1 - GetDoubleFromHeader("DDOF_VIGNOUT")) * 100;
                cfg.ddof_gain = GetDoubleFromHeader("DDOF_GAIN");
                cfg.zdof_int = GetDoubleFromHeader("ZDOF_INTENSITY") * 100;
                cfg.rdof_int = GetDoubleFromHeader("RDOF_INTENSITY") * 100;
                cfg.puddles_grow = GetDoubleFromHeader("PUDDLES_GROW_SPEED") * 100;
                #endregion

                #region params check
                cfg.ss_dust = GetBoolFromHeader("SS_DUST");
                cfg.ss_volume_dust = GetBoolFromHeader("USE_VOLUMETRIC_DUST");
                cfg.moonroad = GetBoolFromHeader("USE_MOON_ROAD");
                cfg.long_flare = GetBoolFromHeader("FL_USE_LONG_FLARE");
                cfg.indirect_bounce = GetBoolFromHeader("SSDO_USE_INDIRECT_BOUNCES");
                cfg.vignette = GetBoolFromHeader("DDOF_VIGNETTING");
                cfg.chrom_aberr = GetBoolFromHeader("DDOF_CHROMATIC_ABERRATION");
                #endregion
            }
            catch (Exception)
            {
                //MessageBox.Show("Error 446\n" + e.Message);
            }
        }
        public void Save(OgseGraphics cfg)
        {
            try
            {
                #region numeric
                SetDoubleToHeader("SS_VOL_MAX_INTENSITY", cfg.vol_max_int * 0.01);
                SetDoubleToHeader("VOL_LIGHT_MAX_INTENSITY", cfg.vol_light_max_int * 0.001);
                SetDoubleToHeader("SSDO_BLEND_FACTOR", cfg.ssdo_int * 0.02);
                SetDoubleToHeader("HBAO_BLEND_FACTOR", cfg.hbao_int * 0.01);
                SetDoubleToHeader("DDOF_VIGNOUT", (100 - cfg.vignette_int) * 0.01);
                SetDoubleToHeader("DDOF_GAIN", cfg.ddof_gain);
                SetDoubleToHeader("ZDOF_INTENSITY", cfg.zdof_int * 0.01);
                SetDoubleToHeader("RDOF_INTENSITY", cfg.rdof_int * 0.01);
                SetDoubleToHeader("PUDDLES_GROW_SPEED", cfg.puddles_grow * 0.01);
                #endregion

                #region params check
                SetBoolToHeader("SS_DUST", cfg.ss_dust);
                SetBoolToHeader("USE_VOLUMETRIC_DUST", cfg.ss_volume_dust);
                SetBoolToHeader("USE_MOON_ROAD", cfg.moonroad);
                SetBoolToHeader("FL_USE_LONG_FLARE", cfg.long_flare);
                SetBoolToHeader("SSDO_USE_INDIRECT_BOUNCES", cfg.indirect_bounce);
                SetBoolToHeader("DDOF_VIGNETTING", cfg.vignette);
                SetBoolToHeader("DDOF_CHROMATIC_ABERRATION", cfg.chrom_aberr);
                #endregion

                Write();
            }
            catch (Exception)
            {
                //MessageBox.Show("Error 446\n" + e.Message);
            }
        }
        private double GetDoubleFromHeader(string key)
        {
            double result = 0.0;
            foreach (string str in raw_file)
            {
                if (str.Contains(key))
                {
                    string pattern = @"#define\s*\t*";
                    pattern += key;
                    pattern += @"\s*\t*float\((\d*\.\d*)\)";
                    Regex rgx = new Regex(pattern);
                    if (rgx.IsMatch(str))
                    {
                        Match value = rgx.Match(str);
                        string res = value.Groups[1].ToString();
                        res = res.Replace('.', ',');
                        Double.TryParse(res, NumberStyles.Any, CultureInfo.CurrentCulture, out result);
                    }
                }
            }
            return result;
        }
        private void SetDoubleToHeader(string key, double value)
        {
            string val = value.ToString("0.000").Replace(',','.');
            for (int i = 0; i < raw_file.Count; ++i)
            {
                if (raw_file[i].Contains(key))
                    raw_file[i] = "#define " + key + " float(" + val + ")";
            }
        }
        private bool GetBoolFromHeader(string key)
        {
            bool result = false;
            foreach (string str in raw_file)
            {
                if (str.Contains(key))
                {
                    string pattern = @"//\s*\t*#define\s*\t*";
                    pattern += key;
                    Regex rgx = new Regex(pattern);
                    if (rgx.IsMatch(str))
                        result = false;
                    else
                        result = true;
                }
            }
            return result;
        }
        private void SetBoolToHeader(string key, bool value)
        {
            for (int i = 0; i < raw_file.Count; ++i)
            {
                if (raw_file[i].Contains(key))
                {
                    if (value)
                        raw_file[i] = "#define " + key;
                    else
                        raw_file[i] = "//#define " + key;
                }
            }
        }
    }
}
