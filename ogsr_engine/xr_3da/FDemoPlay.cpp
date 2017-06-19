// CDemoPlay.cpp: implementation of the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "fdemoplay.h"
#include "xr_ioconsole.h"
#include "motion.h"
#include "Render.h"
#include "CameraManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemoPlay::CDemoPlay(const char *name, float ms, u32 cycles, float life_time) : CEffectorCam(cefDemo,life_time/*,FALSE*/)
{
	Msg					("*** Playing demo: %s",name);
	Console->Execute	("hud_weapon 0");
	if(g_bBenchmark)	Console->Execute	("hud_draw 0");

	fSpeed				= ms;
	dwCyclesLeft		= cycles?cycles:1;

	m_pMotion			= 0;
	m_MParam			= 0;
	string_path			nm, fn;
	strcpy_s			(nm,sizeof(nm),name);	
	
	if (strext(nm))	
		strcpy(strext(nm),".anm");

	if ( FS.exist(fn,"$level$",nm) || FS.exist(fn,"$game_anims$",nm) )
	{
		m_pMotion				= xr_new<COMotion>		();
		m_pMotion->LoadMotion	(fn);
		m_MParam				= xr_new<SAnimParams>	();
		m_MParam->Set			(m_pMotion);
		m_MParam->Play			();
	}else{
		if (!FS.exist(name))						{
			g_pGameLevel->Cameras().RemoveCamEffector	(cefDemo);
			return		;
		}
		IReader*	fs	= FS.r_open	(name);
		u32 sz			= fs->length();
		if				(sz%sizeof(Fmatrix) != 0)	{
			FS.r_close	(fs);
			g_pGameLevel->Cameras().RemoveCamEffector	(cefDemo);
			return		;
		}
		
		seq.resize		(sz/sizeof(Fmatrix));
		m_count			= seq.size();
		CopyMemory	(&*seq.begin(),fs->pointer(),sz);
		FS.r_close		(fs);
		Log				("~ Total key-frames: ",m_count);
	}
	stat_started		= FALSE;
	Device.PreCache		(50);
}

CDemoPlay::~CDemoPlay		()
{
	stat_Stop				();
	xr_delete				(m_pMotion	);
	xr_delete				(m_MParam	);
	Console->Execute		("hud_weapon 1");
	if(g_bBenchmark)		Console->Execute	("hud_draw 1");
}

void CDemoPlay::stat_Start	()
{
	if (stat_started)		return;
	stat_started			= TRUE				;
	Sleep					(1)					;
	stat_StartFrame			=	Device.dwFrame	;
	stat_Timer_frame.Start	()					;
	stat_Timer_total.Start	()					;
	stat_table.clear		()					;
	stat_table.reserve		(1024)				;
	fStartTime				= 0;
}

extern string512		g_sBenchmarkName;

void CDemoPlay::stat_Stop	()
{
	if (!stat_started)		return;
	stat_started			= FALSE;
	float	stat_total		= stat_Timer_total.GetElapsed_sec	();

	float	rfps_min, rfps_max, rfps_middlepoint, rfps_average	;

	// total
	u32	dwFramesTotal		= Device.dwFrame-stat_StartFrame	;
	rfps_average			= float(dwFramesTotal)/stat_total	;

	// min/max/average
	rfps_min				= flt_max;
	rfps_max				= flt_min;
	rfps_middlepoint		= 0;
	for (u32	it=1; it<stat_table.size(); it++)
	{
		float	fps	= 1.f / stat_table[it];
		if		(fps<rfps_min)	rfps_min = fps;
		if		(fps>rfps_max)	rfps_max = fps;
		rfps_middlepoint	+=	fps;
	}
	rfps_middlepoint		/= float(stat_table.size()-1);

	Msg("* [DEMO] FPS: average[%f], min[%f], max[%f], middle[%f]",rfps_average,rfps_min,rfps_max,rfps_middlepoint);

	if(g_bBenchmark){
		string_path			fname;

		if(xr_strlen(g_sBenchmarkName))
			sprintf_s	(fname,sizeof(fname),"%s.result",g_sBenchmarkName);
		else
			strcpy_s	(fname,sizeof(fname),"benchmark.result");


		FS.update_path		(fname,"$app_data_root$",fname);
		CInifile			res		(fname,FALSE,FALSE,TRUE);
		res.w_float			("general","renderer",	float(::Render->get_generation())/10.f,	"dx-level required"		);
		res.w_float			("general","min",		rfps_min,								"absolute minimum"		);
		res.w_float			("general","max",		rfps_max,								"absolute maximum"		);
		res.w_float			("general","average",	rfps_average,							"average for this run"	);
		res.w_float			("general","middle",	rfps_middlepoint,						"per-frame middle-point");
		for (u32	it=1; it<stat_table.size(); it++)
		{
			string32		id;
			sprintf_s		(id,sizeof(id),"%7d",it);
			for (u32 c=0; id[c]; c++) if (' '==id[c]) id[c] = '0';
			res.w_float		("per_frame_stats",	id, 1.f / stat_table[it]);
		}

		Console->Execute	("quit");
	}
}

#define FIX(a) while (a>=m_count) a-=m_count
void spline1( float t, Fvector *p, Fvector *ret )
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	float     m[4];

	ret->x=0.0f;
	ret->y=0.0f;
	ret->z=0.0f;
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

	for( int i=0; i<4; i++ )
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}

BOOL CDemoPlay::Process(Fvector &P, Fvector &D, Fvector &N, float& fFov, float& fFar, float& fAspect)
{
	// skeep a few frames before counting
	if (Device.dwPrecacheFrame)	return	TRUE;
	stat_Start					()		;

	// Per-frame statistics
	{
		stat_table.push_back		(stat_Timer_frame.GetElapsed_sec());
		stat_Timer_frame.Start		();
	}

	// Process motion
	if (m_pMotion)
	{
		Fvector R;
		Fmatrix mRotate;
		m_pMotion->_Evaluate	(m_MParam->Frame(),P,R);
		m_MParam->Update		(Device.fTimeDelta,1.f,true);
		fLifeTime				-= Device.fTimeDelta;
		if (m_MParam->bWrapped)	{ stat_Stop(); stat_Start(); }
		mRotate.setXYZi			(R.x,R.y,R.z);
		D.set					(mRotate.k);
		N.set					(mRotate.j);
	}
	else
	{
		if (seq.empty()) {
			g_pGameLevel->Cameras().RemoveCamEffector(cefDemo);
			return		TRUE;
		}

		fStartTime		+=	Device.fTimeDelta;
		
		float	ip;
		float	p		=	fStartTime/fSpeed;
		float	t		=	modff(p, &ip);
		int		frame	=	iFloor(ip);
		VERIFY	(t>=0);
		
		if (frame>=m_count)
		{
			dwCyclesLeft			--	;
			if (0==dwCyclesLeft)	return FALSE;
			fStartTime				= 0	;
			// just continue
			// stat_Stop			();
			// stat_Start			();
		}
		
		int f1=frame; FIX(f1);
		int f2=f1+1;  FIX(f2);
		int f3=f2+1;  FIX(f3);
		int f4=f3+1;  FIX(f4);
		
		Fmatrix *m1,*m2,*m3,*m4;
		Fvector v[4];
		m1 = (Fmatrix *) &seq[f1];
		m2 = (Fmatrix *) &seq[f2];
		m3 = (Fmatrix *) &seq[f3];
		m4 = (Fmatrix *) &seq[f4];
		
		for (int i=0; i<4; i++) {
			v[0].x = m1->m[i][0]; v[0].y = m1->m[i][1];  v[0].z = m1->m[i][2];
			v[1].x = m2->m[i][0]; v[1].y = m2->m[i][1];  v[1].z = m2->m[i][2];
			v[2].x = m3->m[i][0]; v[2].y = m3->m[i][1];  v[2].z = m3->m[i][2];
			v[3].x = m4->m[i][0]; v[3].y = m4->m[i][1];  v[3].z = m4->m[i][2];
			spline1	( t, &(v[0]), (Fvector *) &(Device.mView.m[i][0]) );
		}
		
		Fmatrix mInvCamera;
		mInvCamera.invert(Device.mView);
		N.set( mInvCamera._21, mInvCamera._22, mInvCamera._23 );
		D.set( mInvCamera._31, mInvCamera._32, mInvCamera._33 );
		P.set( mInvCamera._41, mInvCamera._42, mInvCamera._43 );
		
		fLifeTime-=Device.fTimeDelta;
	}
	return TRUE;
}
