#pragma once

#include "game_base.h"
#include "../xr_3da/NET_Server_Trash/client_id.h"
#include "WeaponAmmo.h"
//#include "Level_Bullet_Manager.h"

class	NET_Packet;
class	CGameObject;
class	CUIGameCustom;
class	CUI;
class	CUIDialogWnd;

struct WeaponUsageStatistic;

class	game_cl_GameState	: public game_GameState, public ISheduled
{
	typedef game_GameState	inherited;
	shared_str							m_game_type_name;
//	bool								m_bCrosshair;	//был ли показан прицел-курсор HUD перед вызовом меню
protected:
	u16									m_u16VotingEnabled;	
	bool								m_bServerControlHits;	

public:
	typedef xr_map<ClientID,game_PlayerState*>	PLAYERS_MAP;
	typedef PLAYERS_MAP::iterator				PLAYERS_MAP_IT;
	typedef PLAYERS_MAP::const_iterator			PLAYERS_MAP_CIT;

	PLAYERS_MAP							players;
	ClientID							local_svdpnid;
	game_PlayerState*					local_player;
	bool								m_need_to_update;
//.	xr_vector<CGameObject*>				targets;


	WeaponUsageStatistic				*m_WeaponUsageStatistic;	
	virtual		void				reset_ui				();
	virtual		void				CommonMessageOut		(LPCSTR msg);

private:
				void				switch_Phase			(u32 new_phase)		{inherited::switch_Phase(new_phase);};
protected:

	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	

	//for scripting enhancement
	virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);

	virtual		shared_str			shedule_Name			() const		{ return shared_str("game_cl_GameState"); };
	virtual		float				shedule_Scale			();
	virtual		bool				shedule_Needed			()				{return true;};

				void				sv_GameEventGen			(NET_Packet& P);
				void				sv_EventSend			(NET_Packet& P);
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();
				LPCSTR				type_name				() const {return *m_game_type_name;};
				void				set_type_name			(LPCSTR s);
	virtual		void				Init					(){};
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_import_GameTime		(NET_Packet& P);						// update GameTime only for remote clients
	virtual		void				net_signal				(NET_Packet& P);

				bool				IR_OnKeyboardPress		(int dik);
				bool				IR_OnKeyboardRelease	(int dik);
				bool				IR_OnMouseMove			(int dx, int dy);
				bool				IR_OnMouseWheel			(int direction);


	virtual		bool				OnKeyboardPress			(int key){return false;};
	virtual		bool				OnKeyboardRelease		(int key){return false;};
				void				OnGameMessage			(NET_Packet& P);

				game_PlayerState*	GetPlayerByGameID		(u32 GameID);
				game_PlayerState*	GetPlayerByOrderID		(u32 id);
				ClientID			GetClientIDByOrderID	(u32 id);
				u32					GetPlayersCount			() const {return players.size();};
	virtual		CUIGameCustom*		createGameUI			(){return NULL;};

				void				StartStopMenu			(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	virtual		void				shedule_Update			(u32 dt);

	void							u_EventGen				(NET_Packet& P, u16 type, u16 dest);
	void							u_EventSend				(NET_Packet& P);

	virtual		void				ChatSayTeam				(const shared_str &phrase)	{};
	virtual		void				ChatSayAll				(const shared_str &phrase)	{};
	virtual		void				OnChatMessage			(NET_Packet* P)	{};
	virtual		void				OnWarnMessage			(NET_Packet* P)	{};
	virtual		void				OnRadminMessage			(u16 type, NET_Packet* P)	{};
	
	virtual		void				OnRender				()	{};
	virtual		bool				IsServerControlHits		()	{return m_bServerControlHits;};
	virtual		bool				IsEnemy					(game_PlayerState* ps)	{return false;};
	virtual		bool				IsEnemy					(CEntityAlive* ea1, CEntityAlive* ea2)	{return false;};

	virtual		void				OnSpawn					(CObject* pObj)	{};
	virtual		void				OnDestroy				(CObject* pObj)	{};

	virtual		void				SendPickUpEvent			(u16 ID_who, u16 ID_what);
};
