#pragma once

int XRCORE_API _strconcatSingle(char*& destPtr, char* pDestEnd, const char* Str);


// Giperion XRay Oxygen - ultimate version of strconcat
template<typename StringReceiverType, typename... ArgList>
char* xr_strconcat(StringReceiverType& receiver, ArgList... args)
{
	static_assert(std::is_array< StringReceiverType>::value); // must be array...
	static_assert(std::is_same<std::remove_extent< StringReceiverType>::type, char>::value); // ... of chars

	char* pStrCursor = &receiver[0];
	char* pStrEnd = &receiver[0] + sizeof(StringReceiverType);
	int dummy[] = { _strconcatSingle(pStrCursor, pStrEnd, args)... };

	*pStrCursor = '\0';
	return &receiver[0];
}