// xrGameSpy.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrGameSpy.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

void	FillSecretKey(char* SecretKey)
{
	SecretKey[0] = 't';
	SecretKey[1] = '9';
	SecretKey[2] = 'F';
	SecretKey[3] = 'j';
	SecretKey[4] = '3';
	SecretKey[5] = 'M';
	SecretKey[6] = 'x';
	SecretKey[7] = '\0';
}

const char* GetGameVersion	(const char*KeyValue)
{
	HKEY KeyCDKey = 0;
	long res = RegOpenKeyEx(REGISTRY_BASE, 
		REGISTRY_PATH, 0, KEY_READ, &KeyCDKey);

//	char	KeyValue[1024] = "";
	DWORD KeyValueSize = 128;
	DWORD KeyValueType = REG_SZ;
	if (res == ERROR_SUCCESS && KeyCDKey != 0)
	{
		res = RegQueryValueEx(KeyCDKey, REGISTRY_VALUE_VERSION, NULL, &KeyValueType, (LPBYTE)KeyValue, &KeyValueSize);
	};
	if (KeyCDKey != 0) RegCloseKey(KeyCDKey);

	if (res == ERROR_PATH_NOT_FOUND ||
		res == ERROR_FILE_NOT_FOUND ||
		KeyValueSize == 0)
	{
		return GAME_VERSION;
	};
//	return KeyValue;	
	return GAME_VERSION;
}

XRGAMESPY_API const char* xrGS_GetGameVersion	(const char*KeyValue)
{
	return GetGameVersion(KeyValue);
}

XRGAMESPY_API void xrGS_GetGameID	(int* GameID, int verID)
{
	*GameID = int(GAMESPY_GAMEID);

#ifdef DEMO_BUILD
	switch (verID)
	{
	case 1: *GameID = int(1067); break;
	case 2: *GameID = int(1576); break;
	case 3: *GameID = int(1620); break;
	default: *GameID = int(GAMESPY_GAMEID); break;
	}	
#endif
}