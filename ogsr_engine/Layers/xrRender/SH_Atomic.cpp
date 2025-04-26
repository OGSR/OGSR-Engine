#include "stdafx.h"

#include "sh_atomic.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

SVS::SVS() : sh(nullptr)
{
}
SVS::~SVS()
{
    DEV->_DeleteVS(this);

    //	Now it is release automatically
    //_RELEASE(signature);

    _RELEASE(sh);
}
SPS::~SPS()
{
    _RELEASE(sh);
    DEV->_DeletePS(this);
}
SGS::~SGS()
{
    _RELEASE(sh);
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

//	SState
SState::~SState()
{
    _RELEASE(state);
    DEV->_DeleteState(this);
}

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
