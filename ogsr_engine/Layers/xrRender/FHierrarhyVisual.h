// FHierrarhyVisual.h: interface for the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FHierrarhyVisualH
#define FHierrarhyVisualH

#pragma once

#include "fbasicvisual.h"

class  FHierrarhyVisual : public dxRender_Visual
{
public:
	xr_vector<dxRender_Visual*>		children;
	BOOL							bDontDelete;
public:
    				FHierrarhyVisual();
	virtual 		~FHierrarhyVisual();

	virtual void	Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void	Copy			(dxRender_Visual *pFrom);
	virtual void	Release			();
	/************************* Add by Zander *******************************/
	virtual u32 	ChildCount 		() const { return children.size(); }	
	virtual void 	SetChildRFlag 	(u32 id, bool v);						
	virtual bool 	GetChildRFlag 	(u32 id) const;							
	/************************* End add *************************************/
	
};

#endif //FHierrarhyVisualH
