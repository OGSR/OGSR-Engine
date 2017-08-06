#include "StdAfx.h"
#include "RegistryFuncs.h"

#define REGISTRY_BASE	HKEY_LOCAL_MACHINE

//#define DEMO_BUILD

#ifdef DEMO_BUILD
	#define REGISTRY_PATH	"Software\\GSC Game World\\STALKER-SoC\\"
#else
	#define REGISTRY_PATH	"Software\\GSC Game World\\STALKER-SHOC\\"
#endif


bool	ReadRegistryValue(LPCSTR rKeyName, DWORD rKeyType, void* value )
{	
	HKEY hKey = 0;	
	long res = RegOpenKeyEx(REGISTRY_BASE, 
		REGISTRY_PATH, 0, KEY_READ, &hKey);

	if (res != ERROR_SUCCESS)
	{
		Msg ("! Unable to find %s in registry", REGISTRY_PATH);
		return false;
	}

	if (!hKey) 
	{
		Msg ("! Unable to find %s entry in registry", REGISTRY_PATH); 
		return false;
	}

	string64	rBuf;
	DWORD KeyValueSize = 0;
	switch (rKeyType)
	{
	case REG_DWORD:
		{
			KeyValueSize = 4;
		}break;
	case REG_SZ:
		{
			KeyValueSize = 64;
		}break;
	default:
		{
			Msg ("! Unknown registry data type.");
			return false;
		}break;
	};	
		
	res = RegQueryValueEx(hKey, rKeyName, NULL, &rKeyType, (LPBYTE)rBuf, &KeyValueSize);
	if (hKey != 0) RegCloseKey(hKey);

	if (res != ERROR_SUCCESS)
	{
		Msg ("! Unable to find %s entry in registry", rKeyName); 
		return false;
	}
	
	memcpy(value, rBuf, KeyValueSize);
	return true;
};

bool	WriteRegistryValue	(LPCSTR rKeyName, DWORD rKeyType, const void* value)
{
	HKEY hKey;

	long res = RegOpenKeyEx(REGISTRY_BASE, 
		REGISTRY_PATH, 0, KEY_WRITE, &hKey);

	if (res != ERROR_SUCCESS)
	{
		Msg ("! Unable to find %s in registry", REGISTRY_PATH);
		return false;
	}

	if (!hKey) 
	{
		Msg ("! Unable to find %s entry in registry", REGISTRY_PATH); 
		return false;
	}

	DWORD KeyValueSize = 0;
	switch (rKeyType)
	{
	case REG_DWORD:
		{
			KeyValueSize = 4;
		}break;
	case REG_SZ:
		{
			KeyValueSize = 64;
		}break;
	default:
		{
			Msg ("! Unknown registry data type.");
			return false;
		}break;
	};	
	
	res = RegSetValueEx(hKey, rKeyName, NULL, rKeyType, (LPBYTE)value, KeyValueSize);

	if (hKey) RegCloseKey(hKey);
	return true;
};

void	ReadRegistry_StrValue	(LPCSTR rKeyName, char* value )
{
	ReadRegistryValue(rKeyName, REG_SZ, value);
}

void	WriteRegistry_StrValue	(LPCSTR rKeyName, const char* value )
{
	WriteRegistryValue(rKeyName, REG_SZ, value);
}

void	ReadRegistry_DWValue	(LPCSTR rKeyName, DWORD& value )
{
	ReadRegistryValue(rKeyName, REG_DWORD, &value);
}
void	WriteRegistry_DWValue	(LPCSTR rKeyName, const DWORD& value )
{
	WriteRegistryValue(rKeyName, REG_DWORD, &value);
}