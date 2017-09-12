////////////////////////////////
// Generated header: TypeInfo.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/LokiTypeInfo.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/LokiTypeInfo.h"
#	elif (__MWERKS__)
#		include "Reference/LokiTypeInfo.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/LokiTypeInfo.h"
#	elif (_MSC_VER >= 1301)
#		include "Borland/LokiTypeInfo.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/LokiTypeInfo.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/LokiTypeInfo.h"
#	else
#		include "Reference/LokiTypeInfo.h"
#	endif
#endif
