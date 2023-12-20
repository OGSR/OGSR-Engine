#pragma once

// refs
struct xr_token;

XRCORE_API int _GetItemCount(LPCSTR, char separator = ',');

XRCORE_API LPSTR _GetItem(LPCSTR, int, LPSTR, char separator = ',', LPCSTR = "", bool trim = true);
XRCORE_API LPCSTR _GetItem(LPCSTR, int, xr_string& p, char separator = ',', LPCSTR = "", bool trim = true);

XRCORE_API LPSTR _GetItems(LPCSTR, int, int, LPSTR, char separator = ',');

XRCORE_API u32 _ParseItem(LPCSTR src, xr_token* token_list);
XRCORE_API void _SequenceToList(SStringVec& lst, LPCSTR in, char separator = ',');

XRCORE_API LPSTR _Trim(LPSTR str);
XRCORE_API LPSTR _TrimLeft(LPSTR str);
XRCORE_API LPSTR _TrimRight(LPSTR str);

XRCORE_API xr_string& _Trim(xr_string& src);
XRCORE_API xr_string& _TrimLeft(xr_string& src);
XRCORE_API xr_string& _TrimRight(xr_string& src);
