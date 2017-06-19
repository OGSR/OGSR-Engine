////////////////////////////////////////////////////////////////////////////
//	Module 		: xrSE_Factory.cpp
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrSE_Factory.h"
#include "ai_space.h"
#include "script_engine.h"
#include "object_factory.h"
#include "xrEProps.h"
#include "xrSE_Factory_import_export.h"
#include "script_properties_list_helper.h"

#include "character_info.h"
#include "specific_character.h"
//#include "character_community.h"
//#include "monster_community.h"
//#include "character_rank.h"
//#include "character_reputation.h"

extern CSE_Abstract *F_entity_Create	(LPCSTR section);

extern CScriptPropertiesListHelper	*g_property_list_helper;
extern HMODULE						prop_helper_module;

extern "C" {
	FACTORY_API	ISE_Abstract* __stdcall create_entity	(LPCSTR section)
	{
		return					(F_entity_Create(section));
	}

	FACTORY_API	void		__stdcall destroy_entity	(ISE_Abstract *&abstract)
	{
		CSE_Abstract			*object = smart_cast<CSE_Abstract*>(abstract);
		F_entity_Destroy		(object);
		abstract				= 0;
	}
};

typedef void DUMMY_STUFF (const void*,const u32&,void*);
XRCORE_API DUMMY_STUFF	*g_temporary_stuff;

#define TRIVIAL_ENCRYPTOR_DECODER
#include "../xr_3da/trivial_encryptor.h"

BOOL APIENTRY DllMain		(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
			g_temporary_stuff			= &trivial_encryptor::decode;

 			Core._initialize			("xrSE_Factory",NULL,TRUE,"fsfactory.ltx");
			string_path					SYSTEM_LTX;
			FS.update_path				(SYSTEM_LTX,"$game_config$","system.ltx");
			pSettings					= xr_new<CInifile>(SYSTEM_LTX);

			CCharacterInfo::InitInternal					();
			CSpecificCharacter::InitInternal				();
//			CHARACTER_COMMUNITY::InitInternal				();
//			CHARACTER_RANK::InitInternal					();
//			CHARACTER_REPUTATION::InitInternal				();
//			MONSTER_COMMUNITY::InitInternal					();

			break;
		}
		case DLL_PROCESS_DETACH: {
			CCharacterInfo::DeleteSharedData				();
			CCharacterInfo::DeleteIdToIndexData				();
			CSpecificCharacter::DeleteSharedData			();
			CSpecificCharacter::DeleteIdToIndexData			();
//			CHARACTER_COMMUNITY::DeleteIdToIndexData		();
//			CHARACTER_RANK::DeleteIdToIndexData				();
//			CHARACTER_REPUTATION::DeleteIdToIndexData		();
//			MONSTER_COMMUNITY::DeleteIdToIndexData			();


			xr_delete					(g_object_factory);
			xr_delete					(pSettings);
			xr_delete					(g_property_list_helper);
			xr_delete					(g_ai_space);
			xr_delete					(g_object_factory);
			if (prop_helper_module)
				FreeLibrary				(prop_helper_module);
			Core._destroy				();
			break;
		}
	}
    return				(TRUE);
}
