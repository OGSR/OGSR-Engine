#pragma once

#define VPUSH(a) a.x, a.y, a.z

void XRCORE_API __cdecl Msg(const char* format, ...);
void XRCORE_API Log(const std::string& msg);
void XRCORE_API Log(const char* msg);
void XRCORE_API Log(const char* msg, const Fvector& dop);
void XRCORE_API Log(const char* msg, const Fmatrix& dop);

using LogCallback = std::function<void(const char*)>;
void XRCORE_API SetLogCB(LogCallback cb);
void CreateLog(BOOL no_log = FALSE);

extern XRCORE_API xr_vector<std::string> LogFile;
extern XRCORE_API string_path logFName;
