#pragma once

void	ReadRegistry_StrValue	(LPCSTR rKeyName, char* value );
void	WriteRegistry_StrValue	(LPCSTR rKeyName, const char* value );

void	ReadRegistry_DWValue	(LPCSTR rKeyName, DWORD& value );
void	WriteRegistry_DWValue	(LPCSTR rKeyName, const DWORD& value );