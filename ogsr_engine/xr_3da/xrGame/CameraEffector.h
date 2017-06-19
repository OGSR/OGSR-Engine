#pragma once

#include "../cameramanager.h"
#include "../effector.h"
#include "../effectorPP.h"

#define eStartEffectorID		50

#define effHit					(eStartEffectorID+1)
#define effAlcohol				(eStartEffectorID+2)
#define effFireHit				(eStartEffectorID+3)
#define effExplodeHit			(eStartEffectorID+4)
#define effNightvision			(eStartEffectorID+5)
#define effPsyHealth			(eStartEffectorID+6)
#define effControllerAura		(eStartEffectorID+7)
#define effControllerAura2		(eStartEffectorID+8)
#define effBigMonsterHit		(eStartEffectorID+9)

#define	eCEFall					((ECamEffectorType)(cefNext+1))
#define	eCENoise				((ECamEffectorType)(cefNext+2))
#define	eCEShot					((ECamEffectorType)(cefNext+3))
#define	eCEZoom					((ECamEffectorType)(cefNext+4))
#define	eCERecoil				((ECamEffectorType)(cefNext+5))
#define	eCEBobbing				((ECamEffectorType)(cefNext+6))
#define	eCEHit					((ECamEffectorType)(cefNext+7))
#define	eCEUser					((ECamEffectorType)(cefNext+11))
#define	eCEControllerPsyHit		((ECamEffectorType)(cefNext+12))
#define	eCEVampire				((ECamEffectorType)(cefNext+13))
#define	eCEPseudoGigantStep		((ECamEffectorType)(cefNext+14))
#define	eCEMonsterHit			((ECamEffectorType)(cefNext+15))
