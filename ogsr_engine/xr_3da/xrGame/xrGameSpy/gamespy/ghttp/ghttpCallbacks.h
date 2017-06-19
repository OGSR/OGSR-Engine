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

#ifndef _GHTTPCALLBACKS_H_
#define _GHTTPCALLBACKS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ghttpMain.h"
#include "ghttpConnection.h"

// Call the completed callback for this connection.
///////////////////////////////////////////////////
void ghiCallCompletedCallback
(
	GHIConnection * connection
);

// Call the progress callback for this connection.
//////////////////////////////////////////////////
void ghiCallProgressCallback
(
	GHIConnection * connection,
	const char * buffer,
	GHTTPByteCount bufferLen
);

// Call the post callback for this connection.
//////////////////////////////////////////////
void ghiCallPostCallback
(
	GHIConnection * connection
);

#ifdef __cplusplus
}
#endif

#endif
