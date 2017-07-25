//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESound_Source.h"
#include "../../xrSound/SoundRender_Source.h"
#include "d3dutils.h"
#include "../ECore/Editor/ui_main.h"
#include "IGame_Persistent.h"
//----------------------------------------------------

#define VIS_RADIUS 0.25f
#define SOUND_SEL0_COLOR 	0x00A0A0F0
#define SOUND_SEL1_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000
//----------------------------------------------------

#define SOUND_SOURCE_VERSION   			0x0014
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x1001
#define SOUND_CHUNK_TYPE				0x1002
#define SOUND_CHUNK_SOURCE_NAME			0x1003
#define SOUND_CHUNK_SOURCE_PARAMS		0x1004
#define SOUND_CHUNK_SOURCE_FLAGS		0x1005
#define SOUND_CHUNK_SOURCE_PARAMS2		0x1006
#define SOUND_CHUNK_SOURCE_PARAMS3		0x1007
#define SOUND_CHUNK_GAME_PARAMS			0x1008
//----------------------------------------------------

ESoundSource::ESoundSource(LPVOID data, LPCSTR name)
	:CCustomObject(data,name)
{
	Construct(data);
}

void ESoundSource::Construct(LPVOID data)
{
	ClassID					= OBJCLASS_SOUND_SRC;

    m_Type					= stStaticSource;

    m_WAVName				= "";
    m_Params.volume 		= 1.f;
    m_Params.freq			= 1.f;
    m_Params.min_distance   = 1.f;
    m_Params.max_distance   = 300.f;
    m_Params.max_ai_distance= 300.f;
    m_Params.position.set	(0,0,0);

    m_Flags.set				(flLooped,TRUE);
	m_Command				= stNothing; 
	m_RandomPause.set		(0.f,0.f);
	m_ActiveTime.set		(0.f,0.f);
    m_PlayTime.set			(0.f,0.f);
    m_NextTime				= 0;
    m_StopTime				= 0;
}

ESoundSource::~ESoundSource()
{
	m_Source.destroy		();
}

//----------------------------------------------------

bool ESoundSource::GetBox( Fbox& box )
{
	box.set( m_Params.position, m_Params.position );
	box.min.sub(m_Params.max_distance);
	box.max.add(m_Params.max_distance);
	return true;
}

void ESoundSource::Render(int priority, bool strictB2F)
{
    if((1==priority)&&(false==strictB2F)){
	 	RCache.set_xform_world	(Fidentity);
	 	Device.SetShader		(Device.m_WireShader);
        u32 clr0				= Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL0_COLOR:SOUND_NORM_COLOR);
        u32 clr1				= Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL1_COLOR:SOUND_NORM_COLOR);
        if (Selected()){ 
        	DU.DrawLineSphere	(m_Params.position, m_Params.max_distance, clr1, true);
        	DU.DrawLineSphere	(m_Params.position, m_Params.min_distance, clr0, false);
        }else{
			DU.DrawSound		(m_Params.position,VIS_RADIUS, clr1);
        }
    }
}

bool ESoundSource::FrustumPick(const CFrustum& frustum)
{
    return (frustum.testSphere_dirty(m_Params.position,VIS_RADIUS))?true:false;
}

bool ESoundSource::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	Fvector ray2;
	ray2.sub( m_Params.position, start );

	float d = ray2.dotproduct(direction);
	if( d > 0  ){
		float d2 = ray2.magnitude();
		if( ((d2*d2-d*d) < (VIS_RADIUS*VIS_RADIUS)) && (d>VIS_RADIUS)){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
		}
	}

	return false;
}
//----------------------------------------------------

bool ESoundSource::Load(IReader& F)
{
	u32 version 	= 0;

    if(F.r_chunk(SOUND_CHUNK_VERSION,&version)){
        if(version!=SOUND_SOURCE_VERSION){
            ELog.Msg( mtError, "ESoundSource: Unsupported version.");
            return false;
        }
    }else return false;

	inherited::Load			(F);

    R_ASSERT(F.find_chunk(SOUND_CHUNK_TYPE));
	m_Type					= ESoundType(F.r_u32());

    R_ASSERT(F.find_chunk(SOUND_CHUNK_SOURCE_NAME));
    F.r_stringZ		(m_WAVName);

    
    if (F.find_chunk(SOUND_CHUNK_SOURCE_PARAMS3)){
       	m_Params.base_volume	= 1.f;
    	F.r_fvector3			(m_Params.position);
       	m_Params.volume			= F.r_float();
        m_Params.freq			= F.r_float();
        m_Params.min_distance	= F.r_float();
        m_Params.max_distance	= F.r_float();
        m_Params.max_ai_distance= F.r_float();
    }else if (F.find_chunk(SOUND_CHUNK_SOURCE_PARAMS2)){
       	m_Params.base_volume	= 1.f;
    	F.r_fvector3			(m_Params.position);
       	m_Params.volume			= F.r_float();
        m_Params.freq			= F.r_float();
        m_Params.min_distance	= F.r_float();
        m_Params.max_distance	= F.r_float();
        m_Params.max_ai_distance= F.r_float();
    }else{
    	if (!F.find_chunk(SOUND_CHUNK_SOURCE_PARAMS)){
            ELog.DlgMsg( mtError, "ESoundSource: Can't load Sound Source '%s'. Unsupported version.",*m_WAVName);
            return false;
        }
       	m_Params.base_volume	= 1.f;
    	F.r_fvector3			(m_Params.position);
       	m_Params.volume			= F.r_float();
        m_Params.freq			= F.r_float();
        m_Params.min_distance	= F.r_float();
        m_Params.max_distance	= F.r_float();
        m_Params.max_ai_distance= m_Params.max_distance;
    }

    if(F.find_chunk(SOUND_CHUNK_SOURCE_FLAGS))
		F.r			(&m_Flags,sizeof(m_Flags));
    

    if(F.find_chunk(SOUND_CHUNK_GAME_PARAMS)){
	    F.r_fvector2			(m_RandomPause);
    	F.r_fvector2			(m_ActiveTime);
	    F.r_fvector2			(m_PlayTime);
	}
    
    ResetSource		();

    switch (m_Type){
    case stStaticSource: 
    	if (m_Flags.is(flPlaying)) 		Play(); 
    	if (m_Flags.is(flSimulating)) 	Simulate(); 
    break;
    default: THROW;
    }
    return true;
}

void ESoundSource::Save(IWriter& F)
{
	inherited::Save	(F);

	F.open_chunk	(SOUND_CHUNK_VERSION);
	F.w_u16			(SOUND_SOURCE_VERSION);
	F.close_chunk	();

    F.w_chunk		(SOUND_CHUNK_TYPE,&m_Type,sizeof(m_Type));

    F.open_chunk	(SOUND_CHUNK_SOURCE_NAME);
    F.w_stringZ		(m_WAVName);
    F.close_chunk	();

    F.w_chunk		(SOUND_CHUNK_SOURCE_FLAGS,&m_Flags,sizeof(m_Flags));
    
    F.open_chunk	(SOUND_CHUNK_SOURCE_PARAMS3);
    F.w_fvector3	(m_Params.position);
    F.w_float		(m_Params.volume);
    F.w_float		(m_Params.freq);
    F.w_float		(m_Params.min_distance);
    F.w_float		(m_Params.max_distance);
    F.w_float		(m_Params.max_ai_distance);
    F.close_chunk	();

    F.open_chunk	(SOUND_CHUNK_GAME_PARAMS);
    F.w_fvector2	(m_RandomPause);
    F.w_fvector2	(m_ActiveTime);
    F.w_fvector2	(m_PlayTime);
    F.close_chunk	();
}

//----------------------------------------------------

void ESoundSource::OnChangeWAV	(PropValue* prop)
{
	BOOL bPlay 		= !!m_Source._feedback();
	ResetSource		();
	if (bPlay) 		Play();
}

void ESoundSource::OnChangeSource	(PropValue* prop)
{
	m_Source.set_params			(&m_Params);
}

void ESoundSource::OnControlClick(ButtonValue* V, bool& bModif, bool& bSafe)
{
    switch (V->btn_num){
    case 0: Play();		break;
    case 1: Stop();		break;
    case 2: Simulate(); break;
	}
    UI->RedrawScene();
    bModif = false;
}

void ESoundSource::FillProp(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
	ButtonValue* B;
    B=PHelper().CreateButton	(values, PrepareKey(pref,"Custom\\Controls"), 	"Play,Stop,Simulate",0);
    B->OnBtnClickEvent.bind		(this,&ESoundSource::OnControlClick);
    PropValue* V;
    V=PHelper().CreateChoose	(values,PrepareKey(pref,"Source\\WAVE name"),	&m_WAVName,					smSoundSource);
    V->OnChangeEvent.bind		(this,&ESoundSource::OnChangeWAV);
	V=PHelper().CreateFloat		(values,PrepareKey(pref,"Source\\Frequency"),	&m_Params.freq,				0.0f,2.f);
    V->OnChangeEvent.bind		(this,&ESoundSource::OnChangeSource);
	V=PHelper().CreateFloat		(values,PrepareKey(pref,"Source\\Volume"),		&m_Params.volume,			0.0f,1.f);
    V->OnChangeEvent.bind		(this,&ESoundSource::OnChangeSource);
	V=PHelper().CreateFloat		(values,PrepareKey(pref,"Source\\Min dist"),	&m_Params.min_distance,		0.1f,1000.f,0.1f,1);
    V->Owner()->Enable			(FALSE);
	V=PHelper().CreateFloat		(values,PrepareKey(pref,"Source\\Max dist"),	&m_Params.max_distance,		0.1f,1000.f,0.1f,1);
    V->Owner()->Enable			(FALSE);
	V=PHelper().CreateFloat		(values,PrepareKey(pref,"Source\\Max ai dist"),	&m_Params.max_ai_distance,	0.1f,1000.f,0.1f,1);
    V->Owner()->Enable			(FALSE);
	PHelper().CreateCaption		(values,PrepareKey(pref,"Game\\Active time\\Hint"),	"Zero - play sound looped round the clock.");
	PHelper().CreateTime		(values,PrepareKey(pref,"Game\\Active time\\From"),	&m_ActiveTime.x);
	PHelper().CreateTime		(values,PrepareKey(pref,"Game\\Active time\\To"),	&m_ActiveTime.y);
	PHelper().CreateCaption		(values,PrepareKey(pref,"Game\\Play time\\Hint"),	"Zero - play sound once.");
	PHelper().CreateTime		(values,PrepareKey(pref,"Game\\Play time\\From"),	&m_PlayTime.x);
	PHelper().CreateTime		(values,PrepareKey(pref,"Game\\Play time\\To"),		&m_PlayTime.y);
	PHelper().CreateCaption		(values,PrepareKey(pref,"Game\\Pause delta\\Hint"),	"Zero - play sound looped.");
	PHelper().CreateTime		(values,PrepareKey(pref,"Game\\Pause delta\\From"),	&m_RandomPause.x);
	PHelper().CreateTime		(values,PrepareKey(pref,"Game\\Pause delta\\To"),	&m_RandomPause.y);
//	V=PHelper().CreateFlag32		(values,PHelper().PrepareKey(pref,"Looped"),	&m_Flags,				flLooped);
//    V->OnChangeEvent			= OnChangeSource;
}
//----------------------------------------------------

bool ESoundSource::GetSummaryInfo(SSceneSummary* inf)
{
	inherited::GetSummaryInfo	(inf);
	if (m_WAVName.size()) inf->waves.insert(*m_WAVName);
    inf->sound_source_cnt++;
	return true;
}

void ESoundSource::OnFrame()
{
	inherited::OnFrame();
    switch (m_Command){
    case stPlay: 	
    	m_Source.play		(0,m_Flags.is(flLooped));
		m_Source.set_params	(&m_Params);
		m_Command			= stNothing; 
		m_Flags.set			(flPlaying,TRUE);
    break;
    case stStop: 
		m_Source.stop		();
        m_Command			= stNothing; 
		m_Flags.set			(flPlaying,FALSE);
		m_Flags.set			(flSimulating,FALSE);
    break;
    case stSimulate:{
		m_Flags.set			(flSimulating,TRUE);
    	if ((fis_zero(m_ActiveTime.x)&&fis_zero(m_ActiveTime.y))||
        	((g_pGamePersistent->Environment().GetGameTime()>m_ActiveTime.x)&&(g_pGamePersistent->Environment().GetGameTime()<m_ActiveTime.y))){
            if (0==m_Source._feedback()){
            	if (fis_zero(m_RandomPause.x)&&fis_zero(m_RandomPause.y)){    
                    m_Source.play			(0,sm_Looped);
                    m_Source.set_params		(&m_Params);
                    m_StopTime				= 0xFFFFFFFF;
				}else{
                    if (Device.dwTimeGlobal>=m_NextTime){
                    	bool bFullPlay		= fis_zero(m_PlayTime.x)&&fis_zero(m_PlayTime.y);
                        m_Source.play		(0,bFullPlay?0:sm_Looped);
                        m_Source.set_params	(&m_Params);
                        if (bFullPlay){
                            m_StopTime		= 0xFFFFFFFF;
                            m_NextTime		= Device.dwTimeGlobal+m_Source._handle()->length_ms()+Random.randF(m_RandomPause.x,m_RandomPause.y)*1000;
                        }else{
                            m_StopTime		= bFullPlay?0:Device.dwTimeGlobal+Random.randF(m_PlayTime.x,m_PlayTime.y)*1000;
                            m_NextTime		= m_StopTime+Random.randF(m_RandomPause.x,m_RandomPause.y)*1000;
                        }
                    }
                }
            }else{
                if (Device.dwTimeGlobal>=m_StopTime)
	            	m_Source.stop_deffered();
            }
            
        }else{
            if (0!=m_Source._feedback())
            	m_Source.stop_deffered();
        }
    }break;
    case stNothing:    		break;
    default: THROW;
    }
}

void ESoundSource::ResetSource()
{
	m_Source.destroy();
	if (m_WAVName.size()){ 
    	m_Source.create		(*m_WAVName,st_Effect,sg_Undefined);
        CSoundRender_Source* src= (CSoundRender_Source*)m_Source._handle();
        m_Params.min_distance	= src->m_fMinDist;
        m_Params.max_distance	= src->m_fMaxDist;
        m_Params.max_ai_distance= src->m_fMaxAIDist;
        ExecCommand			(COMMAND_UPDATE_PROPERTIES);
    }
	m_Source.set_params(&m_Params);
}

void ESoundSource::SetSourceWAV(LPCSTR fname)
{
    m_WAVName		= fname;
    ResetSource		();
}

bool ESoundSource::ExportGame(SExportStreams* F)
{
	SExportStreamItem& I	= F->sound_static;
	I.stream.open_chunk		(I.chunk++);
	I.stream.open_chunk		(0);
    I.stream.w_stringZ		(m_WAVName);
    I.stream.w_fvector3		(m_Params.position);
    I.stream.w_float		(m_Params.volume);
    I.stream.w_float		(m_Params.freq);
    I.stream.w_u32			(m_ActiveTime.x*1000);
    I.stream.w_u32			(m_ActiveTime.y*1000);
    I.stream.w_u32			(m_PlayTime.x*1000);
    I.stream.w_u32			(m_PlayTime.y*1000);
    I.stream.w_u32			(m_RandomPause.x*1000);
    I.stream.w_u32			(m_RandomPause.y*1000);
    I.stream.close_chunk	();
    I.stream.close_chunk	();
    return true;
}

