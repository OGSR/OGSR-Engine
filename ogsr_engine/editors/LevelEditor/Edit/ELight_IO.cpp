//----------------------------------------------------
// file: ELight.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ELight.h"
#include "LightAnimLibrary.h"
#include "escenelighttools.h"

static const u32 LIGHT_VERSION   			= 0x0011;
//----------------------------------------------------
enum{
    LIGHT_CHUNK_VERSION			= 0xB411,
    LIGHT_CHUNK_FLAG			= 0xB413,
    LIGHT_CHUNK_BRIGHTNESS		= 0xB425,
    LIGHT_CHUNK_D3D_PARAMS     	= 0xB435,
    LIGHT_CHUNK_USE_IN_D3D		= 0xB436,
    LIGHT_CHUNK_ROTATE			= 0xB437,
    LIGHT_CHUNK_ANIMREF			= 0xB438,
    LIGHT_CHUNK_FALLOFF_TEXTURE	= 0xB439,
    LIGHT_CHUNK_FUZZY_DATA		= 0xB440,
    LIGHT_CHUNK_LCONTROL		= 0xB441,
    LIGHT_CHUNK_PARAMS     		= 0xB442,
};
//----------------------------------------------------

bool CLight::Load(IReader& F)
{
	u32 version = 0;

    string1024 buf;
    R_ASSERT(F.r_chunk(LIGHT_CHUNK_VERSION,&version));
    if((version!=0x0010)&&(version!=LIGHT_VERSION)){
        ELog.DlgMsg( mtError, "CLight: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (F.find_chunk(LIGHT_CHUNK_PARAMS)){
        m_Type			= (ELight::EType)F.r_u32();            
        F.r_fcolor		(m_Color);  	        
        m_Brightness   	= F.r_float();			
        m_Range			= F.r_float();			
        m_Attenuation0	= F.r_float();			
        m_Attenuation1	= F.r_float();			
        m_Attenuation2	= F.r_float();			
        m_Cone			= F.r_float();
        m_VirtualSize	= F.r_float();
    }else{
	    R_ASSERT(F.find_chunk(LIGHT_CHUNK_D3D_PARAMS));
        Flight			d3d;
	    F.r				(&d3d,sizeof(d3d));
        m_Type			= (ELight::EType)d3d.type;   	        
        m_Color.set		(d3d.diffuse); 	        
        PPosition		= d3d.position;
        m_Range			= d3d.range;			
        m_Attenuation0	= d3d.attenuation0;		
        m_Attenuation1	= d3d.attenuation1;		
        m_Attenuation2	= d3d.attenuation2;		
        m_Cone			= d3d.phi;				
	    R_ASSERT(F.r_chunk(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness));
    }
    
    R_ASSERT(F.r_chunk(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D));
    if(version==0x0010){
    	if (F.find_chunk(LIGHT_CHUNK_ROTATE)){
        	F.r_fvector3(FRotation);
        }else{
            FRotation.set(0,0,0);
        }
    }

    if (F.find_chunk(LIGHT_CHUNK_FLAG)) 	F.r(&m_Flags.flags,sizeof(m_Flags));
    if (F.find_chunk(LIGHT_CHUNK_LCONTROL))	F.r(&m_LControl,sizeof(m_LControl));

	if (D3DLIGHT_DIRECTIONAL==m_Type){
    	ESceneLightTools* lt = dynamic_cast<ESceneLightTools*>(ParentTools); VERIFY(lt);
        lt->m_SunShadowDir.set(FRotation.x,FRotation.y);
        ELog.DlgMsg( mtError, "CLight: Can't load sun.");
    	return false;
    } 

    if (F.find_chunk(LIGHT_CHUNK_ANIMREF)){
    	F.r_stringZ(buf,sizeof(buf));
        m_pAnimRef	= LALib.FindItem(buf);
        if (!m_pAnimRef) ELog.Msg(mtError, "Can't find light animation: %s",buf);
    }

    if (F.find_chunk(LIGHT_CHUNK_FALLOFF_TEXTURE)){
    	F.r_stringZ(m_FalloffTex);
    }

    if (F.find_chunk(LIGHT_CHUNK_FUZZY_DATA)){
        m_FuzzyData	= xr_new<SFuzzyData>();
        m_FuzzyData->Load(F);
		m_Flags.set(ELight::flPointFuzzy,TRUE);
    }else{
		m_Flags.set(ELight::flPointFuzzy,FALSE);
    }
    
	UpdateTransform	();

    return true;
}
//----------------------------------------------------

void CLight::Save(IWriter& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(LIGHT_CHUNK_VERSION);
	F.w_u16			(LIGHT_VERSION);
	F.close_chunk	();

	F.open_chunk	(LIGHT_CHUNK_PARAMS);
    F.w_u32		    (m_Type);     
    F.w_fcolor	    (m_Color);  
    F.w_float	    (m_Brightness);
    F.w_float	    (m_Range);	
    F.w_float	    (m_Attenuation0);
    F.w_float	    (m_Attenuation1);
    F.w_float	    (m_Attenuation2);
    F.w_float	    (m_Cone);
    F.w_float	    (m_VirtualSize);
	F.close_chunk	();

    F.w_chunk		(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D,sizeof(m_UseInD3D));
    F.w_chunk		(LIGHT_CHUNK_FLAG,&m_Flags,sizeof(m_Flags));
    F.w_chunk		(LIGHT_CHUNK_LCONTROL,&m_LControl,sizeof(m_LControl));

    if (m_pAnimRef){
		F.open_chunk(LIGHT_CHUNK_ANIMREF);
		F.w_stringZ	(m_pAnimRef->cName);
		F.close_chunk();
    }

    if (m_FalloffTex.size()){
	    F.open_chunk(LIGHT_CHUNK_FALLOFF_TEXTURE);
    	F.w_stringZ	(m_FalloffTex);
	    F.close_chunk();
    }

	if (m_Flags.is(ELight::flPointFuzzy)){
        VERIFY(m_FuzzyData);
        F.open_chunk(LIGHT_CHUNK_FUZZY_DATA);
        m_FuzzyData->Save(F);
        F.close_chunk();
    }
}
//----------------------------------------------------


