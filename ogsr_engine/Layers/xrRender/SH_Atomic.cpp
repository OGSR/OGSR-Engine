#include "stdafx.h"


#include "sh_atomic.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

// Atomic
// SVS::~SVS								()			{	_RELEASE(vs);		dxRenderDeviceRender::Instance().Resources->_DeleteVS			(this);	}
// SPS::~SPS								()			{	_RELEASE(ps);		dxRenderDeviceRender::Instance().Resources->_DeletePS			(this);	}
// SState::~SState							()			{	_RELEASE(state);	dxRenderDeviceRender::Instance().Resources->_DeleteState		(this);	}
// SDeclaration::~SDeclaration				()			{	_RELEASE(dcl);		dxRenderDeviceRender::Instance().Resources->_DeleteDecl		(this);	}

///////////////////////////////////////////////////////////////////////
//	SVS
SVS::SVS() : vs(0) {}

SVS::~SVS()
{
    DEV->_DeleteVS(this);
    _RELEASE(vs);
}

///////////////////////////////////////////////////////////////////////
//	SPS
SPS::~SPS()
{
    _RELEASE(ps);
    DEV->_DeletePS(this);
}

///////////////////////////////////////////////////////////////////////
//	SGS
SGS::~SGS()
{
    _RELEASE(gs);
    DEV->_DeleteGS(this);
}

SHS::~SHS()
{
    _RELEASE(sh);
    DEV->_DeleteHS(this);
}
SDS::~SDS()
{
    _RELEASE(sh);
    DEV->_DeleteDS(this);
}
SCS::~SCS()
{
    _RELEASE(sh);
    DEV->_DeleteCS(this);
}

///////////////////////////////////////////////////////////////////////
//	SInputSignature
SInputSignature::SInputSignature(ID3DBlob* pBlob)
{
    VERIFY(pBlob);
    signature = pBlob;
    signature->AddRef();
};
SInputSignature::~SInputSignature()
{
    _RELEASE(signature);
    DEV->_DeleteInputSignature(this);
}

///////////////////////////////////////////////////////////////////////
//	SState
SState::~SState()
{
    _RELEASE(state);
    DEV->_DeleteState(this);
}

///////////////////////////////////////////////////////////////////////
//	SDeclaration
SDeclaration::~SDeclaration()
{
    DEV->_DeleteDecl(this);
    xr_map<ID3DBlob*, ID3DInputLayout*>::iterator iLayout;
    iLayout = vs_to_layout.begin();
    for (; iLayout != vs_to_layout.end(); ++iLayout)
    {
        //	Release vertex layout
        _RELEASE(iLayout->second);
    }
}
