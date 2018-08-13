#pragma once
#include "UIWindow.h"
#include "../UIStaticItem.h"
#include "../inventory_space.h"

class CUIXml;
class CArtefact;

class CUIArtefactPanel : public CUIWindow
{
public:
	CUIArtefactPanel() = default;
	~CUIArtefactPanel() { m_vRects.clear_and_free(); }

	virtual void InitIcons		(const TIItemContainer& artefacts);
	virtual void Draw			();
			void InitFromXML	(CUIXml& xml, LPCSTR path, int index);

protected:
	float						m_fScale;
	Fvector2					m_cell_size;
	xr_vector<Frect>            m_vRects;
	CUIStaticItem               m_si;
};