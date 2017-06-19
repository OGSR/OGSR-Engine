#include "stdafx.h"
#pragma hdrstop

#include "sh_atomic.h"
#include "ResourceManager.h"

// Atomic
SVS::~SVS								()			{	_RELEASE(vs);		Device.Resources->_DeleteVS			(this);	}
SPS::~SPS								()			{	_RELEASE(ps);		Device.Resources->_DeletePS			(this);	}
SState::~SState							()			{	_RELEASE(state);	Device.Resources->_DeleteState		(this);	}
SDeclaration::~SDeclaration				()			{	_RELEASE(dcl);		Device.Resources->_DeleteDecl		(this);	}
