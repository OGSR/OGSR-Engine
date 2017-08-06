#include "pch_script.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "PHCommander.h"
#include "net_queue.h"
#include "MainMenu.h"
#include "space_restriction_manager.h"
#include "ai_space.h"
#include "script_engine.h"
#include "stalker_animation_data_storage.h"
#include "client_spawn_manager.h"
#include "seniority_hierarchy_holder.h"

ENGINE_API bool g_dedicated_server;

const int max_objects_size			= 2*1024;
const int max_objects_size_in_save	= 6*1024;

extern bool	g_b_ClearGameCaptions;

void CLevel::remove_objects	()
{
	if (!IsGameTypeSingle()) Msg("CLevel::remove_objects - Start");
	BOOL						b_stored = psDeviceFlags.test(rsDisableObjectsAsCrows);

	Game().reset_ui				();

	if (OnServer()) {
		VERIFY					(Server);
		Server->SLS_Clear		();
	}
	
	snd_Events.clear			();
	for (int i=0; i<6; ++i) {
		psNET_Flags.set			(NETFLAG_MINIMIZEUPDATES,FALSE);
		// ugly hack for checks that update is twice on frame
		// we need it since we do updates for checking network messages
		++(Device.dwFrame);
		psDeviceFlags.set		(rsDisableObjectsAsCrows,TRUE);
		ClientReceive			();
		ProcessGameEvents		();
		Objects.Update			(true);
		Sleep					(100);
	}

	if (OnClient())
		ClearAllObjects			();

	BulletManager().Clear		();
	ph_commander().clear		();
	ph_commander_scripts().clear();

	if(!g_dedicated_server)
		space_restriction_manager().clear	();

	psDeviceFlags.set			(rsDisableObjectsAsCrows, b_stored);
	g_b_ClearGameCaptions		= true;

	if (!g_dedicated_server)
		ai().script_engine().collect_all_garbage	();

	stalker_animation_data_storage().clear		();
	
	VERIFY										(Render);
	Render->models_Clear						(TRUE/*FALSE*/);
	Render->clear_static_wallmarks				();

#ifdef DEBUG
	if(!g_dedicated_server)
		if (!client_spawn_manager().registry().empty())
			client_spawn_manager().dump				();
#endif // DEBUG
	if(!g_dedicated_server)
	{
		VERIFY										(client_spawn_manager().registry().empty());
		client_spawn_manager().clear			();
	}

	for (int i=0; i<6; i++)
	{
		++(Device.dwFrame);
		Objects.Update(true);
	}

	g_pGamePersistent->destroy_particles		(false);

//.	xr_delete									(m_seniority_hierarchy_holder);
//.	m_seniority_hierarchy_holder				= xr_new<CSeniorityHierarchyHolder>();
	if (!IsGameTypeSingle()) Msg("CLevel::remove_objects - End");
}

#ifdef DEBUG
	extern void	show_animation_stats	();
#endif // DEBUG

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");
	bReady						= false;
	m_bGameConfigStarted		= FALSE;
	game_configured				= FALSE;

	remove_objects				();
	
	IGame_Level::net_Stop		();
	IPureClient::Disconnect		();

	if (Server) {
		Server->Disconnect		();
		xr_delete				(Server);
	}

	if (!g_dedicated_server)
		ai().script_engine().collect_all_garbage	();

#ifdef DEBUG
	show_animation_stats		();
#endif // DEBUG
}


void CLevel::ClientSend()
{
	if (GameID() == GAME_SINGLE || OnClient())
	{
		if ( !net_HasBandwidth() ) return;
	};

#ifdef BATTLEYE
	battleye_system.UpdateClient();
#endif // BATTLEYE

	NET_Packet				P;
	u32						start	= 0;
	//----------- for E3 -----------------------------
//	if () 
	{
//		if (!(Game().local_player) || Game().local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;
		if (CurrentControlEntity()) 
		{
			CObject* pObj = CurrentControlEntity();
			if (!pObj->getDestroy() && pObj->net_Relevant())
			{				
				P.w_begin		(M_CL_UPDATE);
				

				P.w_u16			(u16(pObj->ID())	);
				P.w_u32			(0);	//reserved place for client's ping

				pObj->net_Export			(P);

				if (P.B.count>9)				
				{
					if (OnServer())
					{
						if (net_IsSyncronised() && IsDemoSave()) 
						{
							DemoCS.Enter();
							Demo_StoreData(P.B.data, P.B.count, DATA_CLIENT_PACKET);
							DemoCS.Leave();
						}						
					}
					else
						Send	(P, net_flags(FALSE));
				}				
			}			
		}		
	};
	if (OnClient()) 
	{
		Flush_Send_Buffer();
		return;
	}
	//-------------------------------------------------
	while (1)
	{
		P.w_begin						(M_UPDATE);
		start	= Objects.net_Export	(&P, start, max_objects_size);

		if (P.B.count>2)
		{
			Device.Statistic->TEST3.Begin();
				Send	(P, net_flags(FALSE));
			Device.Statistic->TEST3.End();
		}else
			break;
	}

}

u32	CLevel::Objects_net_Save	(NET_Packet* _Packet, u32 start, u32 max_object_size)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (; start<Objects.o_count(); start++)	{
		CObject		*_P = Objects.o_get_by_iterator(start);
		CGameObject *P = smart_cast<CGameObject*>(_P);
//		Msg			("save:iterating:%d:%s",P->ID(),*P->cName());
		if (P && !P->getDestroy() && P->net_SaveRelevant())	{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open16	(position);
//			Msg						("save:saving:%d:%s",P->ID(),*P->cName());
			P->net_Save				(Packet);
#ifdef DEBUG
			u32 size				= u32		(Packet.w_tell()-position)-sizeof(u16);
//			Msg						("save:saved:%d bytes:%d:%s",size,P->ID(),*P->cName());
			if				(size>=65536)			{
				Debug.fatal	(DEBUG_INFO,"Object [%s][%d] exceed network-data limit\n size=%d, Pend=%d, Pstart=%d",
					*P->cName(), P->ID(), size, Packet.w_tell(), position);
			}
#endif
			Packet.w_chunk_close16	(position);
//			if (0==(--count))		
//				break;
			if (max_object_size > (NET_PacketSizeLimit - Packet.w_tell()))
				break;
		}
	}
	return	++start;
}

void CLevel::ClientSave	()
{
	NET_Packet		P;
	u32				start	= 0;

	for (;;) {
		P.w_begin	(M_SAVE_PACKET);
		
		start		= Objects_net_Save(&P, start, max_objects_size_in_save);

		if (P.B.count>2)
			Send	(P, net_flags(FALSE));
		else
			break;
	}
}

extern		float		phTimefactor;
extern		BOOL		g_SV_Disable_Auth_Check;

void CLevel::Send		(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	if (IsDemoPlay() && m_bDemoStarted) return;
	// optimize the case when server located in our memory
	if(psNET_direct_connect){
		ClientID	_clid;
		_clid.set	(1);
		Server->OnMessage	(P,	_clid );
	}else
	if (Server && game_configured && OnServer() )
	{
		Server->OnMessage	(P,Game().local_svdpnid	);
	}else											
		IPureClient::Send	(P,dwFlags,dwTimeout	);

	if (g_pGameLevel && Level().game && GameID() != GAME_SINGLE && !g_SV_Disable_Auth_Check)		{
		// anti-cheat
		phTimefactor		= 1.f					;
		psDeviceFlags.set	(rsConstantFPS,FALSE)	;	
	}
}

void CLevel::net_Update	()
{
	if(game_configured){
		// If we have enought bandwidth - replicate client data on to server
		Device.Statistic->netClient2.Begin	();
		ClientSend					();
		Device.Statistic->netClient2.End		();
	}
	// If server - perform server-update
	if (Server && OnServer())	{
		Device.Statistic->netServer.Begin();
		Server->Update					();
		Device.Statistic->netServer.End	();
	}
}

struct _NetworkProcessor	: public pureFrame
{
	virtual void OnFrame	( )
	{
		if (g_pGameLevel && !Device.Paused() )	g_pGameLevel->net_Update();
	}
}	NET_processor;

pureFrame*	g_pNetProcessor	= &NET_processor;

const int ConnectionTimeOut = 60000; //1 min

BOOL			CLevel::Connect2Server				(LPCSTR options)
{
	NET_Packet					P;
	m_bConnectResultReceived	= false	;
	m_bConnectResult			= true	;
	if (!Connect(options))		return	FALSE;
	//---------------------------------------------------------------------------
	if(psNET_direct_connect) m_bConnectResultReceived = true;
	u32 EndTime = GetTickCount() + ConnectionTimeOut;
	while	(!m_bConnectResultReceived)		{ 
		ClientReceive	();
		Sleep			(5); 
		if(Server)
			Server->Update()	;
		//-----------------------------------------
		u32 CurTime = GetTickCount();
		if (CurTime > EndTime)
		{
			NET_Packet	P;
			P.B.count = 0;
			P.r_pos = 0;

			P.w_u8(0);
			P.w_u8(0);
			P.w_stringZ("Data verification failed. Cheater? [1]");

			OnConnectResult(&P);			
		}
		if (net_isFails_Connect())
		{
			OnConnectRejected	();	
			Disconnect		()	;
			return	FALSE;
		}
		//-----------------------------------------
	}
	Msg							("%c client : connection %s - <%s>", m_bConnectResult ?'*':'!', m_bConnectResult ? "accepted" : "rejected", m_sConnectResult.c_str());
	if		(!m_bConnectResult) 
	{
		OnConnectRejected	();	
		Disconnect		()	;
		return FALSE		;
	};

	
	if(psNET_direct_connect)
		net_Syncronised = TRUE;
	else
		net_Syncronize	();

	while (!net_IsSyncronised()) {
	};

	//---------------------------------------------------------------------------
	P.w_begin	(M_CLIENT_REQUEST_CONNECTION_DATA);
	Send		(P, net_flags(TRUE, TRUE, TRUE, TRUE));
	//---------------------------------------------------------------------------
	return TRUE;
};

void			CLevel::OnBuildVersionChallenge		()
{
	NET_Packet P;
	P.w_begin				(M_CL_AUTH);
	u64 auth = FS.auth_get();
	P.w_u64					(auth);
	Send					(P, net_flags(TRUE, TRUE, TRUE, TRUE));
};

void			CLevel::OnConnectResult				(NET_Packet*	P)
{
	// multiple results can be sent during connection they should be "AND-ed"
	m_bConnectResultReceived	= true;
	u8	result					= P->r_u8();
	u8  res1					= P->r_u8();
	string128 ResultStr			;	
	P->r_stringZ(ResultStr)		;
	if (!result)				
	{
		m_bConnectResult	= false			;	
		switch (res1)
		{
		case 0:		//Standart error
			{
				if (!xr_strcmp(ResultStr, "Data verification failed. Cheater? [2]"))
					MainMenu()->SetErrorDialog(CMainMenu::ErrDifferentVersion);
			}break;
		case 1:		//GameSpy CDKey
			{
				if (!xr_strcmp(ResultStr, "Invalid CD Key"))
					MainMenu()->SetErrorDialog(CMainMenu::ErrCDKeyInvalid);//, ResultStr);
				if (!xr_strcmp(ResultStr, "CD Key in use"))
					MainMenu()->SetErrorDialog(CMainMenu::ErrCDKeyInUse);//, ResultStr);
				if (!xr_strcmp(ResultStr, "Your CD Key is disabled. Contact customer service."))
					MainMenu()->SetErrorDialog(CMainMenu::ErrCDKeyDisabled);//, ResultStr);
			}break;		
		case 2:		//login+password
			{
				MainMenu()->SetErrorDialog(CMainMenu::ErrInvalidPassword);
			}break;		
		}
	};	
	m_sConnectResult			= ResultStr;
	
	if (IsDemoSave())
	{
//		P->r_stringZ(m_sDemoHeader.LevelName);
//		P->r_stringZ(m_sDemoHeader.GameType);
		m_sDemoHeader.bServerClient = P->r_u8();
		P->r_stringZ(m_sDemoHeader.ServerOptions);
		//-----------------------------------------
		FILE* fTDemo = fopen(m_sDemoName, "ab");
		if (fTDemo)
		{
			fwrite(&m_sDemoHeader.bServerClient, 32, 1, fTDemo);
			
			DWORD OptLen = m_sDemoHeader.ServerOptions.size();
			fwrite(&OptLen, 4, 1, fTDemo);
			fwrite(*m_sDemoHeader.ServerOptions, OptLen, 1, fTDemo);
			fclose(fTDemo);
		};
		//-----------------------------------------
	};	
};

void			CLevel::ClearAllObjects				()
{

	bool ParentFound = true;
	
	while (ParentFound)
	{	
		ProcessGameEvents				();

		u32 CLObjNum					= Level().Objects.o_count();
		ParentFound						= false;

		for (u32 i=0; i<CLObjNum; i++)
		{
			CObject* pObj				= Level().Objects.o_get_by_iterator(i);
			if (!pObj->H_Parent()) 
				continue;
			//-----------------------------------------------------------
			NET_Packet					GEN;
			GEN.w_begin					(M_EVENT);
			//------------------		---------------------------		
			GEN.w_u32					(Level().timeServer());
			GEN.w_u16					(GE_OWNERSHIP_REJECT);
			GEN.w_u16					(pObj->H_Parent()->ID());
			GEN.w_u16					(u16(pObj->ID()));
			game_events->insert			(GEN);
			if (g_bDebugEvents)	
				ProcessGameEvents		();
			//-------------------------------------------------------------
			ParentFound					= true;
			//-------------------------------------------------------------
#ifdef DEBUG
			Msg ("Rejection of %s[%d] from %s[%d]", *(pObj->cNameSect()), pObj->ID(), *(pObj->H_Parent()->cNameSect()), pObj->H_Parent()->ID());
#endif
		};
	};

	u32 CLObjNum = Level().Objects.o_count();

	for (u32 i=0; i<CLObjNum; i++)
	{
		CObject* pObj = Level().Objects.o_get_by_iterator(i);
		R_ASSERT(pObj->H_Parent()==NULL);
		//-----------------------------------------------------------
		NET_Packet			GEN;
		GEN.w_begin			(M_EVENT);
		//---------------------------------------------		
		GEN.w_u32			(Level().timeServer());
		GEN.w_u16			(GE_DESTROY);
		GEN.w_u16			(u16(pObj->ID()));
		game_events->insert	(GEN);
		if (g_bDebugEvents)	ProcessGameEvents();
		//-------------------------------------------------------------
		ParentFound = true;
		//-------------------------------------------------------------
#ifdef DEBUG
		Msg ("Destruction of %s[%d]", *(pObj->cNameSect()), pObj->ID());
#endif
	};
	ProcessGameEvents();
};

void				CLevel::OnInvalidHost			()
{
	IPureClient::OnInvalidHost();
	if (MainMenu()->GetErrorDialogType() == CMainMenu::ErrNoError)
		MainMenu()->SetErrorDialog(CMainMenu::ErrInvalidHost);
};

void				CLevel::OnInvalidPassword		()
{
	IPureClient::OnInvalidPassword();
	MainMenu()->SetErrorDialog(CMainMenu::ErrInvalidPassword);
};

void				CLevel::OnSessionFull			()
{
	IPureClient::OnSessionFull();
	if (MainMenu()->GetErrorDialogType() == CMainMenu::ErrNoError)
		MainMenu()->SetErrorDialog(CMainMenu::ErrSessionFull);
}

void				CLevel::OnConnectRejected		()
{
	IPureClient::OnConnectRejected();

//	if (MainMenu()->GetErrorDialogType() != CMainMenu::ErrNoError)
//		MainMenu()->SetErrorDialog(CMainMenu::ErrServerReject);
};

void				CLevel::net_OnChangeSelfName			(NET_Packet* P)
{
	if (!P) return;
	string64 NewName			;
	P->r_stringZ(NewName)		;
	if (!strstr(*m_caClientOptions, "/name="))
	{
		string1024 tmpstr;
		strcpy_s(tmpstr, *m_caClientOptions);
		strcat_s(tmpstr, "/name=");
		strcat_s(tmpstr, NewName);
		m_caClientOptions = tmpstr;
	}
	else
	{
		string1024 tmpstr;
		strcpy_s(tmpstr, *m_caClientOptions);
		*(strstr(tmpstr, "name=")+5) = 0;
		strcat_s(tmpstr, NewName);
		const char* ptmp = strstr(strstr(*m_caClientOptions, "name="), "/");
		if (ptmp)
			strcat_s(tmpstr, ptmp);
		m_caClientOptions = tmpstr;
	}
}