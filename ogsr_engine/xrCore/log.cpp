#include "stdafx.h"

#include "log.h"

#include <sstream> //для std::stringstream
#include <fstream> //для std::ofstream
#include <iomanip> //для std::strftime
#include <array> //для std::array

static LogCallback LogCB = nullptr;
std::vector<std::string> LogFile;
static std::ofstream logstream;
string_path logFName{};

static void AddOne(std::string& split, bool first_line)
{
	static std::recursive_mutex logCS;
	std::scoped_lock<decltype(logCS)> lock(logCS);

	if (IsDebuggerPresent()) { //Вывод в отладчик студии
		OutputDebugString(split.c_str());
		OutputDebugString("\n");
	}

	if (LogCB)
		LogCB(split.c_str()); //Вывод в логкаллбек

	auto insert_time = [&] {
		if (first_line)
		{
			string64 buf, curTime;
			using namespace std::chrono;
			const auto now = system_clock::now();
			const auto time = system_clock::to_time_t(now);
			const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) - duration_cast<seconds>(now.time_since_epoch());
			std::strftime(buf, sizeof(buf), "%d.%m.%y %H:%M:%S", std::localtime(&time));
			sprintf_s(curTime, "\n[%s.%03lld] ", buf, ms.count());
			split = curTime + split;
		}
		else
		{
			split = "\n" + split;
		}
	};

	if (!logstream.is_open())
		insert_time();

	LogFile.push_back(split); //Вывод в консоль

	if (logstream.is_open()) {
		insert_time();

		//Вывод в лог-файл
		logstream << split;
		logstream.flush();
	}
}

void Log(std::stringstream&& ss)
{
	std::string str = ss.str();

	if (str.empty()) return; //Строка пуста - выходим

	bool not_first_line = false;

	constexpr std::array<char, 5> color_codes{ '-', '~', '!', '*', '#' }; //Зелёный, Жёлтый, Красный, Серый, Бирюзовый
	const char& color_s = str.front();
	const bool have_color = std::find(color_codes.begin(), color_codes.end(), color_s) != color_codes.end(); //Ищем в начале строки цветовой код

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


// Это всё не нужно на самом деле, от них бы избавиться...
/////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////


void SetLogCB(LogCallback cb)
{
	LogCB = cb;
}

void CreateLog(BOOL nl)
{
	if (!nl)
	{
		if (!strstr(Core.Params, "-no_unique_logs")) {
			string32 TimeBuf;
			using namespace std::chrono;
			const auto now = system_clock::now();
			const auto time = system_clock::to_time_t(now);
			std::strftime(TimeBuf, sizeof(TimeBuf), "%d-%m-%y_%H-%M-%S", std::localtime(&time));

			xr_strconcat(logFName, Core.ApplicationName, "_", Core.UserName, "_", TimeBuf, ".log");
		}
		else {
			xr_strconcat(logFName, Core.ApplicationName, "_", Core.UserName, ".log");
		}

		try {
			if (FS.path_exist("$logs$")) {
				FS.update_path(logFName, "$logs$", logFName);
			}
			else { //Для компрессора
				string_path temp;
				strcpy_s(temp, logFName);
				xr_strconcat(logFName, "logs\\", temp);
			}

			logstream.imbue(std::locale(""));
			VerifyPath(logFName);
			logstream.open(logFName);
		}
		catch(...) {
			Debug.do_exit("Can't create log file!");
		}

		for (const auto& str : LogFile)
			logstream << str;

		logstream.flush();
	}

	LogFile.reserve(500);
}
