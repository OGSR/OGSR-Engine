#pragma once

#include "UIWindow.h"
#include "../game_base_space.h"

class CUIListBox;
class CUILabel;
class CUIStatic;
class CUIFrameWindow;
class CUI3tButton;
class CUISpinText;
class CUIMapInfo;
class CUIComboBox;
class CUIXml;
class CExtraContentFilter;

#define	MAP_ROTATION_LIST	"maprot_list.ltx"

class CUIMapList : public CUIWindow {
public:
					CUIMapList();
	virtual			~CUIMapList();
	virtual void	Init(float x, float y, float width, float height);
	virtual void	Update();
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData  = NULL);
			void	InitFromXml(CUIXml& xml_doc, const char* path);	

			void	SetWeatherSelector(CUIComboBox* ws);
			void	SetModeSelector(CUISpinText* ms);
			void	SetMapPic(CUIStatic* map_pic);
			void	SetMapInfo(CUIMapInfo* map_info);
			void	SetServerParams(LPCSTR params);
			void	OnModeChange();
			void	OnListItemClicked();
			void	LoadMapList();
			void	SaveMapList();
	const char*		GetCommandLine(LPCSTR player_name);
		EGameTypes	GetCurGameType();
			void	StartDedicatedServer();
			bool	IsEmpty();
	const shared_str& GetMapNameInt(EGameTypes _type, u32 idx);

private:
	const char*		GetCLGameModeName(); // CL - command line
			void	UpdateMapList(EGameTypes GameType);						
			void	SaveRightList();

			void	OnBtnLeftClick();
			void	OnBtnRightClick();
			void	OnBtnUpClick();
			void	OnBtnDownClick();
			void	AddWeather(const shared_str& WeatherType, const shared_str& WeatherTime, u32 _id);
			void	ParseWeather(char** ps, char* e);

	CUIListBox*			m_pList1;
	CUIListBox*			m_pList2;
	CUIFrameWindow*		m_pFrame1;
	CUIFrameWindow*		m_pFrame2;
	CUILabel*			m_pLbl1;
	CUILabel*			m_pLbl2;
	CUI3tButton*		m_pBtnLeft;
	CUI3tButton*		m_pBtnRight;
	CUI3tButton*		m_pBtnUp;
	CUI3tButton*		m_pBtnDown;

	CUIComboBox*		m_pWeatherSelector;
	CUISpinText*		m_pModeSelector;
	CUIStatic*			m_pMapPic;
	CUIMapInfo*			m_pMapInfo;

	struct Sw{
		shared_str	weather_name;
		int			weather_time;
	};
	xr_vector<Sw>		m_mapWeather;
	xr_string			m_command;
	xr_string			m_srv_params;

	int					m_item2del;

	CExtraContentFilter*	m_pExtraContentFilter;
};