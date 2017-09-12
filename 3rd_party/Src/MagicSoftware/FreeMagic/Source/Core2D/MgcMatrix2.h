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

#ifndef MGCMATRIX2_H
#define MGCMATRIX2_H

#include "MagicFMLibType.h"
#include "MgcVector2.h"

namespace Mgc {

// NOTE.  Rotation matrices are of the form
//   R = cos(t) -sin(t)
//       sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the xy-plane.


class MAGICFM Matrix2
{
public:
    // construction
    Matrix2 ();
    Matrix2 (const Real aafEntry[2][2]);
    Matrix2 (const Matrix2& rkMatrix);
    Matrix2 (Real fEntry00, Real fEntry01,
             Real fEntry10, Real fEntry11);

    // member access, allows use of construct mat[r][c]
    Real* operator[] (int iRow) const;
    operator Real* ();
    Vector2 GetColumn (int iCol) const;

    // assignment and comparison
    Matrix2& operator= (const Matrix2& rkMatrix);
    bool operator== (const Matrix2& rkMatrix) const;
    bool operator!= (const Matrix2& rkMatrix) const;

    // arithmetic operations
    Matrix2 operator+ (const Matrix2& rkMatrix) const;
    Matrix2 operator- (const Matrix2& rkMatrix) const;
    Matrix2 operator* (const Matrix2& rkMatrix) const;
    Matrix2 operator- () const;

    // matrix * vector [2x2 * 2x1 = 2x1]
    Vector2 operator* (const Vector2& rkVector) const;

    // vector * matrix [1x2 * 2x2 = 1x2]
    MAGICFM friend Vector2 operator* (const Vector2& rkVector,
        const Matrix2& rkMatrix);

    // matrix * scalar
    Matrix2 operator* (Real fScalar) const;

    // scalar * matrix
    MAGICFM friend Matrix2 operator* (Real fScalar,
        const Matrix2& rkMatrix);

    // utilities
    Matrix2 Transpose () const;
    bool Inverse (Matrix2& rkInverse, Real fTolerance = 1e-06f) const;
    Matrix2 Inverse (Real fTolerance = 1e-06f) const;
    Real Determinant () const;

    // singular value decomposition
    void SingularValueDecomposition (Matrix2& rkL, Vector2& rkS,
        Matrix2& rkR) const;
    void SingularValueComposition (const Matrix2& rkL, const Vector2& rkS,
        const Matrix2& rkR);

    // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
    void Orthonormalize ();

    // orthogonal Q, diagonal D, upper triangular U stored as (u01)
    void QDUDecomposition (Matrix2& rkQ, Vector2& rkD, Real& rfU) const;

    Real SpectralNorm () const;
    Real L2NormSqr () const;

    // matrix must be orthonormal
    void ToAngle (Real& rfRadians) const;
    void FromAngle (Real fRadians);

    // eigensolver, matrix must be symmetric
    void EigenSolveSymmetric (Real afEigenvalue[2],
        Vector2 akEigenvector[2]) const;

    static void TensorProduct (const Vector2& rkU, const Vector2& rkV,
        Matrix2& rkProduct);

    static const Real EPSILON;
    static const Matrix2 ZERO;
    static const Matrix2 IDENTITY;

protected:
    // support for eigensolver
    void Tridiagonal (Real afDiag[2], Real afSubDiag[2]);
    bool QLAlgorithm (Real afDiag[2], Real afSubDiag[2]);

    Real m_aafEntry[2][2];
};

#include "MgcMatrix2.inl"

} // namespace Mgc

#endif


