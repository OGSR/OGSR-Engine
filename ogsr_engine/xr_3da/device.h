#pragma once

// Note:
// ZNear - always 0.0f
// ZFar  - always 1.0f

class	ENGINE_API	CResourceManager;
class	ENGINE_API	CGammaControl;

#include "pure.h"
#include "hw.h"
#include "ftimer.h"
#include "stats.h"
#include "xr_effgamma.h"
#include "shader.h"
#include "R_Backend.h"

extern u32 g_dwFPSlimit;

#define VIEWPORT_NEAR 0.05f //0.1f

#define DEVICE_RESET_PRECACHE_FRAME_COUNT 10

// refs
class ENGINE_API CRenderDevice 
{
public:
	class ENGINE_API CSecondVPParams //--#SM+#-- +SecondVP+
	{
	public:
		bool m_bCamReady; // Флаг готовности камеры (FOV, позиция, и т.п) к рендеру второго вьюпорта
	private:
		bool m_bIsActive;  // Флаг активации рендера во второй вьюпорт
		u8   m_FrameDelay; // На каком кадре с момента прошлого рендера во второй вьюпорт мы начнём новый (не может быть меньше 2 - каждый второй кадр, чем больше тем более низкий FPS во втором вьюпорте)
	public:
		IC bool IsSVPActive() { return m_bIsActive; }
		void    SetSVPActive(bool bState);
		bool    IsSVPFrame();

		IC u8 GetSVPFrameDelay() { return m_FrameDelay; }
		void  SetSVPFrameDelay(u8 iDelay)
		{
			m_FrameDelay = iDelay;
			clamp<u8>(m_FrameDelay, 2, u8(-1));
		}
	};
private:
    // Main objects used for creating and rendering the 3D scene
    LONG_PTR								m_dwWindowStyle;
    RECT									m_rcWindowBounds;
    RECT									m_rcWindowClient;

	u32										Timer_MM_Delta;
	CTimer_paused							Timer;
	CTimer_paused							TimerGlobal;
	CTimer									TimerMM;

	void									_Create		(LPCSTR shName);
	void									_Destroy	(BOOL	bKeepTextures);
	void									_SetupStates();
public:
    HWND									m_hWnd;
	LRESULT									MsgProc		(HWND,UINT,WPARAM,LPARAM);

	u32										dwFrame;
	u32										dwPrecacheFrame;
	u32										dwPrecacheTotal;

	u32										dwWidth, dwHeight;
	float									fWidth_2, fHeight_2;
	BOOL									b_is_Ready;
	BOOL									b_is_Active;
	void									OnWM_Activate(WPARAM wParam, LPARAM lParam);
public:
	ref_shader								m_WireShader;
	ref_shader								m_SelectionShader;

	BOOL									m_bNearer;
	void									SetNearer	(BOOL enabled)
	{
		if (enabled&&!m_bNearer){
			m_bNearer						= TRUE;
			mProject._43					-= EPS_L;
		}else if (!enabled&&m_bNearer){
			m_bNearer						= FALSE;
			mProject._43					+= EPS_L;
		}
		RCache.set_xform_project			(mProject);
	}
public:
	// Registrators
	CRegistrator	<pureRender			>			seqRender;
	CRegistrator	<pureAppActivate	>			seqAppActivate;
	CRegistrator	<pureAppDeactivate	>			seqAppDeactivate;
	CRegistrator	<pureAppStart		>			seqAppStart;
	CRegistrator	<pureAppEnd			>			seqAppEnd;
	CRegistrator	<pureFrame			>			seqFrame;
	CRegistrator	<pureFrame			>			seqFrameMT;
	CRegistrator	<pureDeviceReset	>			seqDeviceReset;
	xr_vector		<fastdelegate::FastDelegate0<> >	seqParallel;
	CSecondVPParams m_SecondViewport; //--#SM+#-- +SecondVP+

	// Dependent classes
	CResourceManager*						Resources;	  
	CStats*									Statistic;
	CGammaControl							Gamma;

	// Engine flow-control
	float									fTimeDelta;
	float									fTimeGlobal;
	u32										dwTimeDelta;
	u32										dwTimeGlobal;
	u32										dwTimeContinual;

	// Cameras & projection
	Fvector									vCameraPosition;
	Fvector									vCameraDirection;
	Fvector									vCameraTop;
	Fvector									vCameraRight;
	Fmatrix									mView;
	Fmatrix									mProject;
	Fmatrix									mFullTransform;
	Fmatrix									mInvFullTransform;

	// Copies of corresponding members. Used for synchronization.
	Fvector vCameraPositionSaved;
	Fvector vCameraDirectionSaved;
	Fvector vCameraTopSaved;
	Fvector vCameraRightSaved;

	Fmatrix mViewSaved;
	Fmatrix mProjectSaved;
	Fmatrix mFullTransformSaved;

	float									fFOV;
	float									fASPECT;
	
	CRenderDevice			()
		#ifdef PROFILE_CRITICAL_SECTIONS
			: mt_csEnter(MUTEX_PROFILE_ID(CRenderDevice::mt_csEnter))
			,mt_csLeave(MUTEX_PROFILE_ID(CRenderDevice::mt_csLeave))
		#endif // PROFILE_CRITICAL_SECTIONS
	{
	    m_hWnd              = NULL;
		b_is_Active			= FALSE;
		b_is_Ready			= FALSE;
		Timer.Start			();
		m_bNearer			= FALSE;
		//--#SM+#-- +SecondVP+
		m_SecondViewport.SetSVPActive(false);
		m_SecondViewport.SetSVPFrameDelay(2);
		m_SecondViewport.m_bCamReady = false;
	};

	void	Pause							(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason);
	BOOL	Paused							();

	// Scene control
	void PreCache							(u32 frames);
	BOOL Begin								();
	void Clear								();
	void End								();
	void FrameMove							();
	
	void overdrawBegin						();
	void overdrawEnd						();

	// Mode control
	void DumpFlags							();
	IC	 CTimer_paused* GetTimerGlobal		()	{ return &TimerGlobal;								}
	u32	 TimerAsync							()	{ return TimerGlobal.GetElapsed_ms();				}
	u32	 TimerAsync_MMT						()	{ return TimerMM.GetElapsed_ms() +	Timer_MM_Delta; }

	// Creation & Destroying
	void Create								(void);
	void Run								(void);
	void Destroy							(void);
	void Reset								(bool precache = true);

	void Initialize							(void);
	void ShutDown							(void);

public:
	void time_factor						(const float &time_factor)
	{
		Timer.time_factor		(time_factor);
		TimerGlobal.time_factor	(time_factor);
	}
	

/*#pragma warning( push )
#pragma warning( disable : 4172 )
	IC const float& time_factor() const
	{
		VERIFY					(Timer.time_factor() == TimerGlobal.time_factor());
		return					(Timer.time_factor());
	}
#pragma warning( pop )*/

	// Multi-threading
	xrCriticalSection	mt_csEnter;
	xrCriticalSection	mt_csLeave;
	volatile BOOL		mt_bMustExit;

	ICF		void			remove_from_seq_parallel	(const fastdelegate::FastDelegate0<> &delegate)
	{
		xr_vector<fastdelegate::FastDelegate0<> >::iterator I = std::find(
			seqParallel.begin(),
			seqParallel.end(),
			delegate
		);
		if (I != seqParallel.end())
			seqParallel.erase	(I);
	}
};

extern		ENGINE_API		CRenderDevice		Device;
extern		ENGINE_API		bool				g_bBenchmark;

typedef fastdelegate::FastDelegate0<bool>		LOADING_EVENT;
extern	ENGINE_API xr_list<LOADING_EVENT>		g_loading_events;

#include	"R_Backend_Runtime.h"

