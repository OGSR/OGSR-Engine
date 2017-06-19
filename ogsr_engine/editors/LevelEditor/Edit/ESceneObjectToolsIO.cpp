#include "stdafx.h"
#pragma hdrstop

#include "ESceneObjectTools.h"
#include "SceneObject.h"

// chunks
static const u16 OBJECT_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
enum{
    CHUNK_VERSION			= 0x1001ul,
    CHUNK_APPEND_RANDOM		= 0x1002ul,
    CHUNK_FLAGS				= 0x1003ul,
};

bool ESceneObjectTools::Load(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version)){
        if( version!=OBJECT_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }
    }
	if (!inherited::Load(F)) return false;
    
    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.assign(F.r_u32());

    if (F.find_chunk(CHUNK_APPEND_RANDOM)){
        F.r_fvector3(m_AppendRandomMinScale);
        F.r_fvector3(m_AppendRandomMaxScale);
        F.r_fvector3(m_AppendRandomMinRotation);
        F.r_fvector3(m_AppendRandomMaxRotation);
        int cnt		= F.r_u32();
        if (cnt){
        	shared_str	buf;
            for (int i=0; i<cnt; i++){
                F.r_stringZ						(buf);
                m_AppendRandomObjects.push_back	(buf);
            }
        }
    };

    m_Flags.set(flAppendRandom,FALSE);
    
    return true;
}
//----------------------------------------------------

void ESceneObjectTools::Save(IWriter& F)
{
	inherited::Save(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&OBJECT_TOOLS_VERSION,sizeof(OBJECT_TOOLS_VERSION));

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();

    F.open_chunk	(CHUNK_APPEND_RANDOM);
    F.w_fvector3	(m_AppendRandomMinScale);
    F.w_fvector3	(m_AppendRandomMaxScale);
    F.w_fvector3	(m_AppendRandomMinRotation);
    F.w_fvector3	(m_AppendRandomMaxRotation);
    F.w_u32			(m_AppendRandomObjects.size());
    if (m_AppendRandomObjects.size()){
    	for (RStringVecIt it=m_AppendRandomObjects.begin(); it!=m_AppendRandomObjects.end(); it++)
            F.w_stringZ(*it);
    }
    F.close_chunk	();
}
//----------------------------------------------------

bool ESceneObjectTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=OBJECT_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}
//----------------------------------------------------

void ESceneObjectTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&OBJECT_TOOLS_VERSION,sizeof(OBJECT_TOOLS_VERSION));
    
	inherited::SaveSelection(F);
}
//----------------------------------------------------

bool ESceneObjectTools::ExportGame(SExportStreams* F)
{
	if (!inherited::ExportGame(F)) 	return false;

    // export breakable objects
    if (!ExportBreakableObjects(F))	return false;
    if (!ExportClimableObjects(F))	return false;
    
	return true;
}
//----------------------------------------------------

void ESceneObjectTools::GetStaticDesc(int& v_cnt, int& f_cnt)
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	CSceneObject* obj = (CSceneObject*)(*it);
		if (obj->IsStatic()){
			f_cnt	+= obj->GetFaceCount();
    	    v_cnt	+= obj->GetVertexCount();
        }
    }
}
//----------------------------------------------------

