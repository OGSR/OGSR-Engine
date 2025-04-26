#pragma once

#include "IRenderDetailModel.h"

class ECORE_API CDetail : public IRender_DetailModel
{
    void LoadGeom();

public:
    ref_geom DetailGeom;
    ID3DVertexBuffer* DetailVb{};
    ID3DIndexBuffer* DetailIb{};

    void Load(IReader* S);
    void Optimize() const;
    virtual void Unload();

    virtual void transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset);
    virtual void transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, float du, float dv);

    virtual ~CDetail();
};
