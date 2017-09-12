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
template <class T>
TImage<T>::TImage (int iDimensions, int* aiBound, T* atData)
    :
    Lattice(iDimensions,aiBound)
{
    SetData(atData);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (const TImage& rkImage)
    :
    Lattice(rkImage)
{
    m_atData = new T[m_iQuantity];
    memcpy(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T));
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (const char* acFilename)
{
    Load(acFilename);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (int iDimensions)
    :
    Lattice(iDimensions)
{
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::~TImage ()
{
    delete[] m_atData;
}
//----------------------------------------------------------------------------
template <class T>
void TImage<T>::SetData (T* atData)
{
    if ( atData )
    {
        m_atData = atData;
    }
    else
    {
        assert( m_iQuantity > 0 );
        m_atData = new T[m_iQuantity];
        memset(m_atData,0,m_iQuantity*sizeof(T));
    }
}
//----------------------------------------------------------------------------
template <class T>
T* TImage<T>::GetData () const
{
    return m_atData;
}
//----------------------------------------------------------------------------
template <class T>
T& TImage<T>::operator[] (int i) const
{
    // assert:  i < m_iQuantity
    return m_atData[i];
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>& TImage<T>::operator= (const TImage& rkImage)
{
    Lattice::operator=(rkImage);

    delete[] m_atData;
    m_atData = new T[m_iQuantity];
    memcpy(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T));

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>& TImage<T>::operator= (T tValue)
{
    for (int i = 0; i < m_iQuantity; i++)
        m_atData[i] = tValue;

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::operator== (const TImage& rkImage) const
{
    if ( Lattice::operator!=(rkImage) )
        return false;

    return memcmp(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T)) == 0;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::operator!= (const TImage& rkImage) const
{
    return !operator==(rkImage);
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::Load (const char* acFilename)
{
    std::ifstream kIStr(acFilename,std::ios::in|std::ios::binary);
    if ( !kIStr )
    {
        m_iDimensions = 0;
        m_iQuantity = 0;
        m_aiBound = NULL;
        m_aiOffset = NULL;
        m_atData = NULL;
        return false;
    }

    if ( !Lattice::Load(kIStr) )
    {
        m_atData = NULL;
        return false;
    }

    int iRTTI, iSizeOf;
    kIStr.read((char*)&iRTTI,sizeof(int));
    kIStr.read((char*)&iSizeOf,sizeof(int));

    m_atData = new T[m_iQuantity];
    if ( iRTTI == T::GetRTTI() )
    {
        kIStr.read((char*)m_atData,m_iQuantity*sizeof(T));
    }
    else
    {
        char* acFileData = new char[m_iQuantity*iSizeOf];
        kIStr.read(acFileData,m_iQuantity*iSizeOf);
        ImageConvert(m_iQuantity,iRTTI,acFileData,T::GetRTTI(),m_atData);
        delete[] acFileData;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::Save (const char* acFilename) const
{
    std::ofstream kOStr(acFilename,std::ios::out|std::ios::binary);
    if ( !kOStr )
        return false;

    if ( !Lattice::Save(kOStr) )
        return false;

    int iRTTI = T::GetRTTI(), iSizeOf = sizeof(T);
    kOStr.write((const char*)&iRTTI,sizeof(int));
    kOStr.write((const char*)&iSizeOf,sizeof(int));
    kOStr.write((const char*)m_atData,m_iQuantity*sizeof(T));

    return true;
}
//----------------------------------------------------------------------------


