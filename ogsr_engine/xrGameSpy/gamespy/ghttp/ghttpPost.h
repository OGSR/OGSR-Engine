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

#ifndef _GHTTPPOST_H_
#define _GHTTPPOST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ghttp.h"
#include "ghttpBuffer.h"
#include "../darray.h"

typedef enum
{
	GHIPostingError,
	GHIPostingDone,
	GHIPostingPosting,
	GHIPostingWaitForContinue
} GHIPostingResult;

typedef struct GHIPostingState
{
	DArray states;
	int index;
	int bytesPosted;
	int totalBytes;
	ghttpPostCallback callback;
	void * param;
	GHTTPBool waitPostContinue; // does DIME need to wait for continue?
	GHTTPBool completed; // prevent re-post in the event of a redirect.
} GHIPostingState;

GHTTPPost ghiNewPost
(
	void
);

void ghiPostSetAutoFree
(
	GHTTPPost post,
	GHTTPBool autoFree
);

GHTTPBool ghiIsPostAutoFree
(
	GHTTPPost post
);

void ghiFreePost
(
	GHTTPPost post
);

GHTTPBool ghiPostAddString
(
	GHTTPPost post,
	const char * name,
	const char * string
);

GHTTPBool ghiPostAddFileFromDisk
(
	GHTTPPost post,
	const char * name,
	const char * filename,
	const char * reportFilename,
	const char * contentType
);

GHTTPBool ghiPostAddFileFromMemory
(
	GHTTPPost post,
	const char * name,
	const char * buffer,
	int bufferLen,
	const char * reportFilename,
	const char * contentType
);

GHTTPBool ghiPostAddXml
(
	GHTTPPost post,
	GSXmlStreamWriter xmlSoap
);

void ghiPostSetCallback
(
	GHTTPPost post,
	ghttpPostCallback callback,
	void * param
);

const char * ghiPostGetContentType
(
	struct GHIConnection * connection
);

GHTTPBool ghiPostInitState
(
	struct GHIConnection * connection
);

void ghiPostCleanupState
(
	struct GHIConnection * connection
);

GHIPostingResult ghiPostDoPosting
(
	struct GHIConnection * connection
);

#ifdef __cplusplus
}
#endif

#endif
