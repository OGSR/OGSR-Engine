#include "stdafx.h"
#pragma hdrstop

#include "ESceneSectorTools.h"

// chunks
static const u16 SECTOR_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
enum{
    CHUNK_VERSION			= 0x1001ul,
    CHUNK_FLAGS				= 0x1002ul,
};
//----------------------------------------------------

bool ESceneSectorTools::Load(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=SECTOR_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::Load(F)) return false;

    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.assign	(F.r_u32());

    return true;
}
//----------------------------------------------------

void ESceneSectorTools::Save(IWriter& F)
{
	inherited::Save	(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&SECTOR_TOOLS_VERSION,sizeof(SECTOR_TOOLS_VERSION));

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();
}
//----------------------------------------------------
 
bool ESceneSectorTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=SECTOR_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}
//----------------------------------------------------

void ESceneSectorTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&SECTOR_TOOLS_VERSION,sizeof(SECTOR_TOOLS_VERSION));
    
	inherited::SaveSelection(F);
}
//----------------------------------------------------

 