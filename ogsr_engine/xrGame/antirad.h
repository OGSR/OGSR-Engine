///////////////////////////////////////////////////////////////
// Antirad.h
// Antirad - таблетки выводящие радиацию
///////////////////////////////////////////////////////////////


#pragma once

#include "eatable_item_object.h"

class CAntirad: public CEatableItemObject {
private:
    typedef	CEatableItemObject inherited;

public:
				 CAntirad			();
	virtual		 ~CAntirad			();
	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();
	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	(bool just_before_destroy);
};
