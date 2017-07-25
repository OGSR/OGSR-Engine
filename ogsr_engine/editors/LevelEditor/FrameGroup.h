//---------------------------------------------------------------------------
#ifndef FrameGroupH
#define FrameGroupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "ExtBtn.hpp"
#include "ESceneCustomMTools.h"
#include "MXCtrls.hpp"
#include "multi_edit.hpp"
// refs
class ESceneGroupTools;
//---------------------------------------------------------------------------
class TfraGroup : public TForm
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebUngroup;
	TExtBtn *ebGroup;
	TPanel *paCurrentObject;
	TLabel *APHeadLabel2;
	TPanel *Panel2;
	TLabel *Label1;
	TExtBtn *ebCenterToGroup;
	TExtBtn *ebAlignToObject;
	TExtBtn *ebSaveSelected;
	TExtBtn *ebSelect;
	TMxLabel *MxLabel1;
	TMxLabel *lbCurrent;
	TPanel *paSelectObject;
	TBevel *Bevel3;
	TLabel *Label2;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn3;
	TLabel *APHeadLabel3;
	TExtBtn *ExtBtn5;
	TExtBtn *ExtBtn6;
	TLabel *Label4;
	TExtBtn *ExtBtn7;
	TLabel *Label6;
	TMultiObjSpinEdit *seSelPercent;
	TExtBtn *ebOpen;
	TExtBtn *ebClose;
	TExtBtn *ebReloadRefs;
	TExtBtn *ebMakeThumbnail;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebMakeGroupClick(TObject *Sender);
	void __fastcall ebUngroupClick(TObject *Sender);
	void __fastcall ebCenterToGroupClick(TObject *Sender);
	void __fastcall ebAlignToObjectClick(TObject *Sender);
	void __fastcall ebSelectClick(TObject *Sender);
	void __fastcall ebSaveSelectedClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
	void __fastcall ExtBtn6Click(TObject *Sender);
	void __fastcall ExtBtn7Click(TObject *Sender);
	void __fastcall seSelPercentKeyPress(TObject *Sender, char &Key);
	void __fastcall ebOpenClick(TObject *Sender);
	void __fastcall ebCloseClick(TObject *Sender);
	void __fastcall ebReloadRefsClick(TObject *Sender);
	void __fastcall ebMakeThumbnailClick(TObject *Sender);
private:	// User declarations
	ESceneGroupTools* ParentTools;
	void 			SelByRefObject		( bool flag );
	void 			MultiSelByRefObject ( bool clear_prev );
public:		// User declarations
	__fastcall TfraGroup(TComponent* Owner, ESceneGroupTools* gt);
};
//---------------------------------------------------------------------------
#endif
