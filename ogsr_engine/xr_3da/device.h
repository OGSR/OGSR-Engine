#pragma once

#include <atomic>
#include "Event.hpp"

#include "pure.h"
#include "../xrcore/ftimer.h"
#include "stats.h"

extern u32 g_dwFPSlimit;

#define VIEWPORT_NEAR 0.2f
#define HUD_VIEWPORT_NEAR 0.05f

#define DEVICE_RESET_PRECACHE_FRAME_COUNT 10

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/RenderDeviceRender.h"

#ifdef INGAME_EDITOR
#	include "../Include/editor/interfaces.hpp"
#endif // #ifdef INGAME_EDITOR

class engine_impl;

#pragma pack(push,4)

class IRenderDevice
{
public:
	virtual		CStatsPhysics*	_BCL		StatPhysics		()							= 0;								
	virtual				void	_BCL		AddSeqFrame		( pureFrame* f, bool mt )	= 0;
	virtual				void	_BCL		RemoveSeqFrame	( pureFrame* f )			= 0;
};

class ENGINE_API CRenderDeviceData
{
public:
	class ENGINE_API CSecondVPParams //--#SM+#-- +SecondVP+
	{
	public:
		bool m_bCamReady; // Флаг готовности камеры (FOV, позиция, и т.п) к рендеру второго вьюпорта
	private:
		bool m_bIsActive;  // Флаг активации рендера во второй вьюпорт
	public:
		IC bool IsSVPActive() { return m_bIsActive; }
		void    SetSVPActive(bool bState);
		bool    IsSVPFrame();
	};

public:
	u32										dwWidth;
	u32										dwHeight;
	
	u32										dwPrecacheFrame;
	BOOL									b_is_Ready;
	BOOL									b_is_Active;
public:

		// Engine flow-control
	u32										dwFrame;

	float									fTimeDelta;
	float									fTimeGlobal;
	u32										dwTimeDelta;
	u32										dwTimeGlobal;
	u32										dwTimeContinual;

	Fvector									vCameraPosition;
	Fvector									vCameraDirection;
	Fvector									vCameraTop;
	Fvector									vCameraRight;

	Fmatrix									mView;
	Fmatrix									mProject;
	Fmatrix									mFullTransform;

	// Copies of corresponding members. Used for synchronization.
	Fvector									vCameraPosition_saved;

	Fmatrix									mView_saved;
	Fmatrix									mProject_saved;
	Fmatrix									mFullTransform_saved;

	float									fFOV;
	float									fASPECT;
protected:

	u32										Timer_MM_Delta;
	CTimer_paused							Timer;
	CTimer_paused							TimerGlobal;
public:

// Registrators
	CRegistrator	<pureRender			>			seqRender;
	CRegistrator	<pureAppActivate	>			seqAppActivate;
	CRegistrator	<pureAppDeactivate	>			seqAppDeactivate;
	CRegistrator	<pureAppStart		>			seqAppStart;
	CRegistrator	<pureAppEnd			>			seqAppEnd;
	CRegistrator	<pureFrame			>			seqFrame;
	CRegistrator	<pureScreenResolutionChanged>	seqResolutionChanged;

	HWND									m_hWnd;
//	CStats*									Statistic;

};

class	ENGINE_API CRenderDeviceBase :
	public IRenderDevice,
	public CRenderDeviceData
{
public:
};

#pragma pack(pop)
// refs
class ENGINE_API CRenderDevice: public CRenderDeviceBase
{
private:
    // Main objects used for creating and rendering the 3D scene
    u32										m_dwWindowStyle;
    RECT									m_rcWindowBounds;
    RECT									m_rcWindowClient;

	//u32										Timer_MM_Delta;
	//CTimer_paused							Timer;
	//CTimer_paused							TimerGlobal;
	CTimer									TimerMM;

	void									_Create		(LPCSTR shName);
	void									_Destroy	(BOOL	bKeepTextures);
	void									_SetupStates();
public:
 //   HWND									m_hWnd;
	LRESULT									MsgProc		(HWND,UINT,WPARAM,LPARAM);

//	u32										dwFrame;
//	u32										dwPrecacheFrame;
	u32										dwPrecacheTotal;

//	u32										dwWidth, dwHeight;
	float									fWidth_2, fHeight_2;
//	BOOL									b_is_Ready;
//	BOOL									b_is_Active;
	void									OnWM_Activate(WPARAM wParam, LPARAM lParam);
public:
	//ref_shader								m_WireShader;
	//ref_shader								m_SelectionShader;

	IRenderDeviceRender						*m_pRender;

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
		m_pRender->SetCacheXform(mView, mProject);
		//R_ASSERT(0);
		//	TODO: re-implement set projection
		//RCache.set_xform_project			(mProject);
	}

	void									DumpResourcesMemoryUsage() { m_pRender->ResourcesDumpMemoryUsage();}
public:
	// Registrators
	//CRegistrator	<pureRender			>			seqRender;
//	CRegistrator	<pureAppActivate	>			seqAppActivate;
//	CRegistrator	<pureAppDeactivate	>			seqAppDeactivate;
//	CRegistrator	<pureAppStart		>			seqAppStart;
//	CRegistrator	<pureAppEnd			>			seqAppEnd;
	//CRegistrator	<pureFrame			>			seqFrame;
	CRegistrator	<pureFrame			>			seqFrameMT;
	CRegistrator	<pureDeviceReset	>			seqDeviceReset;
	xr_vector<fastdelegate::FastDelegate<void()>> seqParallel;

	CSecondVPParams m_SecondViewport; //--#SM+#-- +SecondVP+

	// Dependent classes
	//CResourceManager*						Resources;

	CStats*									Statistic;

	// Engine flow-control
	//float									fTimeDelta;
	//float									fTimeGlobal;
	//u32										dwTimeDelta;
	//u32										dwTimeGlobal;
	//u32										dwTimeContinual;

	// Cameras & projection
	//Fvector									vCameraPosition;
	//Fvector									vCameraDirection;
	//Fvector									vCameraTop;
	//Fvector									vCameraRight;

	//Fmatrix									mView;
	//Fmatrix									mProject;
	//Fmatrix									mFullTransform;

	Fmatrix									mInvFullTransform;

	//float									fFOV;
	//float									fASPECT;
	
	CRenderDevice			()
		:
		m_pRender(0)
#ifdef INGAME_EDITOR
		,m_editor_module(0),
		m_editor_initialize(0),
		m_editor_finalize(0),
		m_editor(0),
		m_engine(0)
#endif // #ifdef INGAME_EDITOR
	{
	    m_hWnd              = NULL;
		b_is_Active			= FALSE;
		b_is_Ready			= FALSE;
		Timer.Start			();
		m_bNearer			= FALSE;

		//--#SM+#-- +SecondVP+
		m_SecondViewport.SetSVPActive(false);
		m_SecondViewport.m_bCamReady = false;
	};

	void	Pause							(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason);
	BOOL	Paused							();

	// Scene control
	void PreCache							(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input);
	BOOL Begin								();
	void Clear								();
	void End								();
	void FrameMove							();
	
	void overdrawBegin						();
	void overdrawEnd						();

	// Mode control
	void DumpFlags							();

#pragma warning(push)
#pragma warning(disable:4366)
	IC CTimer_paused* GetTimerGlobal() { return &TimerGlobal; }
#pragma warning(pop)

	u32	 TimerAsync							()	{ return TimerGlobal.GetElapsed_ms();				}
	u32	 TimerAsync_MMT						()	{ return TimerMM.GetElapsed_ms() +	Timer_MM_Delta; }

	// Creation & Destroying
	void ConnectToRender();
	void Create								(void);
	void Run								(void);
	void Destroy							(void);
	void Reset								(bool precache = true);

	void Initialize							(void);
	void ShutDown							(void);

	void time_factor(const float& time_factor); //--#SM+#--
	inline const float time_factor() const {
		VERIFY(Timer.time_factor() == TimerGlobal.time_factor());
		return (Timer.time_factor());
	}

private:
	// Multi-threading
	Event syncProcessFrame, syncFrameDone, syncThreadExit; // Secondary thread events
	std::atomic_bool mt_bMustExit;
	std::chrono::duration<double, std::milli> SecondThreadTasksElapsedTime;

public:
	ICF void remove_from_seq_parallel(const fastdelegate::FastDelegate<void()> &delegate)
	{
		seqParallel.erase( std::remove( seqParallel.begin(), seqParallel.end(), delegate ), seqParallel.end() );
	}

public:
			void on_idle				();
			bool on_message			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result);

private:
			void					message_loop		();
virtual		void			_BCL	AddSeqFrame			( pureFrame* f, bool mt );
virtual		void			_BCL	RemoveSeqFrame		( pureFrame* f );
virtual		CStatsPhysics*	_BCL	StatPhysics			()	{ return  Statistic ;}
#ifdef INGAME_EDITOR
public:
	IC		editor::ide			*editor				() const { return m_editor; }

private:
			void				initialize_editor	();
			void				message_loop_editor	();

private:
	typedef editor::initialize_function_ptr			initialize_function_ptr;
	typedef editor::finalize_function_ptr			finalize_function_ptr;

private:
	HMODULE						m_editor_module;
	initialize_function_ptr		m_editor_initialize;
	finalize_function_ptr		m_editor_finalize;
	editor::ide					*m_editor;
	engine_impl					*m_engine;
#endif // #ifdef INGAME_EDITOR
};

extern		ENGINE_API		CRenderDevice		Device;

#ifndef	_EDITOR
#define	RDEVICE	Device
#else
#define RDEVICE	EDevice
#endif


extern		ENGINE_API		bool				g_bBenchmark;

extern ENGINE_API xr_list<fastdelegate::FastDelegate<bool()>> g_loading_events;

class ENGINE_API CLoadScreenRenderer :public pureRender
{
public:
					CLoadScreenRenderer	();
	void			start				(bool b_user_input);
	void			stop				();
	virtual void	OnRender			();

	bool			b_registered;
	bool			b_need_user_input{};
};
extern ENGINE_API CLoadScreenRenderer load_screen_renderer;
