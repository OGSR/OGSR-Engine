// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include "MgcInterp2DVectorField.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DVectorField::Interp2DVectorField (int iQuantity,
    Vector2* akInput, Vector2* akOutput)
{
    // Repackage the output vectors into individual components.  This is
    // required because of the format that the quadratic interpolator expects
    // for its input data.
    Real* afXOutput = new Real[iQuantity];
    Real* afYOutput = new Real[iQuantity];
    for (int i = 0; i < iQuantity; i++)
    {
        afXOutput[i] = akOutput[i].x;
        afYOutput[i] = akOutput[i].y;
    }
    delete[] akOutput;

    // Create interpolator for x-coordinate of vector field.
    m_pkXInterp = new Interp2DQdrNonuniform(iQuantity,akInput,afXOutput);

    // Create interpolator for y-coordinate of vector field, but share the
    // already created triangulation for the x-interpolator.
    m_pkYInterp = new Interp2DQdrNonuniform(*m_pkXInterp,afYOutput);
}
//----------------------------------------------------------------------------
Interp2DVectorField::~Interp2DVectorField ()
{
    delete m_pkXInterp;
    delete m_pkYInterp;
}
//----------------------------------------------------------------------------
bool Interp2DVectorField::Evaluate (const Vector2& rkInput, Vector2& rkOutput)
{
    Real fXDeriv, fYDeriv;
    
    return m_pkXInterp->Evaluate(rkInput,rkOutput.x,fXDeriv,fYDeriv)
        && m_pkYInterp->Evaluate(rkInput,rkOutput.y,fXDeriv,fYDeriv);
}
//----------------------------------------------------------------------------


