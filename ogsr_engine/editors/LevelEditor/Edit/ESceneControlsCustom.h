#ifndef ESceneControlsCustomH
#define ESceneControlsCustomH

#include "customobject.h"
//---------------------------------------------------------------------------
// refs
class ESceneCustomMTools;

struct SBeforeAppendCallbackParams{
	LPVOID 		data;
    xr_string 	name_prefix;
    SBeforeAppendCallbackParams()
    {
    	data 		= 0;
        name_prefix	= "";
    }
};
typedef bool __fastcall (__closure *TBeforeAppendCallback)(SBeforeAppendCallbackParams* p);
typedef bool __fastcall (__closure *TAfterAppendCallback)(TShiftState _Shift, CCustomObject* obj);

class TUI_CustomControl{
protected:
    friend class ESceneCustomMTools;
    int sub_target;
    int action;
// select
    bool bBoxSelection;
	bool __fastcall SelectStart	(TShiftState _Shift);
	void __fastcall SelectProcess	(TShiftState _Shift);
    bool __fastcall SelectEnd		(TShiftState _Shift);
// add
    bool __fastcall AddStart		(TShiftState _Shift);
	void __fastcall AddProcess		(TShiftState _Shift);
    bool __fastcall AddEnd			(TShiftState _Shift);
// move
	Fvector m_MovingXVector;
	Fvector m_MovingYVector;
	Fvector m_MovingReminder;
	bool   __fastcall				MovingStart		(TShiftState _Shift);
	void   __fastcall				MovingProcess	(TShiftState _Shift);
    bool   __fastcall				MovingEnd		(TShiftState _Shift);
// scale
	bool   __fastcall				ScaleStart		(TShiftState _Shift);
	void   __fastcall				ScaleProcess	(TShiftState _Shift);
    bool   __fastcall				ScaleEnd		(TShiftState _Shift);
// rotate
	Fvector m_RotateVector;
    float m_fRotateSnapAngle;
	bool 	  __fastcall			RotateStart		(TShiftState _Shift);
	void 	  __fastcall			RotateProcess	(TShiftState _Shift);
	bool 	  __fastcall			RotateEnd		(TShiftState _Shift);
protected:
	bool				CheckSnapList 	(TShiftState Shift);

	CCustomObject*	 __fastcall	DefaultAddObject(TShiftState Shift, TBeforeAppendCallback before=0, TAfterAppendCallback after=0);
    bool			 __fastcall	DefaultMovingProcess(TShiftState Shift, Fvector& amount);
public:
    ESceneCustomMTools*	parent_tool;
public:
    				TUI_CustomControl	(int st, int act, ESceneCustomMTools* parent);
    virtual 		~TUI_CustomControl	(){;}
	virtual bool 	Start  				(TShiftState _Shift);
	virtual bool 	End    				(TShiftState _Shift);
	virtual void 	Move   				(TShiftState _Shift);
	virtual bool 	HiddenMode			();
    virtual bool 	KeyDown				(WORD Key, TShiftState Shift){return false;}
    virtual bool 	KeyUp  				(WORD Key, TShiftState Shift){return false;}
    virtual bool 	KeyPress			(WORD Key, TShiftState Shift){return false;}
    virtual void 	OnEnter				(){;}
    virtual void 	OnExit				(){;}

    int 			Action				(){return action;}
};
//---------------------------------------------------------------------------
#endif
