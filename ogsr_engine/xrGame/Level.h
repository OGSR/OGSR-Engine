// Level.h: interface for the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\xr_3da\igame_level.h"
#include "../xr_3da/NET_Server_Trash/net_client.h"
#include "script_export_space.h"
#include "..\xr_3da\StatGraph.h"
#include "xrMessages.h"
#include "alife_space.h"
#include "xrDebug.h"
#include "xrServer.h"

class	CHUDManager;
class	CParticlesObject;
class	xrServer;
class	game_cl_GameState;
class	NET_Queue_Event;
class	CSE_Abstract;
class	CSpaceRestrictionManager;
class	CSeniorityHierarchyHolder;
class	CClientSpawnManager;
class	CGameObject;
class	CPHCommander;
class	CLevelDebug;
class	CLevelSoundManager;

class CDebugRenderer;

extern float g_fov;

const int maxRP					= 64;
const int maxTeams				= 32;

//class CFogOfWar;
class CFogOfWarMngr;
class CBulletManager;
class CMapManager;

#include "..\xr_3da\feel_touch.h"

class GlobalFeelTouch : public Feel::Touch
{
public:
							GlobalFeelTouch();
	virtual					~GlobalFeelTouch();

			void			update						();
			bool			is_object_denied			(CObject const * O);
};


class CLevel					: public IGame_Level, public IPureClient
{
	#include "Level_network_Demo.h"
	void						ClearAllObjects			();
private:
#ifdef DEBUG
	bool						m_bSynchronization;
#endif
protected:
	typedef IGame_Level			inherited;
	
	CLevelSoundManager			*m_level_sound_manager;

	// movement restriction manager
	CSpaceRestrictionManager	*m_space_restriction_manager;
	// seniority hierarchy holder
	CSeniorityHierarchyHolder	*m_seniority_hierarchy_holder;
	// client spawn_manager
	CClientSpawnManager			*m_client_spawn_manager;
	// debug renderer
	CDebugRenderer				*m_debug_renderer;

	CPHCommander				*m_ph_commander;
	CPHCommander				*m_ph_commander_scripts;
	
	// level name
	shared_str					m_name;
	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;
	//---------------------------------------------
	CStatGraph					*pStatGraphS;
	u32							m_dwSPC;	//SendedPacketsCount
	u32							m_dwSPS;	//SendedPacketsSize
	CStatGraph					*pStatGraphR;
	u32							m_dwRPC;	//ReceivedPacketsCount
	u32							m_dwRPS;	//ReceivedPacketsSize
	//---------------------------------------------
	
public:
#ifdef DEBUG
	// level debugger
	CLevelDebug					*m_level_debug;
#endif

public:
	////////////// network ////////////////////////
	constexpr u32 GetInterpolationSteps() { return 0; }
	static void 				PhisStepsCallback		( u32 Time0, u32 Time1 );

	virtual void				OnMessage				(void* data, u32 size);
	virtual void				OnConnectRejected		();
private:
	CObject*					pCurrentControlEntity;
	xrServer::EConnect			m_connect_server_err;
public:
	CObject*					CurrentControlEntity	( void ) const		{ return pCurrentControlEntity; }
	void						SetControlEntity		( CObject* O  )		{ pCurrentControlEntity=O; }
private:

	BOOL						Connect2Server					(LPCSTR options);
private:
	bool						m_bConnectResultReceived;
	bool						m_bConnectResult;
	xr_string					m_sConnectResult;
public:	
	void						OnGameSpyChallenge(NET_Packet* P) //KRodin: удалить, если не вызывается!
	{
		Msg("!!Called OnGameSpyChallenge!");
	}
	void						OnBuildVersionChallenge			();
	void						OnConnectResult					(NET_Packet* P);
public:
	//////////////////////////////////////////////	
	// static particles
	DEFINE_VECTOR				(CParticlesObject*,POVec,POIt);
	POVec						m_StaticParticles;

	game_cl_GameState			*game;
	BOOL						m_bGameConfigStarted;
	BOOL						game_configured;
	NET_Queue_Event				*game_events;
	xr_deque<CSE_Abstract*>		game_spawn_queue;
	xrServer*					Server;
	GlobalFeelTouch				m_feel_deny;
	std::vector<u16> m_just_destroyed;

private:
	// preload sounds registry
	DEFINE_MAP					(shared_str,ref_sound,SoundRegistryMap,SoundRegistryMapIt);
	SoundRegistryMap			sound_registry;

public:
	void						PrefetchSound (LPCSTR name);

protected:
	BOOL						net_start_result_total;
	BOOL						connected_to_server;

	bool				net_start1				();
	bool				net_start2				();
	bool				net_start3				();
	bool				net_start4				();
	bool				net_start5				();
	bool				net_start6				();

	bool				net_start_client1				();
	bool				net_start_client2				();
	bool				net_start_client3				();
	bool				net_start_client4				();
	bool				net_start_client5				();
	bool				net_start_client6				();

	bool				net_start_finalizer				();

	void						net_OnChangeSelfName			(NET_Packet* P);

public:
	// sounds
	xr_vector<ref_sound*>		static_Sounds;

	// startup options
	shared_str					m_caServerOptions;
	shared_str					m_caClientOptions;

	// Starting/Loading
	virtual BOOL				net_Start				( LPCSTR op_server, LPCSTR op_client);
	virtual void				net_Load				( LPCSTR name );
	virtual void				net_Save				( LPCSTR name );
	virtual void				net_Stop				( );
	virtual BOOL				net_Start_client		( LPCSTR name );
	virtual void				net_Update				( );


	virtual BOOL				Load_GameSpecific_Before( );
	virtual BOOL				Load_GameSpecific_After ( );
	virtual void				Load_GameSpecific_CFORM	( CDB::TRI* T, u32 count );

	// Events
	virtual void				OnEvent					( EVENT E, u64 P1, u64 P2 );
	virtual void				OnFrame					( void );
	virtual void				OnRender				( );
	void						cl_Process_Event		(u16 dest, u16 type, NET_Packet& P);
	void						cl_Process_Spawn		(NET_Packet& P);
	void						ProcessGameEvents		( );
	void						ProcessGameSpawns		( );

	// Input
	virtual	void				IR_OnKeyboardPress		( int btn );
	virtual void				IR_OnKeyboardRelease	( int btn );
	virtual void				IR_OnKeyboardHold		( int btn );
	virtual void				IR_OnMousePress			( int btn );
	virtual void				IR_OnMouseRelease		( int btn );
	virtual void				IR_OnMouseHold			( int btn );
	virtual void				IR_OnMouseMove			( int, int);
	virtual void				IR_OnMouseStop			( int, int);
	virtual void				IR_OnMouseWheel			( int direction);
	virtual void				IR_OnActivate			(void);
	
			int					get_RPID				(LPCSTR name);


	// Game
	void						InitializeClientGame	(NET_Packet& P);
	void						ClientReceive			();
	void						ClientSend				();
	void						ClientSave				();
			u32					Objects_net_Save		(NET_Packet* _Packet, u32 start, u32 count);
	virtual	void				Send					(NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	
	void						g_cl_Spawn				(LPCSTR name, u8 rp, u16 flags, Fvector pos);	// only ask server
	void						g_sv_Spawn				(CSE_Abstract* E);					// server reply/command spawning
	
	// Save/Load/State
	void						SLS_Default				();					// Default/Editor Load
	
	IC CSpaceRestrictionManager		&space_restriction_manager	();
	IC CSeniorityHierarchyHolder	&seniority_holder			();
	IC CClientSpawnManager			&client_spawn_manager		();
	IC CDebugRenderer				&debug_renderer				();

	IC CPHCommander					&ph_commander				();
	IC CPHCommander					&ph_commander_scripts		();

	// C/D
	CLevel();
	virtual ~CLevel();

	//названияе текущего уровня
	virtual shared_str			name				() const;
	virtual void				GetLevelInfo		( CServerInfo* si );

	//gets the time from the game simulation
	
	//возвращает время в милисекундах относительно начала игры
	ALife::_TIME_ID		GetGameTime				();
	//возвращает время для энвайронмента в милисекундах относительно начала игры
	ALife::_TIME_ID		GetEnvironmentGameTime	();
	//игровое время в отформатированном виде
	void				GetGameDateTime			(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs);

	float				GetGameTimeFactor		();
	void				SetGameTimeFactor		(const float fTimeFactor);
	void				SetGameTimeFactor		(ALife::_TIME_ID GameTime, const float fTimeFactor);
	virtual void		SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor);

	void GetGameTimeForShaders(u32& hours, u32& minutes, u32& seconds, u32& milliseconds) override;

	// gets current daytime [0..23]
	u8					GetDayTime				();
	u32					GetGameDayTimeMS		();
	float				GetGameDayTimeSec		();
	float				GetEnvironmentGameDayTimeSec();

protected:
//	CFogOfWarMngr*		m_pFogOfWarMngr;
protected:	
	CMapManager *			m_map_manager;
public:
	CMapManager&			MapManager					()	{return *m_map_manager;}
//	CFogOfWarMngr&			FogOfWarMngr				()	{return *m_pFogOfWarMngr;}

	//работа с пулями
protected:	
	CBulletManager*		m_pBulletManager;
public:
	IC CBulletManager&	BulletManager() {return	*m_pBulletManager;}

	//by Mad Max 
			bool			IsServer					();
			bool			IsClient					();
			CSE_Abstract	*spawn_item					(LPCSTR section, const Fvector &position, u32 level_vertex_id, u16 parent_id, bool return_item = false);
			
protected:
	u32		m_dwCL_PingDeltaSend;
	u32		m_dwCL_PingLastSendTime;
	u32		m_dwRealPing;
public:
	virtual	u32				GetRealPing					() { return m_dwRealPing; };

private:
	bool m_is_removing_objects;
public:
	bool is_removing_objects() { return m_is_removing_objects; }
			void			remove_objects				();
	virtual void			OnSessionTerminate			(LPCSTR reason);
	void OnDestroyObject(u16 id) override;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CLevel)
#undef script_type_list
#define script_type_list save_type_list(CLevel)

IC CLevel&				Level()		{ return *((CLevel*) g_pGameLevel);			}
IC game_cl_GameState&	Game()		{ return *Level().game;					}
	u32					GameID();


IC CHUDManager&			HUD()		{ return *((CHUDManager*)Level().pHUD);	}

#ifdef DEBUG
IC CLevelDebug&			DBG()		{return *((CLevelDebug*)Level().m_level_debug);}
#endif


IC CSpaceRestrictionManager	&CLevel::space_restriction_manager()
{
	VERIFY				(m_space_restriction_manager);
	return				(*m_space_restriction_manager);
}

IC CSeniorityHierarchyHolder &CLevel::seniority_holder()
{
	VERIFY				(m_seniority_hierarchy_holder);
	return				(*m_seniority_hierarchy_holder);
}

IC CClientSpawnManager &CLevel::client_spawn_manager()
{
	VERIFY				(m_client_spawn_manager);
	return				(*m_client_spawn_manager);
}

IC CDebugRenderer &CLevel::debug_renderer()
{
	VERIFY				(m_debug_renderer);
	return				(*m_debug_renderer);
}

IC CPHCommander	& CLevel::ph_commander()
{
	VERIFY(m_ph_commander);
	return *m_ph_commander;
}
IC CPHCommander & CLevel::ph_commander_scripts()
{
	VERIFY(m_ph_commander_scripts);
	return *m_ph_commander_scripts;
}
//by Mad Max 
IC bool					OnServer()	{ return Level().IsServer();}
IC bool					OnClient()	{ return Level().IsClient();}

class  CPHWorld;
extern CPHWorld*				ph_world;
extern BOOL						g_bDebugEvents;
