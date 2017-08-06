#pragma once

#include "UIListItem.h"


struct LIST_SRV_SIZES {
	float icon;
	float server;
	float map;
	float game; 
	float players;
	float ping;
	float version;
};

struct SRV_ICONS{
	bool pass;
	bool dedicated;
	bool punkbuster;
	bool user_pass;
};

struct LIST_SRV_INFO{
	shared_str server;
	shared_str address;
	shared_str map;
	shared_str game;
	shared_str players;
	shared_str ping;
	shared_str version;
	SRV_ICONS  icons;
	int			Index;
};

struct LIST_SRV_ITEM {
	u32				color;
	CGameFont*		font;
	LIST_SRV_SIZES	size;
	LIST_SRV_INFO	info;
};

class CUIListItemServer : public CUIListItem 
{
public:
	CUIListItemServer();


	// CUISimpleWindow
	using CUIWindow::Init;
	virtual void Draw();

	// own
	virtual void			Init					(LIST_SRV_ITEM& params, float x, float y, float width, float height);
			void			SetParams				(LIST_SRV_ITEM& params);
			void			CreateConsoleCommand	(xr_string& command, LPCSTR player_name, LPCSTR player_pass, LPCSTR server_psw);

			int				Get_gs_index			()		{return m_srv_info.info.Index;}
			LIST_SRV_ITEM*	GetInfo					()		{return &m_srv_info;};
	

protected:
			void			SetTextColor			(u32 color);
			void			SetFont					(CGameFont* pFont);
	LIST_SRV_SIZES			m_sizes;

	LIST_SRV_ITEM			m_srv_info;

	CUIWindow 				m_icon;
	CUIStatic 				m_iconPass;
	CUIStatic 				m_iconDedicated;
	CUIStatic 				m_iconPunkBuster;
	CUIStatic 				m_iconUserPass;
	CUIStatic 				m_server;
	CUIStatic 				m_map;
	CUIStatic 				m_game;
	CUIStatic 				m_players;
	CUIStatic 				m_ping;
	CUIStatic 				m_version;
};