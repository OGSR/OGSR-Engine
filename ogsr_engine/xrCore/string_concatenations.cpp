#include "stdafx.h"
#include "string_concatenations.h"

int XRCORE_API _strconcatSingle(char*& destPtr, char* pDestEnd, const char* Str)
{
	char* TargetStrCursor = const_cast<char*> (Str);
	for (; *TargetStrCursor && destPtr < pDestEnd; destPtr++, TargetStrCursor++)
	{
		*destPtr = *TargetStrCursor;
	}

	R_ASSERT3(!(*TargetStrCursor), "Failed to concatenate string", Str);

	return 0;
}
