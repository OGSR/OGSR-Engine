#pragma once
#include "UIWindow.h"
#include "../UIStaticItem.h"
class CUIXml;
class CArtefact;

class CUIArtefactPanel : public CUIWindow
{
	typedef xr_vector<Frect>::const_iterator ITr;
	typedef xr_vector<CUIStaticItem*>::const_iterator ITsi;

public:
	CUIArtefactPanel			();
	~CUIArtefactPanel			();

	virtual void InitIcons		(const xr_vector<const CArtefact*>& artefacts);
	virtual void Draw			();
			void InitFromXML	(CUIXml& xml, LPCSTR path, int index);

protected:
	float						m_fScale;
	Fvector2					m_cell_size;
	xr_vector<Frect>            m_vRects;
	CUIStaticItem               m_si;
};