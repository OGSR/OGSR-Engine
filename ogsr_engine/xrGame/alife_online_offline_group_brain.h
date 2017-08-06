////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_online_offline_group_brain.h
//	Created 	: 25.10.2005
//  Modified 	: 25.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife Online Offline Group brain class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSE_ALifeOnlineOfflineGroup;

class CALifeOnlineOfflineGroupBrain {
private:
	CSE_ALifeOnlineOfflineGroup	*m_object;

public:
	IC							CALifeOnlineOfflineGroupBrain	(CSE_ALifeOnlineOfflineGroup *object);
};

#include "alife_online_offline_group_brain_inline.h"