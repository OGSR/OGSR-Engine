#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomOTools.h"
#include "scene.h"
#include "../ECore/Editor/ui_main.h"
#include "builder.h"
#include "CustomObject.h"

// chunks
//----------------------------------------------------
static const u32 CHUNK_VERSION			= 0x0001;
static const u32 CHUNK_OBJECT_COUNT		= 0x0002;
static const u32 CHUNK_OBJECTS			= 0x0003;
static const u32 CHUNK_FLAGS			= 0x0004;
//----------------------------------------------------

bool ESceneCustomOTools::OnLoadSelectionAppendObject(CCustomObject* obj)
{
    string256 buf;
    Scene->GenObjectName(obj->ClassID,buf,obj->Name);
    obj->Name			= buf;
    Scene->AppendObject	(obj, false);
    return true;
}
//----------------------------------------------------

bool ESceneCustomOTools::OnLoadAppendObject(CCustomObject* O)
{
	Scene->AppendObject	(O,false);
    return true;
}
//----------------------------------------------------

bool ESceneCustomOTools::LoadSelection(IReader& F)
{
    int count		= 0;
	F.r_chunk		(CHUNK_OBJECT_COUNT,&count);

    SPBItem* pb = UI->ProgressStart(count,AnsiString().sprintf("Loading %s's...",ClassDesc()).c_str());
    Scene->ReadObjects(F,CHUNK_OBJECTS,OnLoadSelectionAppendObject,pb);
    UI->ProgressEnd(pb);

    return true;
}
//----------------------------------------------------

void ESceneCustomOTools::SaveSelection(IWriter& F)
{
	F.open_chunk	(CHUNK_OBJECTS);
    int count		= 0;
    for(ObjectIt it = m_Objects.begin();it!=m_Objects.end();it++){
    	if ((*it)->Selected() && !(*it)->IsDeleted()){
	        F.open_chunk(count++);
    	    Scene->SaveObject(*it,F);
        	F.close_chunk();
        }
    }
	F.close_chunk	();

	F.w_chunk		(CHUNK_OBJECT_COUNT,&count,sizeof(count));
}
//----------------------------------------------------

bool ESceneCustomOTools::Load(IReader& F)
{
	inherited::Load	(F);

    int count		= 0;
	F.r_chunk		(CHUNK_OBJECT_COUNT,&count);

    SPBItem* pb 	= UI->ProgressStart(count,AnsiString().sprintf("Loading %s...",ClassDesc()).c_str());
    Scene->ReadObjects(F,CHUNK_OBJECTS,OnLoadAppendObject,pb);
    UI->ProgressEnd		(pb);

    return true;
}
//----------------------------------------------------

void ESceneCustomOTools::Save(IWriter& F)
{
	inherited::Save	(F);

    int count		= m_Objects.size();
	F.w_chunk		(CHUNK_OBJECT_COUNT,&count,sizeof(count));

	F.open_chunk	(CHUNK_OBJECTS);
    count			= 0;
    for(ObjectIt it = m_Objects.begin();it!=m_Objects.end();it++){
    	if (!(*it)->IsDeleted()){
            F.open_chunk		(count++);
            Scene->SaveObject	(*it,F);
            F.close_chunk		();
        }
    }
	F.close_chunk	();
}
//----------------------------------------------------

bool ESceneCustomOTools::Export(LPCSTR path)
{
	return true;
}
//----------------------------------------------------
 
bool ESceneCustomOTools::ExportGame(SExportStreams* F)
{
	bool bres=true;
    for(ObjectIt it = m_Objects.begin();it!=m_Objects.end();it++)
        if (!(*it)->ExportGame(F)) bres=false;
	return bres;
}
//----------------------------------------------------

bool ESceneCustomOTools::ExportStatic(SceneBuilder* B)
{
	return B->ParseStaticObjects(m_Objects);
}
//----------------------------------------------------
 