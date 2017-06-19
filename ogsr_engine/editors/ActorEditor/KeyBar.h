//---------------------------------------------------------------------------
#ifndef KeyBarH
#define KeyBarH
#include "Gradient.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "ElTrackBar.hpp"
#include "ElXPThemedControl.hpp"
#include <StdCtrls.hpp>
#include "MXCtrls.hpp"
#include "multi_edit.hpp"
#include <ComCtrls.hpp>
#include "ExtBtn.hpp"
#include "ElBtnCtl.hpp"
#include "ElCheckCtl.hpp"
#include "ElPopBtn.hpp"
//---------------------------------------------------------------------------
//refs
class CSMotion;

class TfrmKeyBar : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel6;
	TPanel *Panel1;
	TLabel *Label1;
	TBevel *Bevel1;
	TMultiObjSpinEdit *seLOD;
	TMxLabel *lbCurrentTime;
	TPanel *Panel2;
	TPanel *Panel3;
	TStaticText *stStartTime;
	TPanel *Panel4;
	TGradient *Gradient1;
	TElTrackBar *anm_track;
	TElCheckBox *auto_ch;
	TPanel *Panel5;
	TStaticText *stEndTime;
	TMxPanel *PanelCh1;
	TLabel *Label2;
	TElGraphicButton *BtnUpCh1;
	TElGraphicButton *BtnDownCh1;
	TMxPanel *PanelCh2;
	TLabel *Label3;
	TElGraphicButton *BtnUpCh2;
	TElGraphicButton *BtnDownCh2;
	TElGraphicButton *BtnDelCh1;
	TElGraphicButton *BtnDelCh2;
	void __fastcall seLODLWChange(TObject *Sender, int Val);
	void __fastcall seLODKeyPress(TObject *Sender, char &Key);
	void __fastcall seLODExit(TObject *Sender);
	void __fastcall PanelCh1Paint(TObject *Sender);
	void __fastcall PanelCh2Paint(TObject *Sender);
	void __fastcall BtnUpCh1Click(TObject *Sender);
	void __fastcall BtnUpCh2Click(TObject *Sender);
	void __fastcall BtnDownCh2Click(TObject *Sender);
	void __fastcall BtnDownCh1Click(TObject *Sender);
	void __fastcall BtnDelCh1Click(TObject *Sender);
	void __fastcall BtnDelCh2Click(TObject *Sender);
private:	// User declarations
	CSMotion*	m_currentEditMotion;
	void 		draw_marks  (int id);
    void		set_mark	(int id, int action);
public:		// User declarations
    __fastcall TfrmKeyBar(TComponent* Owner);
    static TfrmKeyBar* CreateKeyBar(TWinControl* parent);
    void UpdateBar();
};
//---------------------------------------------------------------------------
#endif
