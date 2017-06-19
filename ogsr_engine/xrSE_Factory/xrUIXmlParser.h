#pragma once

#ifdef XRGAME_EXPORTS
	#include "../../../xrXMLParser/xrXMLParser.h"
#else
	#include "../xrXMLParser/xrXMLParser.h"
#endif

class CUIXml :public CXml
{

protected:
	virtual shared_str correct_file_name	(LPCSTR path, LPCSTR fn);

};