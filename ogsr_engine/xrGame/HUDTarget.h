#pragma once

#include "HUDCrosshair.h"
#include "..\xrcdb\xr_collide_defs.h"

class CHUDManager;

class CHUDTarget {
private:
	friend class CHUDManager;

private:
	typedef collide::rq_result		rq_result;
	typedef collide::rq_results		rq_results;

private:
	ui_shader		hShader;
	float			fuzzyShowInfo;
	rq_result		RQ;
	rq_results		RQR;

private:
	bool			m_bShowCrosshair;
	CHUDCrosshair	HUDCrosshair;

private:
			void	net_Relcase		(CObject* O);

public:
					CHUDTarget		();
			void	CursorOnFrame	();
			void	Render			();
			void	Load			();
			float	GetDist			();
			CObject	*GetObj			();
};
