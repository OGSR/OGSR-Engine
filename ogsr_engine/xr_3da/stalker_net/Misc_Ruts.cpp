#include "stdafx.h"
#include "Misc_Ruts.h"

bool GetToken(char** sx, char* e, char* token)
{
	char* s = *sx;

	if (!s || !token) return false;	

skipspace:

	if (s == e) return false;
	while (*s <= 32 || s == e)
	{
		if (s == e) 
		{
			*sx = s;
			return false;
		}
		s++;
	};

	// comments	; # //
	if (*s == ';' || *s == '#' || (s[0] == '/' && s[1] == '/'))
	{
		while (*s++ != '\n')
		{
			if (s == e) 
			{
				*sx = s;
				return false;
			}
		};
		goto skipspace;
	}
	// comments /* */
	if (s[0] == '/' && s[1] == '*')
	{
		s += 2;
		while (s[0] != '*' || s[1] != '/')
		{
			if (s == e) 
			{
				*sx = s;
				return false;
			}
			s++;
		};
		s += 2;
		goto skipspace;
	};

	char *t = token;
	char control;
	if (*s == '"' || *s == '\'')
	{
		control = *s;
		s++;
		while (*s != control) 
		{
			*t++ = *s++;
			if (s == e) 
			{
				*sx = s;
				return false;
			}
		}
		*t = 0;
		s++;
		*sx = s;
	}
	else
	{
		while (*s > 32 && *s != ';')
		{
			*t++ = *s++;
			if (s == e) 
			{
				*sx = s;
				return false;
			}
		};
		*t = 0;
		*sx = s;
	};
	return true;
};