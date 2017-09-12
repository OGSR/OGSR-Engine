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

#ifndef MGCLATTICE_H
#define MGCLATTICE_H

#include "MagicFMLibType.h"
#include "MgcRTLib.h"
#include <fstream>

namespace Mgc {


class MAGICFM Lattice
{
public:
    // abstract base class for TImage
    virtual ~Lattice ();

    // data access
    int GetDimensions () const;
    const int* GetBounds () const;
    int GetBound (int i) const;
    int GetQuantity () const;
    const int* GetOffsets () const;
    int GetOffset (int i) const;

    // assignment
    Lattice& operator= (const Lattice& rkLattice);

    // comparisons
    bool operator== (const Lattice& rkLattice) const;
    bool operator!= (const Lattice& rkLattice) const;

    // Conversions between n-dim and 1-dim structures.  The coordinate arrays
    // must have the same number of elements as the dimensions of the lattice.
    int GetIndex (const int* aiCoord) const;
    void GetCoordinates (int iIndex, int* aiCoord) const;

    // streaming
    bool Load (std::ifstream& rkIStr);
    bool Save (std::ofstream& rkOStr) const;

    static bool LoadRaw (const char* acFilename, int& riDimensions,
        int*& raiBound, int& riQuantity, int& riRTTI, int& riSizeOf,
        char*& racData);

protected:
    // Construction.  Lattice accepts responsibility for deleting the
    // bound array.
    Lattice (int iDimensions, int* aiBound);
    Lattice (const Lattice& rkLattice);
    Lattice ();

    // For deferred creation of bounds.  Lattice accepts responsibility
    // for deleting the bound array.
    Lattice (int iDimensions);
    void SetBounds (int* aiBound);
    void ComputeQuantityAndOffsets ();

    int m_iDimensions;
    int* m_aiBound;
    int m_iQuantity;
    int* m_aiOffset;

    // streaming
    static const char* ms_acHeader;
};

#include "MgcLattice.inl"

} // namespace Mgc

#endif


