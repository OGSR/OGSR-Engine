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

//----------------------------------------------------------------------------
inline int InterpBSplineUniform::GetDimension () const
{
    return m_iDims;
}
//----------------------------------------------------------------------------
inline int InterpBSplineUniform::GetDegree () const
{
    return m_iDegree;
}
//----------------------------------------------------------------------------
inline Real InterpBSplineUniform::GetDomainMin (int i) const
{
    assert( 0 <= i && i < m_iDims );
    return m_afDomMin[i];
}
//----------------------------------------------------------------------------
inline Real InterpBSplineUniform::GetDomainMax (int i) const
{
    assert( 0 <= i && i < m_iDims );
    return m_afDomMax[i];
}
//----------------------------------------------------------------------------
inline int InterpBSplineUniform::GetGridMin (int i) const
{
    assert( 0 <= i && i < m_iDims );
    return m_aiGridMin[i];
}
//----------------------------------------------------------------------------
inline int InterpBSplineUniform::GetGridMax (int i) const
{
    assert( 0 <= i && i < m_iDims );
    return m_aiGridMax[i];
}
//----------------------------------------------------------------------------
