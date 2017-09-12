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

#ifndef MGCTUNORDEREDSET_H
#define MGCTUNORDEREDSET_H

// An unordered set of objects stored in contiguous memory.  The type T must
// have the following member functions:
//   T::T();
//   T::~T();
//   T& operator= (const T&);
//   bool operator== (const T&) const;

#include <cassert>
#include <cstddef>

namespace Mgc {

template <class T>
class TUnorderedSet
{
public:
    TUnorderedSet (int iMaxQuantity = 0, int iGrow = 0);
    TUnorderedSet (const TUnorderedSet& rkSet);
    ~TUnorderedSet ();

    void Reset (int iMaxQuantity = 0, int iGrow = 0);
    void Clear ();
    TUnorderedSet& operator= (const TUnorderedSet& rkSet);

    int GetMaxQuantity () const;
    int GetGrow () const;

    int GetQuantity () const;
    const T& Get (int i) const;
    T& operator[] (int i);

    bool Exists (const T& rtElement) const;
    bool Insert (const T& rtElement);
    int Append (const T& rtElement);
    bool Remove (const T& rtElement, int* piOld = NULL, int* piNew = NULL);
    bool RemoveAt (int i, int* piOld = NULL, int* piNew = NULL);
    void Compactify ();

    enum { DEFAULT_GROW = 4 };

protected:
    int m_iQuantity, m_iMaxQuantity, m_iGrow;
    T* m_atElement;
};

#include "MgcTUnorderedSet.inl"

} // namespace Mgc

#endif
