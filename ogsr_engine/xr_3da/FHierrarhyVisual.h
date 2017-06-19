// FHierrarhyVisual.h: interface for the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FHierrarhyVisualH
#define FHierrarhyVisualH

#pragma once

#include "fbasicvisual.h"

class ENGINE_API FHierrarhyVisual : public IRender_Visual
{
public:
	xr_vector<IRender_Visual*>		children;
	BOOL							bDontDelete;
public:
    				FHierrarhyVisual();
	virtual 		~FHierrarhyVisual();

	virtual void	Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void	Copy			(IRender_Visual *pFrom);
	virtual void	Release			();
};

#endif //FHierrarhyVisualH
