#pragma once
#include "customzone.h"

class CRadioactiveZone : public CCustomZone
{
private:
	typedef	CCustomZone	inherited;
public:
	CRadioactiveZone(void);
	virtual ~CRadioactiveZone(void);

	virtual void Load			(LPCSTR section);
	virtual void Affect			(SZoneObjectInfo* O);
	virtual bool EnableEffector	()	{return true;}

	virtual		void	feel_touch_new					(CObject* O	);
	virtual		void	UpdateWorkload					(u32	dt	);				// related to fast-mode optimizations
	virtual		BOOL	feel_touch_contact				(CObject* O	);

protected:
	virtual bool BlowoutState	();
};
