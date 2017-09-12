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

#ifndef VETABLE_H
#define VETABLE_H

#include "Vertex3.h"
#include "Triangle3.h"

class VETable
{
public:
    VETable ();

    bool IsValidVertex (int i) const;
    float GetX (int i) const;
    float GetY (int i) const;
    float GetZ (int i) const;
    void Insert (int i, float fX, float fY, float fZ);
    void Insert (int i0, int i1);
    void RemoveTriangles (V3Array& rkVA, T3Array& rkTA);

protected:
    void RemoveVertex (int i);
    bool Remove (Triangle3& rkT);

    class Vertex
    {
    public:
        Vertex ()
        {
            m_iAdjQuantity = 0;
            m_bValid = false;
        }

        float m_fX, m_fY, m_fZ;
        int m_iAdjQuantity;
        int m_aiAdj[4];
        bool m_bValid;
    };

    Vertex m_akVertex[18];
};

#endif
