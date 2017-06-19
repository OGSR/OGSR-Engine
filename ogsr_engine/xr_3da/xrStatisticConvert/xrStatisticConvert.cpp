// xrStatisticConvert.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Statistic.h"

void test(LPCSTR fName);

int _tmain(int argc, _TCHAR* argv[])
{
//.	test(argv[1]);
//.	return 0;

	WeaponUsageStatistic WUS;

	for (int i=1; i<argc; i++)
	{
		printf("%s ", argv[i]);
		WUS.LoadFile(argv[i]);
	}
	//-----------------------------------------
	SYSTEMTIME Time;
	GetLocalTime(&Time);

	string1024 FileName = "";
	sprintf(FileName, "%s(%s)_(%s)_%02d.%02d.%02d_%02d.%02d.%02d.txt", FilePath, LevelName, GameType, Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
	//-----------------------------------------
	WUS.SaveFile(FileName);
	return 0;
}
/*
void test(LPCSTR fName)
{
	DbgOut("Processing \'%s\'\n", fName);

	char fNameOut[256];
	sprintf(fNameOut,"%s_out",fName);
	FILE* pFileOut	= fopen(fNameOut,"wt");
	FILE* pFile		= fopen(fName,"rt");

	if (!pFile) return;
	char	buff[4096];
	char	buff_res[4096];
	
	while(fgets(buff, 4096, pFile))
	{
		if( buff==strstr(buff,"* Loaded: gamedata\\textures") )
		{
			char*	_begin	= strstr(buff,"gamedata\\textures\\")+strlen("gamedata\\textures\\");
			char*	_end	= strstr(buff,".dds");
			int		_len	= _end-_begin;
			
			strcpy		(buff_res,"load   ");
			strncat		(buff_res, buff+(_begin-buff), _len);
			strcat		(buff_res,"\n");
			fputs		(buff_res,pFileOut);
		}else
		if(	buff==strstr(buff,"* Unloading texture") )
		{
			char*	_begin	= strstr(buff,"[")+strlen("[");
			char*	_end	= strstr(buff,"]");
			int		_len	= _end-_begin;
			
			strcpy		(buff_res,"unload ");
			strncat		(buff_res, buff+(_begin-buff), _len);
			strcat		(buff_res,"\n");
			fputs		(buff_res,pFileOut);
		}

	}
	fclose(pFile);
	fclose(pFileOut);
}
*/