#include "stdafx.h"
#pragma hdrstop

#include "ESceneShapeTools.h"

// chunks
static const u16 SHAPE_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
enum{
    CHUNK_VERSION			= 0x1001ul,
};
//----------------------------------------------------

bool ESceneShapeTools::Load(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=SHAPE_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::Load(F)) return false;

    return true;
}
//----------------------------------------------------

void ESceneShapeTools::Save(IWriter& F)
{
	inherited::Save	(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&SHAPE_TOOLS_VERSION,sizeof(SHAPE_TOOLS_VERSION));
}
//----------------------------------------------------
 
bool ESceneShapeTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=SHAPE_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}
//----------------------------------------------------

void ESceneShapeTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&SHAPE_TOOLS_VERSION,sizeof(SHAPE_TOOLS_VERSION));
    
	inherited::SaveSelection(F);
}
//----------------------------------------------------

 