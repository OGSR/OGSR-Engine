#pragma once

#include "script_export_space.h"
#include "ui\xrUIXmlParser.h"

class CUIWindow;
class CUIFrameWindow;
class CUIStatic;
class CUICheckButton;
class CUISpinNum;
class CUISpinText;
class CUISpinFlt;
class CUIComboBox;
class CUIButton;
class CUI3tButton;
class CUICheckButton;
class CUIListWnd;
class CUITabControl;
class CUIFrameLineWnd;
class CUILabel;
class CUIEditBox;
class CUITextBanner;
class CUIMultiTextStatic;
class CUIAnimatedStatic;
class CUIArtefactPanel;
class CServerList;
class CUIMapList;
class CUITrackBar;
class CUIMapInfo;
class CUIMMShniaga;
class CUIScrollView;
class CUIProgressBar;

class CScriptXmlInit {
public:
	DECLARE_SCRIPT_REGISTER_FUNCTION

	CScriptXmlInit();
	CScriptXmlInit(const CScriptXmlInit& other);
	CScriptXmlInit& operator= (const CScriptXmlInit& other);

	void ParseFile		(LPCSTR xml_file);
	void ParseShTexInfo	(LPCSTR xml_file);
	void InitWindow		(LPCSTR path, int index, CUIWindow* pWnd);
	//void InitList		(LPCSTR path, int index, CUIListWnd* pWnd);

	CUIListWnd*			InitList(LPCSTR path, CUIWindow* parent);
	CUIFrameWindow*		InitFrame(LPCSTR path, CUIWindow* parent);
	CUIFrameLineWnd*	InitFrameLine(LPCSTR path, CUIWindow* parent);
	CUILabel*			InitLabel(LPCSTR path, CUIWindow* parent);
	CUIEditBox*			InitEditBox(LPCSTR path, CUIWindow* parent);
	CUIStatic*			InitStatic(LPCSTR path, CUIWindow* parent);
	CUIStatic*			InitAnimStatic(LPCSTR path, CUIWindow* parent);
	CUICheckButton*		InitCheck(LPCSTR path, CUIWindow* parent);
	CUISpinNum*			InitSpinNum(LPCSTR path, CUIWindow* parent);
	CUISpinFlt*			InitSpinFlt(LPCSTR path, CUIWindow* parent);
	CUISpinText*		InitSpinText(LPCSTR path, CUIWindow* parent);
	CUIComboBox*		InitComboBox(LPCSTR path, CUIWindow* parent);
	CUIButton*			InitButton(LPCSTR path, CUIWindow* parent);
	CUI3tButton*		Init3tButton(LPCSTR path, CUIWindow* parent);
	CUITabControl*		InitTab(LPCSTR path, CUIWindow* parent);
	CServerList*		InitServerList(LPCSTR path, CUIWindow* parent);
	CUIMapList*			InitMapList(LPCSTR path, CUIWindow* parent);
	CUIMapInfo*			InitMapInfo(LPCSTR path, CUIWindow* parent);
	CUITrackBar*		InitTrackBar(LPCSTR path, CUIWindow* parent);
	CUIEditBox*			InitCDkey(LPCSTR path, CUIWindow* parent);
	CUIMMShniaga*		InitMMShniaga(LPCSTR path, CUIWindow* parent);
	CUIWindow*			InitKeyBinding(LPCSTR path, CUIWindow* parent);
	CUIScrollView*		InitScrollView(LPCSTR path, CUIWindow* parent);
	CUIProgressBar*		InitProgressBar(LPCSTR path, CUIWindow* parent);
	void				InitAutoStaticGroup(LPCSTR path, CUIWindow* pWnd);
protected:
	CUIXml	m_xml;
};

add_to_type_list(CScriptXmlInit)
#undef script_type_list
#define script_type_list save_type_list(CScriptXmlInit)