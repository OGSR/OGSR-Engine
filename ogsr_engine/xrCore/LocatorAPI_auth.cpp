#include "stdafx.h"
#pragma hdrstop

struct	auth_options	{
	xr_vector<xr_string>				ignore;
	xr_vector<xr_string>				important;
};


void	auth_entry		(void* p)
{
	FS.auth_runtime		(p);
}

void	CLocatorAPI::auth_generate		(xr_vector<xr_string>&	ignore, xr_vector<xr_string>&	important)
{
	auth_options*	_o	= xr_new<auth_options>	();
	_o->ignore			= ignore	;
	_o->important		= important	;

#if 1
	FS.auth_runtime		(_o);
#else
	thread_spawn		(auth_entry,"checksum",0,_o);
#endif
}

u64		CLocatorAPI::auth_get			()
{
	m_auth_lock.Enter	()	;
	m_auth_lock.Leave	()	;
	return	m_auth_code		;
}

void	CLocatorAPI::auth_runtime		(void*	params)
{
	m_auth_lock.Enter	()	;
	auth_options*		_o	= (auth_options*)	params	;
	m_auth_code			= 0;
	bool				do_break = false;

#ifdef DEBUG
	bool				b_extern_auth = !!strstr(Core.Params,"asdf");
	if (!b_extern_auth)
#endif // DEBUG
	{
		for (files_it it = files.begin(); it!=files.end(); ++it) {
			const file&	f	=	*it;

			// test for skip
			BOOL	bSkip	=	FALSE;
			for (u32 s=0; s<_o->ignore.size(); s++) {
				if (strstr(f.name,_o->ignore[s].c_str()))	
					bSkip	=	TRUE;
			}

			if (bSkip)
				continue;

			// test for important
			for (s=0; s<_o->important.size(); s++) {
				if ((f.size_real != 0) && strstr(f.name,_o->important[s].c_str())) {
					// crc for file				
					IReader*	r	= FS.r_open	(f.name);
					if (!r) {
						do_break	= true;
						break;
					}
					u32 crc			= crc32		(r->pointer(),r->length());
					
#ifdef DEBUG
					if(strstr(Core.Params,"qwerty"))
						Msg("auth %s = %d",f.name,crc);
#endif // DEBUG

					FS.r_close		(r);
					m_auth_code	^=	u64(crc);
				}
			}
		
			if (do_break)
				break;
		}
#ifdef DEBUG
		Msg					( "auth_code = %d" , m_auth_code );
#endif // DEBUG
	}
#ifdef DEBUG
	else {
		string64			c_auth_code;
		sscanf				(strstr(Core.Params,"asdf ")+5,"%[^ ] ",c_auth_code);
		m_auth_code			= _atoi64(c_auth_code);
	}
#endif // DEBUG
	xr_delete			(_o);
	m_auth_lock.Leave	()	;
}
