#include "stdafx.h"
#include "Level.h"
#include "HUDManager.h"
#include "UIGameDM.h"
#include "xrServer.h"

#define DEMO_DATA_SIZE	65536
u32	g_dwDemoDeltaFrame = 1;

void						CLevel::Demo_StoreServerData	(void* data, u32 size)
{
	return;
/* warning 4702 - unreachable code
	DemoCS.Enter();
	Demo_StoreData(data, size, DATA_SERVER_PACKET);
	DemoCS.Leave();
*/
}

void						CLevel::Demo_StoreData			(void* data, u32 size, DEMO_CHUNK DataType)
{
 	if (!IsDemoSave()) return;

//	DemoCS.Enter();
	
	u32 CurTime = timeServer_Async();
	u32 TotalSize = 4 + 4 + 4;//	
	switch(DataType)
	{
	case DATA_FRAME:
		{
			TotalSize += size;
		}break;
	case DATA_SERVER_PACKET:
	case DATA_CLIENT_PACKET:
		{
			TotalSize += size + 4;
		}break;
	}

	R_ASSERT2(size <= DEMO_DATA_SIZE, "Data is too BIG!");
	if ((TotalSize + m_dwStoredDemoDataSize) >= DEMO_DATA_SIZE)
	{
		Demo_DumpData();		
	};

	DEMO_CHUNK	Chunk = DataType;
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &Chunk, 4);				m_dwStoredDemoDataSize += 4;					
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &m_dwCurDemoFrame, 4);	m_dwStoredDemoDataSize += 4;		
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &CurTime, 4);			m_dwStoredDemoDataSize += 4;				
	switch (DataType)
	{
	case DATA_FRAME:
		{
			CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, data, size);		m_dwStoredDemoDataSize += size;		
		}break;
	case DATA_SERVER_PACKET:
	case DATA_CLIENT_PACKET:
		{
			CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &size, 4);		m_dwStoredDemoDataSize += 4;
			CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, data, size);		m_dwStoredDemoDataSize += size;		
		}break;
	}
	
	
//	DemoCS.Leave();

	/*
	FILE* fTDemo = fopen(m_sDemoName, "ab");
	if (!fTDemo) return;

	static u32 Count = 0;
	static u32 TotalSize = 0;
	u32 CurTime = timeServer_Async();
	fwrite(&(CurTime), sizeof(CurTime), 1, fTDemo); TotalSize += sizeof(CurTime);
	fwrite(&(size), sizeof(size), 1, fTDemo);		TotalSize += sizeof(size);
	if (size) fwrite((data), 1, size, fTDemo);		TotalSize += size;
	fclose(fTDemo);
	Count++;
	*/
}

void						CLevel::Demo_DumpData()
{
	if (!m_sDemoName[0]) return;
	FILE* fTDemo = fopen(m_sDemoName, "ab");
	if (fTDemo)
	{
		fwrite(m_pStoredDemoData, m_dwStoredDemoDataSize, 1, fTDemo);
		fclose(fTDemo);
	}

	m_dwStoredDemoDataSize = 0;
}

void						CLevel_DemoCrash_Handler	()
{
	if (!g_pGameLevel) return;
	Level().WriteStoredDemo();
	Level().CallOldCrashHandler();
}

//#define STORE_TDEMO

void						CLevel::Demo_PrepareToStore			()
{
	m_bDemoSaveMode = !!strstr(Core.Params,"-techdemo");

	if (!m_bDemoSaveMode) return;

	VERIFY						(!m_we_used_old_crach_handler);
	m_we_used_old_crach_handler	= true;
	m_pOldCrashHandler			= Debug.get_crashhandler();
	Debug.set_crashhandler		(CLevel_DemoCrash_Handler);
	//---------------------------------------------------------------
	string1024 CName = "";
	u32 CNameSize = 1024;
	GetComputerName(CName, (DWORD*)&CNameSize);
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	sprintf_s(m_sDemoName, "xray_%s_%02d-%02d-%02d_%02d-%02d-%02d.tdemo", CName, Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
	Msg("Tech Demo would be stored in - %s", m_sDemoName);
	
	FS.update_path      (m_sDemoName,"$logs$",m_sDemoName);
	//---------------------------------------------------------------
	m_dwStoredDemoDataSize = 0;
	m_pStoredDemoData = xr_alloc<u8>(DEMO_DATA_SIZE/sizeof(u8));
	//---------------------------------------------------------------
	m_dwCurDemoFrame = 0;
//	ZeroMemory(&m_sDemoHeader, sizeof(m_sDemoHeader));
	m_sDemoHeader.Head[0] = 0;
	m_sDemoHeader.ServerOptions = "";
};

void						CLevel::CallOldCrashHandler			()
{
	if (!m_pOldCrashHandler) return;
	m_pOldCrashHandler();
};

void						CLevel::WriteStoredDemo			()
{	
	if (!DemoCS.TryEnter()) return;
	
	Demo_DumpData();
	DemoCS.Leave();
};

BOOL	g_bLeaveTDemo = FALSE;

void						CLevel::Demo_Clear				()
{
	WriteStoredDemo();
	m_bDemoSaveMode = FALSE;
	xr_free(m_pStoredDemoData);
	m_dwStoredDemoDataSize = 0;	 

	if (!g_bLeaveTDemo)
	{
		DeleteFile(m_sDemoName);
	};
};

void						CLevel::Demo_Load				(LPCSTR DemoName)
{	
	string_path			DemoFileName;
	FS.update_path      (DemoFileName,"$logs$",DemoName);
	//-----------------------------------------------------
	HANDLE hDemoFile = CreateFile(DemoFileName, FILE_ALL_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDemoFile == NULL) return;

	u32	FileSize = GetFileSize(hDemoFile, NULL);
	u8* pDemoData = xr_alloc<u8>(FileSize/sizeof(u8));
	ReadFile(hDemoFile, pDemoData, FileSize, (LPDWORD)&FileSize, NULL);
	CloseHandle(hDemoFile);
	u8* pTDemoData = pDemoData;
	//-----------------------------------------------------

//	FILE* fTDemo = fopen(DemoFileName, "rb");
//	if (fTDemo)
	{
//		NET_Packet NewPacket;
		DemoDataStruct NewData;
		Msg("\n------- Loading Demo... ---------\n");

//		while (!feof(fTDemo))
		while (u32(pTDemoData - pDemoData) < FileSize)
		{
			CopyMemory(&(NewData.m_dwDataType), pTDemoData, sizeof(NewData.m_dwDataType));				pTDemoData += sizeof(NewData.m_dwDataType);
			CopyMemory(&(NewData.m_dwFrame), pTDemoData, sizeof(NewData.m_dwFrame));					pTDemoData += sizeof(NewData.m_dwFrame);
			CopyMemory(&(NewData.m_dwTimeReceive), pTDemoData, sizeof(NewData.m_dwTimeReceive));		pTDemoData += sizeof(NewData.m_dwTimeReceive);

//			fread(&(NewData.m_dwDataType), sizeof(NewData.m_dwDataType), 1, fTDemo);
//			fread(&(NewData.m_dwFrame), sizeof(NewData.m_dwFrame), 1, fTDemo);
//			fread(&(NewData.m_dwTimeReceive), sizeof(NewData.m_dwTimeReceive), 1, fTDemo);
			switch (NewData.m_dwDataType)
			{
			case DATA_FRAME:
				{
					CopyMemory(&(NewData.FrameTime), pTDemoData, sizeof(NewData.FrameTime));			pTDemoData += sizeof(NewData.FrameTime);
//					fread(&(NewData.FrameTime), sizeof(NewData.FrameTime), 1, fTDemo);
					m_dwLastDemoFrame = NewData.m_dwFrame;
				}break;
			case DATA_CLIENT_PACKET:
			case DATA_SERVER_PACKET:
				{
					CopyMemory(&(NewData.Packet.B.count), pTDemoData, sizeof(NewData.Packet.B.count));	pTDemoData += sizeof(NewData.Packet.B.count);
					CopyMemory((NewData.Packet.B.data), pTDemoData, NewData.Packet.B.count);			pTDemoData += NewData.Packet.B.count;

//					fread(&(NewData.Packet.B.count), sizeof(NewData.Packet.B.count), 1, fTDemo);
//					fread((NewData.Packet.B.data), 1, NewData.Packet.B.count, fTDemo);
				}break;
			};			

			m_aDemoData.push_back(NewData);
		}
//		fclose(fTDemo);
		if (!m_aDemoData.empty()) 
		{
			m_bDemoPlayMode = TRUE;
			m_bDemoSaveMode = FALSE;
		};
		m_dwCurDemoFrame = 0;
	}
	xr_free(pDemoData);
	//-----------------------------------------------
	g_dwDemoDeltaFrame = 1;
}

static long lFileSize = 0;
void						CLevel::Demo_Load_toFrame	(LPCSTR FileName, DWORD toFrame, long &ofs)
{
	if (ofs == 1) g_dwDemoDeltaFrame = 1;

	m_sDemoFileName = FileName;
	string_path	DemoFileName;
	FS.update_path      (DemoFileName,"$logs$",FileName);
	//-----------------------------------------------------
	FILE* fTDemo = fopen(DemoFileName, "rb");
	if (!fTDemo) return;
	if (ofs == 0)
	{
		//-------------- get file size ------------------------
		fseek(fTDemo, 0, SEEK_END);
		lFileSize = ftell(fTDemo);
		fseek(fTDemo, 0, SEEK_SET);

		//-------------- load file header ---------------------		
		fread(&m_sDemoHeader.bServerClient, 32, 1, fTDemo);
		DWORD OptSize = 0;
		fread(&OptSize, 4, 1, fTDemo);
		string4096 ServerOptions = "";
		fread(&ServerOptions, OptSize, 1, fTDemo);
		m_sDemoHeader.ServerOptions = ServerOptions;
		ofs = ftell(fTDemo);
		//------------- replace server and client options -----
		m_caServerOptions = m_sDemoHeader.ServerOptions;
		m_caClientOptions = "localhost";
		//-----------------------------------------------------
	}
	if (fseek(fTDemo, ofs, SEEK_SET))
	{
		R_ASSERT(0);
	};
	DemoDataStruct NewData;	
	while (!feof(fTDemo) && (ofs < lFileSize))
	{
		fread(&(NewData.m_dwDataType), sizeof(NewData.m_dwDataType), 1, fTDemo);
		fread(&(NewData.m_dwFrame), sizeof(NewData.m_dwFrame), 1, fTDemo);
		fread(&(NewData.m_dwTimeReceive), sizeof(NewData.m_dwTimeReceive), 1, fTDemo);
		switch (NewData.m_dwDataType)
		{
		case DATA_FRAME:
			{				
				fread(&(NewData.FrameTime), sizeof(NewData.FrameTime), 1, fTDemo);
				m_dwLastDemoFrame = NewData.m_dwFrame;
			}break;
		case DATA_CLIENT_PACKET:
		case DATA_SERVER_PACKET:
			{
				fread(&(NewData.Packet.B.count), sizeof(NewData.Packet.B.count), 1, fTDemo);
				fread((NewData.Packet.B.data), 1, NewData.Packet.B.count, fTDemo);
			}break;
		};			

		ofs = ftell(fTDemo);
		m_aDemoData.push_back(NewData);
		if (NewData.m_dwFrame > toFrame) break;
	};
	
	fclose(fTDemo);

	if (!m_aDemoData.empty()) 
	{
		m_bDemoPlayMode = TRUE;
		m_bDemoSaveMode = FALSE;
	};
};

static DWORD dFrame = 1;
void						CLevel::Demo_Update				()
{
	if (!IsDemoPlay() || m_aDemoData.empty() || !m_bDemoStarted) return;
	if (float(m_lDemoOfs)/lFileSize>0.95f)
	{
		g_dwDemoDeltaFrame = 1;
		dFrame = 1;
	}
	static u32 Pos = 0;

	if (m_bDemoPlayByFrame)
		Demo_Load_toFrame(m_sDemoFileName.c_str(), m_dwCurDemoFrame+dFrame, m_lDemoOfs);

	if (Pos >= m_aDemoData.size()) return;	

	if (!m_bDemoPlayByFrame)
	{
		for (Pos; Pos < m_aDemoData.size(); Pos++)
		{
			u32 CurTime = timeServer_Async();
			DemoDataStruct* P = &(m_aDemoData[Pos]);
			{
				switch (P->m_dwDataType)
				{
				case DATA_SERVER_PACKET:
					break;
				case DATA_CLIENT_PACKET:
					{
						if (P->m_dwTimeReceive <= CurTime) 
						{
							Msg("tReceive [%d] - CurTime [%d]",P->m_dwTimeReceive, CurTime);
							IPureClient::OnMessage(P->Packet.B.data, P->Packet.B.count);
						}
						else 
						{			
							break;
						};
					}break;
				case DATA_FRAME:
					if (P->m_dwTimeReceive <= CurTime) 
					{
						Msg("tsReceive [%d] - CurTime [%d]",P->m_dwTimeReceive, CurTime);
						Server->OnMessage(P->Packet, ClientID());
					}
					else 
					{			
						break;
					};
					break;
				}				
			}		
		};
	}
	else
	{
		while (!m_aDemoData.empty())
		{
			DemoDataStruct* P;
			P = &(m_aDemoData.front());
			if (P->m_dwFrame > m_dwCurDemoFrame) 
			{
				break;
			};
			switch (P->m_dwDataType)
			{
			case DATA_FRAME:
				{
					Device.dwTimeDelta		= P->FrameTime.dwTimeDelta;
					Device.dwTimeGlobal		= P->FrameTime.dwTimeGlobal;
					//					CurFrameTime.dwTimeServer		= Level().timeServer();
					//					CurFrameTime.dwTimeServer_Delta = Level().timeServer_Delta();
					Device.fTimeDelta		= P->FrameTime.fTimeDelta;
					Device.fTimeGlobal		= P->FrameTime.fTimeGlobal;

				}break;
			case DATA_CLIENT_PACKET:
				{
					u16			m_type;					
					P->Packet.r_begin	(m_type);
					IPureClient::OnMessage(P->Packet.B.data, P->Packet.B.count);
				}break;
			case DATA_SERVER_PACKET:
				{
					u16			m_type;					
					P->Packet.r_begin	(m_type);
					Server->OnMessage(P->Packet, ClientID());
				}break;
			}
			m_aDemoData.pop_front();
		};
	};	
	//-------------------------------
	if (HUD().GetUI())
	{
		CUIGameDM* game_dm_ui = smart_cast<CUIGameDM*>( HUD().GetUI()->UIGame());
		if (game_dm_ui)
		{
			if (Pos < m_aDemoData.size())
			{
				string1024 tmp;
				if (m_bDemoPlayByFrame)
				{
//					sprintf_s(tmp, "Demo Playing. %d perc.", u32(float(m_dwCurDemoFrame)/m_dwLastDemoFrame*100.0f));
					if (float(m_lDemoOfs)/lFileSize > 0.9)
					{
						int x=0;
						x=x;
					}
					sprintf_s(tmp, "Demo Playing. %d perc.", u32(float(m_lDemoOfs)/lFileSize*100.0f));
				}
				else
				{
					sprintf_s(tmp, "Demo Playing. %d perc.", u32(float(Pos)/m_aDemoData.size()*100.0f));
				}
				game_dm_ui->SetDemoPlayCaption(tmp);
			}
			else
				game_dm_ui->SetDemoPlayCaption("");
			
		}
	}
	//---------------------------------

//	m_dwCurDemoFrame++;

	if (Pos >= m_aDemoData.size() || m_lDemoOfs>lFileSize)
	{
		Msg("! ------------- Demo Ended ------------");
	};	
};

void						CLevel::Demo_StartFrame			()
{
	if (!IsDemoSave() || !net_IsSyncronised()) return;

	DemoCS.Enter();

	DemoFrameTime CurFrameTime;
	CurFrameTime.dwTimeDelta = Device.dwTimeDelta;
	CurFrameTime.dwTimeGlobal = Device.dwTimeGlobal;
	CurFrameTime.dwTimeServer = Level().timeServer();
	CurFrameTime.dwTimeServer_Delta = Level().timeServer_Delta();
	CurFrameTime.fTimeDelta = Device.fTimeDelta;
	CurFrameTime.fTimeGlobal= Device.fTimeGlobal;

	Demo_StoreData(&CurFrameTime, sizeof(CurFrameTime), DATA_FRAME);

	DemoCS.Leave();
};

void						CLevel::Demo_EndFrame			()
{
	if (IsDemoPlay() && m_bDemoPlayByFrame)
		m_dwCurDemoFrame+=dFrame;		
	else
		m_dwCurDemoFrame++;	

	dFrame = g_dwDemoDeltaFrame;
};

