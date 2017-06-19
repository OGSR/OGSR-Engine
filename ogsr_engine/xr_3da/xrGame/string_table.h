//////////////////////////////////////////////////////////////////////////
// string_table.h:		таблица строк используемых в игре
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "string_table_defs.h"

DEFINE_MAP		(STRING_ID, STRING_VALUE, STRING_TABLE_MAP, STRING_TABLE_MAP_IT);

struct STRING_TABLE_DATA
{
	shared_str				m_sLanguage;
	
	STRING_TABLE_MAP		m_StringTable;
	
	STRING_TABLE_MAP		m_string_key_binding;
};


class CStringTable 
{
public:
								CStringTable			();

	static void					Destroy					();
	
//.	STRING_INDEX				IndexById				(const STRING_ID& str_id)		const;
	STRING_VALUE				translate				(const STRING_ID& str_id)		const;
//.	STRING_VALUE				translate				(const STRING_INDEX str_index)	const;

	static	BOOL				m_bWriteErrorsToLog;
	static	void				ReparseKeyBindings		();
	static  shared_str			GetLanguage				();
private:
			void				Init					();
			void				Load					(LPCSTR xml_file);
	static STRING_VALUE			ParseLine				(LPCSTR str, LPCSTR key, bool bFirst);
//.	bool						GetKeyboardItem			(LPCSTR src, char* dst);
	static STRING_TABLE_DATA*	pData;
};