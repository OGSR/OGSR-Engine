////////////////////////////////
// Generated header: SmartPtr.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/SmartPtr.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/SmartPtr.h"
#	elif (__MWERKS__)
#		include "Reference/SmartPtr.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/SmartPtr.h"
#	elif (_MSC_VER >= 1301)
#		include "Reference/SmartPtr.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/SmartPtr.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/SmartPtr.h"
#	else
#		include "Reference/SmartPtr.h"
#	endif
#endif
