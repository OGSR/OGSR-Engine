#include "stdafx.h"
#include "Stalker_net.h"
#include "GameSpyFuncs.h"
#include "ClientDlg.h"

void SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
	CClientDlg* pClDialog = (CClientDlg*) instance;
	if (!pClDialog) return;

	CString address;
	if(server)
		address.Format("%s:%d", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));


	switch (reason)
	{
	case sbc_serveradded : //a server was added to the list, may just have an IP & port at this point
		{
			OutputDebugString("sbc_serveradded \n");
			pClDialog->AddServerToList(server);
		}break;
	case sbc_serverupdated : //server information has been updated - either basic or full information is now available about this server
		{
			OutputDebugString("sbc_serverupdated \n");
			pClDialog->AddServerToList(server);
		}break;
	case sbc_serverupdatefailed : //an attempt to retrieve information about this server, either directly or from the master, failed
		{
			OutputDebugString("sbc_serverupdatefailed \n");
//			pClDialog->RemoveServerFromList(server);
		}break;
	case sbc_serverdeleted : //a server was removed from the list
		{
			OutputDebugString("sbc_serverdeleted \n");
//			pClDialog->RemoveServerFromList(server);
		}break;
	case sbc_updatecomplete : //the server query engine is now idle 
		{
			OutputDebugString("sbc_updatecomplete \n");			
			pClDialog->OnGameSpyUpdateComplete();
		}break;
	case sbc_queryerror		://the master returned an error string for the provided query
		{
			OutputDebugString("sbc_queryerror \n");
		}break;
	};
};