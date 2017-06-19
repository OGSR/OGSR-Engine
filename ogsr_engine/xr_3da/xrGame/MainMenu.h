#pragma once

class CUIWindow;
class CUIDialogWnd;
class CUICursor;
class CUIMessageBoxEx;
class CGameSpy_HTTP;
class CGameSpy_Full;
#include "../IInputReceiver.h"
#include "../IGame_Persistent.h"
#include "UIDialogHolder.h"
#include "ui/UIWndCallback.h"
#include "ui_base.h"

struct  Patch_Dawnload_Progress{
	bool		IsInProgress;
	float		Progress;
	shared_str	Status;
	shared_str	FileName;

	bool		GetInProgress	(){return IsInProgress;};
	float		GetProgress		(){return Progress;};
	LPCSTR		GetStatus		(){return Status.c_str();};
	LPCSTR		GetFlieName		(){return FileName.c_str();};
};

class CMainMenu :
	public IMainMenu,
	public IInputReceiver,
	public pureRender,
	public CDialogHolder,
	public CUIWndCallback,
	public CDeviceResetNotifier

{
	CUIDialogWnd*		m_startDialog;
	

	enum{
		flRestoreConsole	= (1<<0),
		flRestorePause		= (1<<1),
		flRestorePauseStr	= (1<<2),
		flActive			= (1<<3),
		flNeedChangeCapture	= (1<<4),
		flRestoreCursor		= (1<<5),
		flGameSaveScreenshot= (1<<6),
		flNeedVidRestart	= (1<<7),
	};
	Flags16			m_Flags;
	string_path		m_screenshot_name;
	u32				m_screenshotFrame;
	void						ReadTextureInfo		();


	xr_vector<CUIWindow*>		m_pp_draw_wnds;

	CGameSpy_Full*				m_pGameSpyFull;	

public:
	enum	EErrorDlg 
	{
		ErrInvalidPassword,
		ErrInvalidHost,
		ErrSessionFull,
		ErrServerReject,
		ErrCDKeyInUse,
		ErrCDKeyDisabled,
		ErrCDKeyInvalid,
		ErrDifferentVersion,
		ErrGSServiceFailed,
		ErrMasterServerConnectFailed,
		NoNewPatch,
		NewPatchFound,
		PatchDownloadError,
		PatchDownloadSuccess,
		ConnectToMasterServer,
		SessionTerminate,
		LoadingError,
		ErrMax,
		ErrNoError = ErrMax,
	};

	Patch_Dawnload_Progress		m_sPDProgress;
	Patch_Dawnload_Progress*	GetPatchProgress	() {return &m_sPDProgress;}
	void						CancelDownload		();
	CGameSpy_Full*				GetGS() const {return m_pGameSpyFull;};
protected:
	EErrorDlg		m_NeedErrDialog;	
	u32				m_start_time;
	
	shared_str		m_sPatchURL;
	shared_str		m_sPatchFileName;
	
	xr_vector<CUIMessageBoxEx*>	m_pMB_ErrDlgs;

//.	CUIMessageBoxEx*	m_pMSB_NoNewPatch;
//.	CUIMessageBoxEx*	m_pMSB_NewPatch;
//.	CUIMessageBoxEx*	m_pMSB_PatchDownloadError;
//.	CUIMessageBoxEx*	m_pMSB_PatchDownloadSuccess;
//.	CUIMessageBoxEx*	m_pMSB_ConnectToMasterServer;
public:
	u32				m_deactivated_frame;
	virtual void	DestroyInternal					(bool bForce);
					CMainMenu						();
	virtual			~CMainMenu						();

	virtual void	Activate						(bool bActive); 
	virtual	bool	IsActive						(); 

	virtual void	IR_OnMousePress					(int btn);
	virtual void	IR_OnMouseRelease				(int btn);
	virtual void	IR_OnMouseHold					(int btn);
	virtual void	IR_OnMouseMove					(int x, int y);
	virtual void	IR_OnMouseStop					(int x, int y);

	virtual void	IR_OnKeyboardPress				(int dik);
	virtual void	IR_OnKeyboardRelease			(int dik);
	virtual void	IR_OnKeyboardHold				(int dik);

	virtual void	IR_OnMouseWheel					(int direction)	;

	bool			OnRenderPPUI_query				();
	void			OnRenderPPUI_main				();
	void			OnRenderPPUI_PP					();

	virtual void	OnRender						();
	virtual void	OnFrame							(void);
	virtual void	StartStopMenu					(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	virtual bool	UseIndicators					()						{return false;}


	void			OnDeviceCreate					();

	void			Screenshot						(IRender_interface::ScreenshotMode mode=IRender_interface::SM_NORMAL, LPCSTR name = 0);
	void			RegisterPPDraw					(CUIWindow* w);
	void			UnregisterPPDraw				(CUIWindow* w);

	void			SetErrorDialog					(EErrorDlg ErrDlg);
	EErrorDlg		GetErrorDialogType				() const { return m_NeedErrDialog; } ;
	void			CheckForErrorDlg				();
	void			SwitchToMultiplayerMenu			();
	void			OnNewPatchFound					(LPCSTR VersionName, LPCSTR URL);
	void			OnNoNewPatchFound				();
	void xr_stdcall OnDownloadPatch					(CUIWindow*, void*);
	void xr_stdcall OnConnectToMasterServerOkClicked(CUIWindow*, void*);
	void			OnSessionTerminate				(LPCSTR reason);
	void			OnLoadError						(LPCSTR module);
	void			OnDownloadPatchError			();
	void			OnDownloadPatchSuccess			();
	void			OnDownloadPatchProgress			(u64 bytesReceived, u64 totalSize);
	void xr_stdcall OnRunDownloadedPatch			(CUIWindow*, void*);
	void			Show_CTMS_Dialog				();
	void			Hide_CTMS_Dialog				();
	void			SetNeedVidRestart				();
	virtual void	OnDeviceReset					();
	LPCSTR			GetGSVer						();
		bool		ValidateCDKey					();
		bool		IsCDKeyIsValid();
};

extern CMainMenu*	MainMenu();