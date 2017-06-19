#include "stdafx.h"
#pragma hdrstop

#include "xrstring.h"

XRCORE_API	extern		str_container*	g_pStringContainer	= NULL;

#define		HEADER		12			// ref + len + crc

str_value*	str_container::dock		(str_c value)
{
	if (0==value)				return 0;

	cs.Enter					();
#ifdef DEBUG_MEMORY_MANAGER
	Memory.stat_strdock			++	;
#endif // DEBUG_MEMORY_MANAGER

	str_value*	result			= 0	;

	// calc len
	u32		s_len				= xr_strlen(value);
	u32		s_len_with_zero		= (u32)s_len+1;
	VERIFY	(HEADER+s_len_with_zero < 4096);

	// setup find structure
	string16	header;
	str_value*	sv				= (str_value*)header;
	sv->dwReference				= 0;
	sv->dwLength				= s_len;
	sv->dwCRC					= crc32	(value,s_len);
	
	// search
	cdb::iterator	I			= container.find	(sv);	// only integer compares :)
	if (I!=container.end())		{
		// something found - verify, it is exactly our string
		cdb::iterator	save	= I;
		for (; I!=container.end() && (*I)->dwCRC == sv->dwCRC; ++I)	{
			str_value*	V		= (*I);
			if	(V->dwLength!=sv->dwLength)			continue;
			if	(0!=memcmp(V->value,value,s_len))	continue;
			result				= V;				// found
			break;
		}
	}

	// it may be the case, string is not fount or has "non-exact" match
	if (0==result)				{
		// Insert string
//		DUMP_PHASE;

		result					= (str_value*)Memory.mem_alloc(HEADER+s_len_with_zero
#ifdef DEBUG_MEMORY_NAME
			, "storage: sstring"
#endif // DEBUG_MEMORY_NAME
			);

//		DUMP_PHASE;

		result->dwReference		= 0;
		result->dwLength		= sv->dwLength;
		result->dwCRC			= sv->dwCRC;
		CopyMemory				(result->value,value,s_len_with_zero);
		container.insert		(result);
	}
	cs.Leave					();

	return	result;
}

void		str_container::clean	()
{
	cs.Enter	();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	for (; it!=end; )	{
		str_value*	sv = *it;
		if (0==sv->dwReference)	
		{
			cdb::iterator	i_current	= it;
			cdb::iterator	i_next		= ++it;
			xr_free			(sv);
			container.erase	(i_current);
			it							= i_next;
		} else {
			it++;
		}
	}
	if (container.empty())	container.clear	();
	cs.Leave	();
}

void		str_container::verify	()
{
	cs.Enter	();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	for (; it!=end; ++it)	{
		str_value*	sv		= *it;
		u32			crc		= crc32	(sv->value,sv->dwLength);
		string32	crc_str;
		R_ASSERT3	(crc==sv->dwCRC, "CorePanic: read-only memory corruption (shared_strings)", itoa(sv->dwCRC,crc_str,16));
		R_ASSERT3	(sv->dwLength == xr_strlen(sv->value), "CorePanic: read-only memory corruption (shared_strings, internal structures)", sv->value);
	}
	cs.Leave	();
}

void		str_container::dump	()
{
	cs.Enter	();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	FILE* F		= fopen("x:\\$str_dump$.txt","w");
	for (; it!=end; it++)
		fprintf		(F,"ref[%4d]-len[%3d]-crc[%8X] : %s\n",(*it)->dwReference,(*it)->dwLength,(*it)->dwCRC,(*it)->value);
	fclose		(F);
	cs.Leave	();
}

u32			str_container::stat_economy		()
{
	cs.Enter	();
	cdb::iterator	it		= container.begin	();
	cdb::iterator	end		= container.end		();
	int				counter	= 0;
	counter			-= sizeof(*this);
	counter			-= sizeof(cdb::allocator_type);
	const int		node_size = 20;
	for (; it!=end; it++)	{
		counter		-= HEADER;
		counter		-= node_size;
		counter		+= int((int((*it)->dwReference) - 1)*int((*it)->dwLength + 1));
	}
	cs.Leave		();

	return			u32(counter);
}

str_container::~str_container		()
{
	clean	();
	//R_ASSERT(container.empty());
}
