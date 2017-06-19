#include "stdafx.h"
//#include "dxerr.h"
#include "NET_Common.h"
#include "net_server.h"

#include "NET_Log.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

static	INetLog* pSvNetLog = NULL; 

#define BASE_PORT_LAN_SV		5445
#define BASE_PORT				0
#define END_PORT				65535

#define NET_BANNED_STR                "Player banned by server!"
#define NET_PROTECTED_SERVER_STR      "Access denied by protected server for this player!"

void	dump_URL	(LPCSTR p, IDirectPlay8Address* A);

LPCSTR nameTraffic	= "traffic.net";

XRNETSERVER_API int		psNET_ServerUpdate	= 30;		// FPS
XRNETSERVER_API int		psNET_ServerPending	= 2;

XRNETSERVER_API ClientID BroadcastCID(0xffffffff);

void ip_address::set(LPCSTR src_string)
{
	u32		buff[4];
	int cnt = sscanf(src_string, "%d.%d.%d.%d", &buff[0], &buff[1], &buff[2], &buff[3]);
	if(cnt==4)
	{
		m_data.a1	= u8(buff[0]&0xff);
		m_data.a2	= u8(buff[1]&0xff);
		m_data.a3	= u8(buff[2]&0xff);
		m_data.a4	= u8(buff[3]&0xff);
	}else
	{
		Msg			("! Bad ipAddress format [%s]",src_string);
		m_data.data		= 0;
	}
}

xr_string ip_address::to_string() const
{
	string128	res;
	sprintf_s	(res,sizeof(res),"%d.%d.%d.%d", m_data.a1, m_data.a2, m_data.a3, m_data.a4);
	return		res;
}

// ==================================================================================================
void IBannedClient::Load(CInifile& ini, const shared_str& sect)
{
	HAddr.set					(sect.c_str());

	tm							_tm_banned;
	const shared_str& time_to	= ini.r_string(sect,"time_to");
	int res_t					= sscanf(	time_to.c_str(),
											"%02d.%02d.%d_%02d:%02d:%02d", 
											&_tm_banned.tm_mday, 
											&_tm_banned.tm_mon, 
											&_tm_banned.tm_year, 
											&_tm_banned.tm_hour, 
											&_tm_banned.tm_min, 
											&_tm_banned.tm_sec);
	VERIFY(res_t==6);

	_tm_banned.tm_mon			-= 1;
	_tm_banned.tm_year			-= 1900;

	BanTime						= mktime(&_tm_banned);
	
	Msg("- loaded banned client %s to %s", HAddr.to_string().c_str(), BannedTimeTo().c_str());
}

void IBannedClient::Save(CInifile& ini)
{
	ini.w_string		(HAddr.to_string().c_str(), "time_to", BannedTimeTo().c_str());
}

xr_string IBannedClient::BannedTimeTo() const
{
	string256			res;
	tm*					_tm_banned;
	_tm_banned			= _localtime64(&BanTime);
	sprintf_s			(	res, sizeof(res),
							"%02d.%02d.%d_%02d:%02d:%02d",
							_tm_banned->tm_mday, 
							_tm_banned->tm_mon+1, 
							_tm_banned->tm_year+1900, 
							_tm_banned->tm_hour, 
							_tm_banned->tm_min, 
							_tm_banned->tm_sec);

	return res;
}


void gen_auth_code()
{
		xr_vector<xr_string>	ignore, test	;

		LPCSTR pth				= FS.get_path("$app_data_root$")->m_Path;
		ignore.push_back		(xr_string(pth));
		ignore.push_back		(xr_string("gamedata\\config\\localization.ltx"));
		ignore.push_back		(xr_string("gamedata\\config\\fonts.ltx"));
		ignore.push_back		(xr_string("gamedata\\config\\misc\\items.ltx"));
		ignore.push_back		(xr_string("gamedata\\config\\text"));
		ignore.push_back		(xr_string("gamedata\\config\\gameplay"));
		ignore.push_back		(xr_string("gamedata\\config\\ui"));

		test.push_back			(xr_string("gamedata\\config"));
//.		test.push_back			(xr_string("gamedata\\scripts"));
		test.push_back			(xr_string("gamedata\\shaders"));
		test.push_back			(xr_string("gamedata\\textures\\act"));
		test.push_back			(xr_string("gamedata\\textures\\wpn"));

		test.push_back			(xr_string("xrd3d9-null.dll"));
		test.push_back			(xr_string("ode.dll"));
		test.push_back			(xr_string("xrcdb.dll"));
		test.push_back			(xr_string("xrcore.dll"));
		test.push_back			(xr_string("xrcpu_pipe.dll"));
		test.push_back			(xr_string("xrgame.dll"));
		test.push_back			(xr_string("xrgamespy.dll"));
		test.push_back			(xr_string("xrlua.dll"));
		test.push_back			(xr_string("xrnetserver.dll"));
		test.push_back			(xr_string("xrparticles.dll"));
//		test.push_back			(xr_string("xrrender_r1.dll"));
		test.push_back			(xr_string("xrrender.dll"));
		test.push_back			(xr_string("xrsound.dll"));
		test.push_back			(xr_string("xrxmlparser.dll"));
//		test.push_back			(xr_string("xr_3da.exe"));

		FS.auth_generate		(ignore,test);
}

IClient::IClient( CTimer* timer )
  : stats(timer),
    server(NULL)
{
	dwTime_LastUpdate	= 0;
	flags.bLocal = FALSE;
	flags.bConnected = FALSE;
	flags.bReconnect = FALSE;
	flags.bVerified = TRUE;
}

IClient::~IClient()
{
}

void IClientStatistic::Update(DPN_CONNECTION_INFO& CI)
{
	u32 time_global		= TimeGlobal(device_timer);
	if (time_global-dwBaseTime >= 999)
	{
		dwBaseTime		= time_global;
		
		mps_recive		= CI.dwMessagesReceived - mps_receive_base;
		mps_receive_base= CI.dwMessagesReceived;

		u32	cur_msend	= CI.dwMessagesTransmittedHighPriority+CI.dwMessagesTransmittedNormalPriority+CI.dwMessagesTransmittedLowPriority;
		mps_send		= cur_msend - mps_send_base;
		mps_send_base	= cur_msend;

		dwBytesPerSec = (dwBytesPerSec*9 + dwBytesSended)/10;
		dwBytesSended = 0;
	}
	ci_last	= CI;
}

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b, 0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };
// {8D3F9E5E-A3BD-475b-9E49-B0E77139143C}
static const GUID CLSID_NETWORKSIMULATOR_DP8SP_TCPIP =
{ 0x8d3f9e5e, 0xa3bd, 0x475b, { 0x9e, 0x49, 0xb0, 0xe7, 0x71, 0x39, 0x14, 0x3c } };

static HRESULT WINAPI Handler (PVOID pvUserContext, DWORD dwMessageType, PVOID pMessage)
{
	IPureServer* C			= (IPureServer*)pvUserContext;
	return C->net_Handler	(dwMessageType,pMessage);
}


//------------------------------------------------------------------------------

void    
IClient::_SendTo_LL( const void* data, u32 size, u32 flags, u32 timeout )
{
    R_ASSERT(server);
    server->IPureServer::SendTo_LL( ID, const_cast<void*>(data), size, flags, timeout );
}


//------------------------------------------------------------------------------
IClient*	IPureServer::ID_to_client		(ClientID ID, bool ScanAll)
{
	if ( 0 == ID.value() )			return NULL;
	csPlayers.Enter	();

	for ( u32 client = 0; client < net_Players.size(); ++client )
	{
		if ( net_Players[client]->ID == ID )
		{
			csPlayers.Leave();
			return net_Players[client];
		}
	}
	if ( ScanAll )
	{
		for ( u32 client = 0; client < net_Players_disconnected.size(); ++client )
		{
			if ( net_Players_disconnected[client]->ID == ID )
			{
				csPlayers.Leave();
				return net_Players_disconnected[client];
			}
		}
	};
	csPlayers.Leave();
	return NULL;
}

void
IPureServer::_Recieve( const void* data, u32 data_size, u32 param )
{
	if (data_size > NET_PacketSizeLimit) {
		Msg		("! too large packet size[%d] received, DoS attack?", data_size);
		return;
	}

    NET_Packet  packet; 
    ClientID    id;

    id.set( param );
    packet.construct( data, data_size );
	csMessage.Enter();
	//---------------------------------------
	if( psNET_Flags.test(NETFLAG_LOG_SV_PACKETS) ) 
	{
		if( !pSvNetLog) 
			pSvNetLog = xr_new<INetLog>("logs\\net_sv_log.log", TimeGlobal(device_timer));
		    
		if( pSvNetLog ) 
		    pSvNetLog->LogPacket( TimeGlobal(device_timer), &packet, TRUE );
	}
	//---------------------------------------
	u32	result = OnMessage( packet, id );

	csMessage.Leave();
	
	if( result )		
	    SendBroadcast( id, packet, result );
}


//==============================================================================

IPureServer::IPureServer	(CTimer* timer, BOOL	Dedicated)
	:	m_bDedicated(Dedicated)
#ifdef PROFILE_CRITICAL_SECTIONS
	,csPlayers(MUTEX_PROFILE_ID(IPureServer::csPlayers))
	,csMessage(MUTEX_PROFILE_ID(IPureServer::csMessage))
#endif // PROFILE_CRITICAL_SECTIONS
{
	device_timer			= timer;
	stats.clear				();
	stats.dwSendTime		= TimeGlobal(device_timer);
	SV_Client				= NULL;
	NET						= NULL;
	net_Address_device		= NULL;
	pSvNetLog				= NULL;//xr_new<INetLog>("logs\\net_sv_log.log", TimeGlobal(device_timer));
}

IPureServer::~IPureServer	()
{
	for	(u32 it=0; it<BannedAddresses.size(); it++)	
		xr_delete(BannedAddresses[it]);

	BannedAddresses.clear		();

	SV_Client					= NULL;

	xr_delete					(pSvNetLog); 

	psNET_direct_connect		= FALSE;
}

IPureServer::EConnect IPureServer::Connect(LPCSTR options)
{
	connect_options			= options;
	psNET_direct_connect = FALSE;

	if(strstr(options, "/single") && !strstr(Core.Params, "-no_direct_connect" ))
		psNET_direct_connect	=	TRUE;
	else{
		gen_auth_code	();
	}

	// Parse options
	string4096				session_name;
	string4096				session_options = "";
	string64				password_str = "";
	u32						dwMaxPlayers = 0;

	strcpy					(session_name,options);
	if (strchr(session_name,'/'))	*strchr(session_name,'/')=0;
	if (strchr(options,'/'))		strcpy(session_options, strchr(options,'/')+1);
	if (strstr(options, "psw="))
	{
		const char* PSW = strstr(options, "psw=") + 4;
		if (strchr(PSW, '/')) 
			strncpy(password_str, PSW, strchr(PSW, '/') - PSW);
		else
			strncpy(password_str, PSW, 63);
	}
	if (strstr(options, "maxplayers="))
	{
		const char* sMaxPlayers = strstr(options, "maxplayers=") + 11;
		string64 tmpStr = "";
		if (strchr(sMaxPlayers, '/')) 
			strncpy(tmpStr, sMaxPlayers, strchr(sMaxPlayers, '/') - sMaxPlayers);
		else
			strncpy(tmpStr, sMaxPlayers, 63);
		dwMaxPlayers = atol(tmpStr);
	}
	if (dwMaxPlayers > 32 || dwMaxPlayers<1) dwMaxPlayers = 32;
	Msg("MaxPlayers = %d", dwMaxPlayers);

	//-------------------------------------------------------------------
	BOOL bPortWasSet = FALSE;
	u32 dwServerPort = BASE_PORT_LAN_SV;
	if (strstr(options, "portsv="))
	{
		const char* ServerPort = strstr(options, "portsv=") + 7;
		string64 tmpStr = "";
		if (strchr(ServerPort, '/')) 
			strncpy(tmpStr, ServerPort, strchr(ServerPort, '/') - ServerPort);
		else
			strncpy(tmpStr, ServerPort, 63);
		dwServerPort = atol(tmpStr);
		clamp(dwServerPort, u32(BASE_PORT), u32(END_PORT));
		bPortWasSet = TRUE; //this is not casual game
	}
	//-------------------------------------------------------------------

if(!psNET_direct_connect)
{
	//---------------------------
#ifdef DEBUG
//	string1024 tmp;
#endif // DEBUG
//	HRESULT CoInitializeExRes = CoInitializeEx(NULL, 0);	
//	if (CoInitializeExRes != S_OK && CoInitializeExRes != S_FALSE)
//	{
//		DXTRACE_ERR(tmp, CoInitializeExRes);
//		CHK_DX(CoInitializeExRes);
//	};	
	//---------------------------
    // Create the IDirectPlay8Client object.
	HRESULT CoCreateInstanceRes = CoCreateInstance	(CLSID_DirectPlay8Server, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Server, (LPVOID*) &NET);
	//---------------------------	
	if (CoCreateInstanceRes != S_OK)
	{
#ifdef DEBUG
//		DXTRACE_ERR(tmp, CoCreateInstanceRes );
#endif // DEBUG
		CHK_DX(CoCreateInstanceRes );
	}	
	//---------------------------
	
    // Initialize IDirectPlay8Client object.
#ifdef DEBUG
    CHK_DX(NET->Initialize	(this, Handler, 0));
#else
	CHK_DX(NET->Initialize	(this, Handler, DPNINITIALIZE_DISABLEPARAMVAL));
#endif

	BOOL	bSimulator		= FALSE;
	if (strstr(Core.Params,"-netsim"))		bSimulator = TRUE;
	
	
	// dump_URL		("! sv ",	net_Address_device);

	// Set server-player info
    DPN_APPLICATION_DESC		dpAppDesc;
    DPN_PLAYER_INFO				dpPlayerInfo;
    WCHAR						wszName		[] = L"XRay Server";
	
    ZeroMemory					(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize			= sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags	= DPNINFO_NAME;
    dpPlayerInfo.pwszName		= wszName;
    dpPlayerInfo.pvData			= NULL;
    dpPlayerInfo.dwDataSize		= NULL;
    dpPlayerInfo.dwPlayerFlags	= 0;
	
	CHK_DX(NET->SetServerInfo( &dpPlayerInfo, NULL, NULL, DPNSETSERVERINFO_SYNC ) );
	
    // Set server/session description
	WCHAR	SessionNameUNICODE[4096];
	CHK_DX(MultiByteToWideChar(CP_ACP, 0, session_name, -1, SessionNameUNICODE, 4096 ));
    // Set server/session description
	
    // Now set up the Application Description
    ZeroMemory					(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.dwFlags			= DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;
    dpAppDesc.guidApplication	= NET_GUID;
    dpAppDesc.pwszSessionName	= SessionNameUNICODE;
	dpAppDesc.dwMaxPlayers		= (m_bDedicated) ? (dwMaxPlayers+2) : (dwMaxPlayers+1);
	dpAppDesc.pvApplicationReservedData	= session_options;
	dpAppDesc.dwApplicationReservedDataSize = xr_strlen(session_options)+1;

	WCHAR	SessionPasswordUNICODE[4096];
	if (xr_strlen(password_str))
	{
		CHK_DX(MultiByteToWideChar(CP_ACP, 0, password_str, -1, SessionPasswordUNICODE, 4096 ));
		dpAppDesc.dwFlags |= DPNSESSION_REQUIREPASSWORD;
		dpAppDesc.pwszPassword = SessionPasswordUNICODE;
	};
	
	// Create our IDirectPlay8Address Device Address, --- Set the SP for our Device Address
	net_Address_device = NULL;
	CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_device )); 
	CHK_DX(net_Address_device->SetSP		(bSimulator? &CLSID_NETWORKSIMULATOR_DP8SP_TCPIP : &CLSID_DP8SP_TCPIP ));
	
	DWORD dwTraversalMode = DPNA_TRAVERSALMODE_NONE;
	CHK_DX(net_Address_device->AddComponent(DPNA_KEY_TRAVERSALMODE, &dwTraversalMode, sizeof(dwTraversalMode), DPNA_DATATYPE_DWORD));

	HRESULT HostSuccess = S_FALSE;
	// We are now ready to host the app and will try different ports
	psNET_Port = dwServerPort;//BASE_PORT;
	while (HostSuccess != S_OK && psNET_Port <=END_PORT)
	{
		CHK_DX(net_Address_device->AddComponent	(DPNA_KEY_PORT, &psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));

		HostSuccess = NET->Host			
			(
			&dpAppDesc,				// AppDesc
			&net_Address_device, 1, // Device Address
			NULL, NULL,             // Reserved
			NULL,                   // Player Context
			0 );					// dwFlags
		if (HostSuccess != S_OK)
		{
//			xr_string res = Debug.error2string(HostSuccess);
				if (bPortWasSet) 
				{
					Msg("! IPureServer : port %d is BUSY!", psNET_Port);
					return ErrConnect;
				}
#ifdef DEBUG
				else
					Msg("! IPureServer : port %d is BUSY!", psNET_Port);
#endif	
				psNET_Port++;
		}
		else
		{
			Msg("- IPureServer : created on port %d!", psNET_Port);
		}
	};
	
	CHK_DX(HostSuccess);

}	//psNET_direct_connect

//.	config_Load		();

	if(!psNET_direct_connect)
		BannedList_Load	();

	return	ErrNoError;
}

void IPureServer::Disconnect	()
{
//.	config_Save		();

	if (!psNET_direct_connect)
		BannedList_Save	();

    if( NET )	NET->Close(0);
	
	// Release interfaces
    _RELEASE	(net_Address_device);
    _RELEASE	(NET);
}


HRESULT	IPureServer::net_Handler(u32 dwMessageType, PVOID pMessage)
{
    // HRESULT     hr = S_OK;
	
    switch (dwMessageType)
    {
	case DPN_MSGID_ENUM_HOSTS_QUERY :
		{
			PDPNMSG_ENUM_HOSTS_QUERY	msg = PDPNMSG_ENUM_HOSTS_QUERY(pMessage);
			if (0 == msg->dwReceivedDataSize) return S_FALSE;
			if (!stricmp((const char*)msg->pvReceivedData, "ToConnect")) return S_OK;
			if (*((const GUID*) msg->pvReceivedData) != NET_GUID) return S_FALSE;
			if (!OnCL_QueryHost()) return S_FALSE;
			return S_OK;
		}break;
	case DPN_MSGID_CREATE_PLAYER :
        {
			PDPNMSG_CREATE_PLAYER	msg = PDPNMSG_CREATE_PLAYER(pMessage);
			const	u32				max_size = 1024;
			char	bufferData		[max_size];
            DWORD	bufferSize		= max_size;
			ZeroMemory				(bufferData,bufferSize);
			string512				res;

			// retreive info
			DPN_PLAYER_INFO*		Pinfo = (DPN_PLAYER_INFO*) bufferData;
			Pinfo->dwSize			= sizeof(DPN_PLAYER_INFO);
			HRESULT _hr				= NET->GetClientInfo( msg->dpnidPlayer, Pinfo, &bufferSize, 0 );
			if( _hr == DPNERR_INVALIDPLAYER )
			{
				Assign_ServerType( res ); //once
				break;	// server player
			}

			CHK_DX				(_hr);
			
			string64			cname;
			CHK_DX( WideCharToMultiByte( CP_ACP, 0, Pinfo->pwszName, -1, cname, sizeof(cname) , 0, 0 ) );

			SClientConnectData	cl_data;
			strcpy_s( cl_data.name, cname );

			if( Pinfo->pvData && Pinfo->dwDataSize == sizeof(cl_data) )
			{
				cl_data		= *((SClientConnectData*)Pinfo->pvData);
			}
			cl_data.clientID.set( msg->dpnidPlayer );

			new_client( &cl_data );
        }
		break;
	case DPN_MSGID_DESTROY_PLAYER:
		{
			PDPNMSG_DESTROY_PLAYER	msg = PDPNMSG_DESTROY_PLAYER(pMessage);

			csPlayers.Enter			();
			for (u32 I=0; I<net_Players.size(); I++)
				if (net_Players[I]->ID.compare(msg->dpnidPlayer) )
				{
					// gen message
					net_Players[I]->flags.bConnected	= FALSE;
					net_Players[I]->flags.bReconnect	= FALSE;
					OnCL_Disconnected	(net_Players[I]);

					// real destroy
					client_Destroy		(net_Players[I]);
					break;
				}
			csPlayers.Leave			();
		}
		break;
	case DPN_MSGID_RECEIVE:
        {

            PDPNMSG_RECEIVE	pMsg = PDPNMSG_RECEIVE(pMessage);
			void*	m_data		= pMsg->pReceiveData;
			u32		m_size		= pMsg->dwReceiveDataSize;
			DPNID   m_sender	= pMsg->dpnidSender;

			MSYS_PING*	m_ping	= (MSYS_PING*)m_data;
			
			if ((m_size>2*sizeof(u32)) && (m_ping->sign1==0x12071980) && (m_ping->sign2==0x26111975))
			{
				// this is system message
				if (m_size==sizeof(MSYS_PING))
				{
					// ping - save server time and reply
					m_ping->dwTime_Server	= TimerAsync(device_timer);
					ClientID ID; ID.set(m_sender);
					//						IPureServer::SendTo_LL	(ID,m_data,m_size,net_flags(FALSE,FALSE,TRUE));
					IPureServer::SendTo_Buf	(ID,m_data,m_size,net_flags(FALSE,FALSE,TRUE, TRUE));
				}
			} 
			else 
			{
                MultipacketReciever::RecievePacket( pMsg->pReceiveData, pMsg->dwReceiveDataSize, m_sender );
			}
        } break;
        
	case DPN_MSGID_INDICATE_CONNECT :
		{
			PDPNMSG_INDICATE_CONNECT msg = (PDPNMSG_INDICATE_CONNECT)pMessage;

			ip_address			HAddr;
			GetClientAddress	(msg->pAddressPlayer, HAddr);

			if (GetBannedClient(HAddr)) 
			{
				msg->dwReplyDataSize	= xr_strlen(NET_BANNED_STR);
				msg->pvReplyData		= NET_BANNED_STR;
				return					S_FALSE;
			};
		}break;
    }
	
    return S_OK;
}

void	IPureServer::Flush_Clients_Buffers	()
{
    #if NET_LOG_PACKETS
    Msg( "#flush server send-buf" );
    #endif

	csPlayers.Enter();	

	for( xr_vector<IClient*>::iterator it = net_Players.begin(); it != net_Players.end(); ++it )
        (*it)->MultipacketSender::FlushSendBuffer( 0 );

	csPlayers.Leave();
}

void	IPureServer::SendTo_Buf(ClientID id, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
    xr_vector<IClient*>::iterator it = std::find( net_Players.begin(), net_Players.end(), id );

    if( it != net_Players.end()) 
        (*it)->MultipacketSender::SendPacket( data, size, dwFlags, dwTimeout );
}


void	IPureServer::SendTo_LL(ClientID ID/*DPNID ID*/, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
	//	if (psNET_Flags.test(NETFLAG_LOG_SV_PACKETS)) pSvNetLog->LogData(TimeGlobal(device_timer), data, size);
	if (psNET_Flags.test(NETFLAG_LOG_SV_PACKETS)) 
	{
		if (!pSvNetLog) pSvNetLog = xr_new<INetLog>("logs\\net_sv_log.log", TimeGlobal(device_timer));
		if (pSvNetLog) pSvNetLog->LogData(TimeGlobal(device_timer), data, size);
	}

	// send it
	DPN_BUFFER_DESC		desc;
	desc.dwBufferSize	= size;
	desc.pBufferData	= LPBYTE(data);

#ifdef _DEBUG
	u32 time_global		= TimeGlobal(device_timer);
	if (time_global - stats.dwSendTime >= 999)
	{
		stats.dwBytesPerSec = (stats.dwBytesPerSec*9 + stats.dwBytesSended)/10;
		stats.dwBytesSended = 0;
		stats.dwSendTime = time_global;
	};
	if ( ID.value() )
		stats.dwBytesSended += size;
#endif

	// verify
	VERIFY		(desc.dwBufferSize);
	VERIFY		(desc.pBufferData);

	DPNHANDLE	hAsync	= 0;
	HRESULT		_hr		= NET->SendTo(
		ID.value(),
		&desc,1,
		dwTimeout,
		0,&hAsync,
		dwFlags | DPNSEND_COALESCE 
		);

	
//	Msg("- IPureServer::SendTo_LL [%d]", size);

	if (SUCCEEDED(_hr) || (DPNERR_CONNECTIONLOST==_hr))	return;

	R_CHK		(_hr);

}

void	IPureServer::SendTo		(ClientID ID/*DPNID ID*/, NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	SendTo_LL( ID, P.B.data, P.B.count, dwFlags, dwTimeout );
}

void	IPureServer::SendBroadcast_LL(ClientID exclude, void* data, u32 size, u32 dwFlags)
{
	csPlayers.Enter();
	
	for( u32 i=0; i<net_Players.size(); i++ )
	{
		IClient* player = net_Players[i];
		
		if( player->ID == exclude )     continue;
		if( !player->flags.bConnected ) continue;
		
		SendTo_LL( player->ID, data, size, dwFlags );
	}
	
	csPlayers.Leave	();
}

void	IPureServer::SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags)
{
	// Perform broadcasting
	SendBroadcast_LL( exclude, P.B.data, P.B.count, dwFlags );
}

u32	IPureServer::OnMessage	(NET_Packet& P, ClientID sender)	// Non-Zero means broadcasting with "flags" as returned
{
	/*
	u16 m_type;
	P.r_begin	(m_type);
	switch (m_type)
	{
	case M_CHAT:
		{
			char	buffer[256];
			P.r_string(buffer);
			printf	("RECEIVE: %s\n",buffer);
		}
		break;
	}
	*/
	
	return 0;
}

void IPureServer::OnCL_Connected		(IClient* CL)
{
	Msg("* Player '%s' connected.\n",	CL->name.c_str());
}
void IPureServer::OnCL_Disconnected		(IClient* CL)
{
	Msg("* Player '%s' disconnected.\n",CL->name.c_str());
}

BOOL IPureServer::HasBandwidth			(IClient* C)
{
	u32	dwTime			= TimeGlobal(device_timer);
	u32	dwInterval		= 0;

	if(psNET_direct_connect)
	{
		UpdateClientStatistic	(C);
		C->dwTime_LastUpdate	= dwTime;
		dwInterval				= 1000;
		return					TRUE;
	}
	
	if (psNET_ServerUpdate != 0) dwInterval = 1000/psNET_ServerUpdate; 
	if	(psNET_Flags.test(NETFLAG_MINIMIZEUPDATES))	dwInterval	= 1000;	// approx 2 times per second

	HRESULT hr;
	if (psNET_ServerUpdate != 0 && (dwTime-C->dwTime_LastUpdate)>dwInterval)
	{
		// check queue for "empty" state
		DWORD				dwPending;
		hr					= NET->GetSendQueueInfo(C->ID.value(),&dwPending,0,0);
		if (FAILED(hr))		return FALSE;

		if (dwPending > u32(psNET_ServerPending))	
		{
			C->stats.dwTimesBlocked++;
			return FALSE;
		};

		UpdateClientStatistic(C);
		// ok
		C->dwTime_LastUpdate	= dwTime;
		return TRUE;
	}
	return FALSE;
}

void	IPureServer::UpdateClientStatistic		(IClient* C)
{
	// Query network statistic for this client
	DPN_CONNECTION_INFO			CI;
	ZeroMemory					(&CI,sizeof(CI));
	CI.dwSize					= sizeof(CI);
	if(!psNET_direct_connect)
	{
		HRESULT hr					= NET->GetConnectionInfo(C->ID.value(),&CI,0);
		if (FAILED(hr))				return;
	}
	C->stats.Update				(CI);
}

void	IPureServer::ClearStatistic	()
{
	stats.clear();
	for (u32 I=0; I<net_Players.size(); I++)
	{
		net_Players[I]->stats.Clear();
	}
};

bool			IPureServer::DisconnectClient	(IClient* C)
{
	if (!C) return false;

	string64 Reason = "Kicked by server";
	HRESULT res = NET->DestroyClient(C->ID.value(), Reason, xr_strlen(Reason)+1, 0);
	CHK_DX(res);
	return true;
};

bool			IPureServer::DisconnectClient	(IClient* C,string512& Reason)
{
	if (!C) return false;

	HRESULT res = NET->DestroyClient(C->ID.value(), Reason, xr_strlen(Reason)+1, 0);
	CHK_DX(res);
	return true;
};

bool	IPureServer::DisconnectAddress	(const ip_address& Address)
{
	xr_vector<IClient*>		PlayersToDisconnect;

	for (u32 idx = 0; idx<net_Players.size(); ++idx)
	{
		ip_address			ClAddress;
		GetClientAddress	(net_Players[idx]->ID, ClAddress);

		if (Address == ClAddress)
		{
			PlayersToDisconnect.push_back(net_Players[idx]);
		};
	};

	xr_vector<IClient*>::iterator it	= PlayersToDisconnect.begin();
	xr_vector<IClient*>::iterator it_e	= PlayersToDisconnect.end();

	for ( ;it!=it_e; ++it)
	{
		DisconnectClient(*it);
	};
	return true;
};

bool IPureServer::GetClientAddress	(IDirectPlay8Address* pClientAddress, ip_address& Address, DWORD* pPort)
{
	WCHAR				wstrHostname[ 256 ] = {0};
	DWORD dwSize		= sizeof(wstrHostname);
	DWORD dwDataType	= 0;
	CHK_DX(pClientAddress->GetComponentByName( DPNA_KEY_HOSTNAME, wstrHostname, &dwSize, &dwDataType ));

	string256				HostName;
	CHK_DX(WideCharToMultiByte(CP_ACP,0,wstrHostname,-1,HostName,sizeof(HostName),0,0));

	Address.set		(HostName);

	if (pPort != NULL)
	{
		DWORD dwPort			= 0;
		DWORD dwPortSize		= sizeof(dwPort);
		DWORD dwPortDataType	= DPNA_DATATYPE_DWORD;
		CHK_DX(pClientAddress->GetComponentByName( DPNA_KEY_PORT, &dwPort, &dwPortSize, &dwPortDataType ));
		*pPort					= dwPort;
	};

	return true;
};

bool IPureServer::GetClientAddress	(ClientID ID, ip_address& Address, DWORD* pPort)
{
	IDirectPlay8Address* pClAddr	= NULL;
	CHK_DX(NET->GetClientAddress	(ID.value(), &pClAddr, 0));

	return GetClientAddress			(pClAddr, Address, pPort);
};

IBannedClient*	IPureServer::GetBannedClient(const ip_address& Address)
{
	for	(u32 it=0; it<BannedAddresses.size(); it++)	
	{
		IBannedClient* pBClient = BannedAddresses[it];
		if ( pBClient->HAddr == Address ) 
			return pBClient;
	}
	return NULL;
};

void IPureServer::BanClient(IClient* C, u32 BanTime)
{
	ip_address				ClAddress;
	GetClientAddress		(C->ID, ClAddress);
	BanAddress				(ClAddress, BanTime);
};

void IPureServer::BanAddress(const ip_address& Address, u32 BanTimeSec)
{
	if (GetBannedClient(Address)) 
	{
		Msg("Already banned\n");
		return;
	};

	IBannedClient* pNewClient = xr_new<IBannedClient>();
	pNewClient->HAddr				= Address;
	time							(&pNewClient->BanTime);
	pNewClient->BanTime				+= BanTimeSec; 
	if (pNewClient) 
	{
		BannedAddresses.push_back	(pNewClient);
		BannedList_Save				();
	}
};

void IPureServer::UnBanAddress	(const ip_address& Address)
{
	if (!GetBannedClient(Address)) 
	{
		Msg("! Can't find address %s in ban list.", Address.to_string().c_str() );
		return;
	};

	for	(u32 it=0; it<BannedAddresses.size(); it++)	
	{
		IBannedClient* pBClient = BannedAddresses[it];
		if (pBClient->HAddr == Address) 
		{
			xr_delete				(BannedAddresses[it]);
			BannedAddresses.erase	(BannedAddresses.begin()+it);
			Msg						("Unbanning %s", Address.to_string().c_str() );
			BannedList_Save			();
			break;
		}
	};
};

void IPureServer::Print_Banned_Addreses	()
{
	Msg ("- Banned list");
	for (u32 i=0; i<BannedAddresses.size(); i++)
	{
		IBannedClient* pBClient = BannedAddresses[i];
		Msg("- %s to %s", pBClient->HAddr.to_string().c_str(), pBClient->BannedTimeTo().c_str() );
	}
}

void IPureServer::BannedList_Save	()
{
	string_path					temp;
	FS.update_path				(temp,"$app_data_root$", GetBannedListName());
	
	CInifile					ini(temp,FALSE,FALSE,TRUE);
	
	for	(u32 it=0; it<BannedAddresses.size(); it++)
	{
		IBannedClient* cl	= BannedAddresses[it];
		cl->Save			(ini);
	};
}

void IPureServer::BannedList_Load()
{
	string_path					temp;
	FS.update_path				(temp,"$app_data_root$", GetBannedListName());
	
	CInifile					ini(temp);

	CInifile::RootIt it			= ini.sections().begin();
	CInifile::RootIt it_e		= ini.sections().end();
	
	for( ;it!=it_e; ++it)
	{
		const shared_str& sect_name	= (*it)->Name;
		IBannedClient* Cl			= xr_new<IBannedClient>();
		Cl->Load					(ini, sect_name);
		BannedAddresses.push_back	(Cl);
	}
}

bool banned_client_comparer(IBannedClient* C1, IBannedClient* C2)
{
	return C1->BanTime > C2->BanTime;
}

void IPureServer::UpdateBannedList()
{
	if(!BannedAddresses.size())		return;
	std::sort(BannedAddresses.begin(),BannedAddresses.end(), banned_client_comparer );
	time_t						T;
	time						(&T);
	
	IBannedClient* Cl			= BannedAddresses.back();
	if(Cl->BanTime<T)
	{
		ip_address				Address = Cl->HAddr;
		UnBanAddress			(Address);
	}
}

LPCSTR IPureServer::GetBannedListName()
{
	return "banned_list.ltx";
}
