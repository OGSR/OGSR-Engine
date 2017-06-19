/*
GameSpy GHTTP SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2001 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
*/

#ifndef _GHTTPPROCESS_H_
#define _GHTTPPROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ghttpMain.h"
#include "ghttpConnection.h"

void ghiDoSocketInit	  (GHIConnection * connection);
void ghiDoHostLookup      (GHIConnection * connection);
void ghiDoLookupPending   (GHIConnection * connection);
void ghiDoConnecting      (GHIConnection * connection);
void ghiDoSecuringSession (GHIConnection * connection);
void ghiDoSendingRequest  (GHIConnection * connection);
void ghiDoPosting         (GHIConnection * connection);
void ghiDoWaiting         (GHIConnection * connection);
void ghiDoReceivingStatus (GHIConnection * connection);
void ghiDoReceivingHeaders(GHIConnection * connection);
void ghiDoReceivingFile   (GHIConnection * connection);

#ifdef __cplusplus
}
#endif

#endif
