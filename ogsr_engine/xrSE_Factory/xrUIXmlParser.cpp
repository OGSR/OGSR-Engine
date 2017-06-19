#include "stdafx.h"

#include "xrUIXmlParser.h"

#ifdef XRGAME_EXPORTS
	#include "../ui_base.h"
#else

#endif

shared_str CUIXml::correct_file_name	(LPCSTR path, LPCSTR fn)
{
#ifdef XRGAME_EXPORTS
	if(0==xr_strcmp(path,"ui") || 0==xr_strcmp(path,"UI"))
	{
		return UI()->get_xml_name(fn);
	}else
		return fn;
#else
	return fn;
#endif
}
