#include "stdafx.h"


#include "ObjectAnimator.h"
#include "motion.h"
 
bool motion_sort_pred	(COMotion* a, 	COMotion* b)	{	return a->name<b->name;}
bool motion_find_pred	(COMotion* a, 	shared_str b)	{	return a->name<b;}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObjectAnimator::CObjectAnimator()
{
	bLoop			= false;
    m_Current		= 0;
    m_Speed			= 1.f;
	m_Name			= "";
}

CObjectAnimator::~CObjectAnimator()
{	
	Clear				();
}

void CObjectAnimator::Clear()
{
	for(auto m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++)
		xr_delete		(*m_it);
	m_Motions.clear		();
    SetActiveMotion		(0);
}

void CObjectAnimator::SetActiveMotion(COMotion* mot)
{
	m_Current			= mot;
    if (m_Current) 		m_MParam.Set(m_Current);
	m_XFORM.identity	();
}

void CObjectAnimator::LoadMotions(LPCSTR fname)
{
	string_path			full_path;
	if (!FS.exist( full_path, "$level$", fname ))
		if (!FS.exist( full_path, "$game_anims$", fname ))
			Debug.fatal(DEBUG_INFO,"Can't find motion file '%s'.",fname);
            
    LPCSTR  ext			= strext(full_path);
    if (ext){
		Clear			();
    	if (0==xr_strcmp(ext,".anm")){
            COMotion* M	= xr_new<COMotion> ();
            if (M->LoadMotion(full_path)) m_Motions.push_back(M);
            else				FATAL("ERROR: Can't load motion. Incorrect file version.");
        }else if (0==xr_strcmp(ext,".anms")){
            IReader* F			= FS.r_open(full_path);
            u32 dwMCnt			= F->r_u32(); VERIFY(dwMCnt);
            for (u32 i=0; i<dwMCnt; i++){
                COMotion* M		= xr_new<COMotion> ();
                bool bRes		= M->Load(*F);
                if (!bRes)		FATAL("ERROR: Can't load motion. Incorrect file version.");
                m_Motions.push_back(M);
            }
            FS.r_close		(F);
        }
        std::sort(m_Motions.begin(),m_Motions.end(),motion_sort_pred);
    }
}

void CObjectAnimator::Load(const char * name)
{
	m_Name				= name;
	LoadMotions			(name); 
	SetActiveMotion		(0);
}

void CObjectAnimator::Update(float dt)
{
	if (m_Current){
		Fvector R,P;
		m_Current->_Evaluate(m_MParam.Frame(),P,R);
		m_MParam.Update	(dt,m_Speed,bLoop);
		m_XFORM.setXYZi	(R.x,R.y,R.z);
        m_XFORM.translate_over(P);
	}
}

COMotion* CObjectAnimator::Play(bool loop, LPCSTR name)
{
	if (name&&name[0]){
		auto it = std::lower_bound(m_Motions.begin(),m_Motions.end(),name,motion_find_pred);
        if ((it!=m_Motions.end())&&(0==xr_strcmp((*it)->Name(),name))){
            bLoop 		= loop;
            SetActiveMotion(*it);
			m_MParam.Play	();
            return 		*it;
        }else{
            Debug.fatal	(DEBUG_INFO,"OBJ ANIM::Cycle '%s' not found.",name);
            return NULL;
        }
    }else{
        if (!m_Motions.empty()){
            bLoop 		= loop;
            SetActiveMotion(m_Motions.front());
			m_MParam.Play	();
            return 		m_Motions.front();
        }else{
            Debug.fatal	(DEBUG_INFO,"OBJ ANIM::Cycle '%s' not found.",name);
            return NULL;
        }
    }
}

void CObjectAnimator::Stop()
{
	SetActiveMotion		(0);
	m_MParam.Stop		();
}

float CObjectAnimator::GetLength		()
{
	if(!m_Current) return 0.0f;
	float res = m_Current->Length()/m_Current->FPS();
	return res; 
}
