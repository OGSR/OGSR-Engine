#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>

void DbgOut( const char *format, ... ) 
{
	va_list argptr;
	char text[4096];

	va_start (argptr,format);
	vsprintf (text, format, argptr);
	va_end (argptr);

	printf(text);
	OutputDebugString( text );
}

void ConvOut(FILE* pFile, const char *format, ... ) 
{
	va_list argptr;
	char text[4096];

	va_start (argptr,format);
	vsprintf (text, format, argptr);
	va_end (argptr);
	//-------------------------------
	char* pChar = text;
	while (*pChar != 0)
	{
		if (*pChar == '.') *pChar = ',';
		pChar++;
	}
	//-------------------------------
	fprintf(pFile, text);
}



void	ReadName(char* Name, FILE* pFile)
{
	*Name = 0;
	if (!pFile) return;
	byte b = 0;
	while(1)
	{
		fread(&b, 1, 1, pFile);
		*Name++ = b;
		if (!b) break;
	}
}