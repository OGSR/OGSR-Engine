#include "stdafx.h"

#include "log.h"

#include <sstream> //для std::stringstream
#include <fstream> //для std::ofstream
#include <iomanip> //для std::strftime

static BOOL 				no_log	 = TRUE;
static std::recursive_mutex logCS;
static LogCallback			LogCB	 = nullptr;
xr_vector<std::string>*		LogFile  = nullptr;
std::ofstream logstream;

void AddOne(std::string &split, bool first_line)
{
	if (!LogFile)
		return;

	std::scoped_lock<decltype(logCS)> lock(logCS);

#ifdef DEBUG
	OutputDebugString(split.c_str()); //Вывод в отладчик студии?
	OutputDebugString("\n");
#endif

	if (LogCB)
		LogCB(split.c_str()); //Вывод в логкаллбек

	LogFile->push_back(split); //Вывод в консоль

	if (!logstream.is_open()) return;

	if (first_line)
	{
		string64 buf, curTime;
		using namespace std::chrono;
		const auto now = system_clock::now();
		const auto time = system_clock::to_time_t(now);
		const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) - duration_cast<seconds>(now.time_since_epoch());
		std::strftime(buf, sizeof(buf), "%d.%m.%y %H:%M:%S", std::localtime(&time));
		sprintf_s(curTime, sizeof(curTime), "\n[%s.%03lld] ", buf, ms.count());
		split = curTime + split;
	}
	else
	{
		split = "\n" + split;
	}

	//Вывод в лог-файл
	logstream << split;
	logstream.flush();
}

void Log(std::stringstream&& ss)
{
	std::string str = ss.str();

	if (str.empty()) return; //Строка пуста - выходим

	bool not_first_line = false;
	bool have_color = false;
	auto color_s = str.front();
	if ( //Ищем в начале строки цветовой код
		color_s == '-' //Зелёный
		|| color_s == '~' //Жёлтый
		|| color_s == '!' //Красный
		|| color_s == '*' //Серый
		|| color_s == '#' //Бирюзовый
	) have_color = true;

	for (std::string item; std::getline(ss, item);) //Разбиваем текст по "\n"
	{
		if (not_first_line && have_color)
		{
			item = ' ' + item;
			item = color_s + item; //Если надо, перед каждой строкой вставляем спец-символ цвета, чтобы в консоли цветными были все строки текста, а не только первая.
		}
		AddOne(item, !not_first_line);
		not_first_line = true;
	}
}

void Log(const char* s)
{
	std::stringstream ss(s);
	Log(std::move(ss));
}

void __cdecl Msg(const char *format, ...)
{
	string4096 strBuf;
	va_list args;
	va_start(args, format);
	int buf_len = std::vsnprintf(strBuf, sizeof(strBuf), format, args);
	va_end(args);
	if (buf_len > 0)
		Log(strBuf);
}

void Log(const char *msg, const char *dop) { //Надо убрать
	char buf[1024];
	if (dop)
		std::snprintf(buf,sizeof(buf),"%s %s",msg,dop);
	else
		std::snprintf(buf,sizeof(buf),"%s",msg);
	Log(buf);
}

void Log(const char *msg, u32 dop) { //Надо убрать
	char buf[1024];
	std::snprintf(buf,sizeof(buf),"%s %d",msg,dop);
	Log(buf);
}

void Log(const char *msg, int dop) {
	char buf[1024];
	std::snprintf(buf, sizeof(buf),"%s %d",msg,dop);
	Log(buf);
}

void Log(const char *msg, float dop) {
	char buf[1024];
	std::snprintf(buf, sizeof(buf),"%s %f",msg,dop);
	Log(buf);
}

void Log(const char *msg, const Fvector &dop) {
	char buf[1024];
	std::snprintf(buf,sizeof(buf),"%s (%f,%f,%f)",msg,dop.x,dop.y,dop.z);
	Log(buf);
}

void Log(const char *msg, const Fmatrix &dop)	{
	char buf[1024];
	std::snprintf(buf,sizeof(buf),"%s:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",msg,dop.i.x,dop.i.y,dop.i.z,dop._14_
																				,dop.j.x,dop.j.y,dop.j.z,dop._24_
																				,dop.k.x,dop.k.y,dop.k.z,dop._34_
																				,dop.c.x,dop.c.y,dop.c.z,dop._44_);
	Log(buf);
}

void SetLogCB(LogCallback cb)
{
	LogCB = cb;
}

void InitLog()
{
	R_ASSERT(!LogFile);
	LogFile = xr_new<xr_vector<std::string>>();
}

void CreateLog(BOOL nl)
{
	no_log = nl;

	if (!no_log)
	{
		string_path logFName;
		if (strstr(Core.Params, "-unique_logs")) {
			string32 TimeBuf;
			using namespace std::chrono;
			const auto now = system_clock::now();
			const auto time = system_clock::to_time_t(now);
			std::strftime(TimeBuf, sizeof(TimeBuf), "%d-%m-%y_%H-%M-%S", std::localtime(&time));

			strconcat(sizeof(logFName), logFName, Core.ApplicationName, "_", Core.UserName, "_", TimeBuf, ".log");
		}
		else {
			strconcat(sizeof(logFName), logFName, Core.ApplicationName, "_", Core.UserName, ".log");
		}

		__try {
			if (FS.path_exist("$logs$")) {
				FS.update_path(logFName, "$logs$", logFName);
			}
			else { //Для компрессора
				string_path temp;
				strcpy_s(temp, sizeof(temp), logFName);
				strconcat(sizeof(logFName), logFName, "logs\\", temp);
			}

			logstream.imbue(std::locale(""));
			VerifyPath(logFName);
			logstream.open(logFName);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			Debug.do_exit("Can't create log file!");
		}

		for (const auto& str : *LogFile)
			logstream << "\n" << str;

		logstream.flush();
	}

	LogFile->reserve(1000);
}

void CloseLog()
{
	if (logstream.is_open())
		logstream.close();

 	LogFile->clear();
	xr_delete(LogFile);
}
