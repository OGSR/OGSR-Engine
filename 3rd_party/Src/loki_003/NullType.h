////////////////////////////////
// Generated header: NullType.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/NullType.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/NullType.h"
#	elif (__MWERKS__)
#		include "Reference/NullType.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/NullType.h"
#	elif (_MSC_VER >= 1301)
#		include "Reference/NullType.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/NullType.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/NullType.h"
#	else
#		include "Reference/NullType.h"
#	endif
#endif
