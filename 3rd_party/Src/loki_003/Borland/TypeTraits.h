////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: August 9, 2002

#ifndef TYPETRAITS_INC_
#define TYPETRAITS_INC_

#include "Typelist.h"

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template IsCustomUnsignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits
//     class template defined below.
// Invocation: IsCustomUnsignedInt<T> where T is any type
// Defines 'value', an enum that is 1 if T is a custom built-in unsigned
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomUnsignedInt
    {
        static const bool value = false;
    };

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomSignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits
//     class template defined below.
// Invocation: IsCustomSignedInt<T> where T is any type
// Defines 'value', an enum that is 1 if T is a custom built-in signed
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomSignedInt
    {
        static const bool value = false;
    };

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomFloat
// Offers a means to integrate nonstandard floating point types with the
//     TypeTraits class template defined below.
// Invocation: IsCustomFloat<T> where T is any type
// Defines 'value', an enum that is 1 if T is a custom built-in
//     floating point type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomFloat
    {
        static const bool value = false;
    };

////////////////////////////////////////////////////////////////////////////////
// Helper types for class template TypeTraits defined below
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        typedef TYPELIST_4(unsigned char, unsigned short int,
           unsigned int, unsigned long int) StdUnsignedInts;
        typedef TYPELIST_4(signed char, short int,
           int, long int) StdSignedInts;
        typedef TYPELIST_3(bool, char, wchar_t) StdOtherInts;
        typedef TYPELIST_3(float, double, long double) StdFloats;

        template <class U> struct PointerTraits
        {
            static const bool result = false;
            typedef NullType PointeeType;
        };

        template <class U> struct PointerTraits<U *>
        {
            static const bool result = true;
            typedef U PointeeType;
        };

        template <class U> struct ReferenceTraits
        {
            static const bool result = false;
            typedef U ReferredType;
        };

        template <class U> struct ReferenceTraits<U &>
        {
            static const bool result = true;
            typedef U ReferredType;
        };

        template <class U> struct PToMTraits
        {
            static const bool result = false;
        };

        template <class U, class V>
        struct PToMTraits<U V::*>
        {
            static const bool result = true;
        };

        template <class U> struct UnConst
        {
            typedef U Result;
            static const bool isConst = false;
        };

        template <class U> struct UnVolatile
        {
            typedef U Result;
            static const bool isVolatile = false;
        };

        template <class U> struct AddReference
        {
            typedef U & Result;
        };

        template <class U> struct AddReference<U &>
        {
            typedef U & Result;
        };

        template <> struct AddReference<void>
        {
            typedef NullType Result;
        };

#ifndef __BORLANDC__

        template <class U> struct UnConst<const U>
        {
            typedef U Result;
            static const bool isConst = true;
        };

        template <class U> struct UnVolatile<volatile U>
        {
            typedef U Result;
            static const bool isVolatile = true;
        };

#endif

    }   // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
// Figures out various properties of any given type
// Invocations (T is a type):
// a) TypeTraits<T>::isPointer
// returns (at compile time) true if T is a pointer type
// b) TypeTraits<T>::PointeeType
// returns the type to which T points is T is a pointer type, NullType otherwise
// a) TypeTraits<T>::isReference
// returns (at compile time) true if T is a reference type
// b) TypeTraits<T>::ReferredType
// returns the type to which T refers is T is a reference type, NullType
// otherwise
// c) TypeTraits<T>::isMemberPointer
// returns (at compile time) true if T is a pointer to member type
// d) TypeTraits<T>::isStdUnsignedInt
// returns (at compile time) true if T is a standard unsigned integral type
// e) TypeTraits<T>::isStdSignedInt
// returns (at compile time) true if T is a standard signed integral type
// f) TypeTraits<T>::isStdIntegral
// returns (at compile time) true if T is a standard integral type
// g) TypeTraits<T>::isStdFloat
// returns (at compile time) true if T is a standard floating-point type
// h) TypeTraits<T>::isStdArith
// returns (at compile time) true if T is a standard arithmetic type
// i) TypeTraits<T>::isStdFundamental
// returns (at compile time) true if T is a standard fundamental type
// j) TypeTraits<T>::isUnsignedInt
// returns (at compile time) true if T is a unsigned integral type
// k) TypeTraits<T>::isSignedInt
// returns (at compile time) true if T is a signed integral type
// l) TypeTraits<T>::isIntegral
// returns (at compile time) true if T is a integral type
// m) TypeTraits<T>::isFloat
// returns (at compile time) true if T is a floating-point type
// n) TypeTraits<T>::isArith
// returns (at compile time) true if T is a arithmetic type
// o) TypeTraits<T>::isFundamental
// returns (at compile time) true if T is a fundamental type
// p) TypeTraits<T>::ParameterType
// returns the optimal type to be used as a parameter for functions that take Ts
// q) TypeTraits<T>::isConst
// returns (at compile time) true if T is a const-qualified type
// r) TypeTraits<T>::NonConstType
// removes the 'const' qualifier from T, if any
// s) TypeTraits<T>::isVolatile
// returns (at compile time) true if T is a volatile-qualified type
// t) TypeTraits<T>::NonVolatileType
// removes the 'volatile' qualifier from T, if any
// u) TypeTraits<T>::UnqualifiedType
// removes both the 'const' and 'volatile' qualifiers from T, if any
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    class TypeTraits
    {
    public:
        static const bool isPointer = Private::PointerTraits<T>::result;
        typedef typename Private::PointerTraits<T>::PointeeType PointeeType;

        static const bool isReference = Private::ReferenceTraits<T>::result;
        typedef typename Private::ReferenceTraits<T>::ReferredType ReferredType;

        static const bool isMemberPointer = Private::PToMTraits<T>::result;

        static const bool isStdUnsignedInt =
            TL::IndexOf<Private::StdUnsignedInts, T>::value >= 0;

        static const bool isStdSignedInt =
            TL::IndexOf<Private::StdSignedInts, T>::value >= 0;

        static const bool isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
            TL::IndexOf<Private::StdOtherInts, T>::value >= 0;
        static const bool isStdFloat = TL::IndexOf<Private::StdFloats, T>::value >= 0;
        static const bool isStdArith = isStdIntegral || isStdFloat;
        static const bool isStdFundamental = isStdArith || isStdFloat ||
            Conversion<T, void>::sameType;

        static const bool isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<T>::value;
        static const bool isSignedInt = isStdSignedInt || IsCustomSignedInt<T>::value;
        static const bool isIntegral = isStdIntegral || isUnsignedInt || isSignedInt;
        static const bool isFloat = isStdFloat || IsCustomFloat<T>::value;
        static const bool isArith = isIntegral || isFloat;
        static const bool isFundamental = isStdFundamental || isArith || isFloat;

        static const bool temp = isStdArith || isPointer || isMemberPointer;

        typedef typename Select< ::Loki::TypeTraits<T>::temp,
            T, typename Private::AddReference<T>::Result>::Result ParameterType;

        static const bool isConst = Private::UnConst<T>::isConst;
        typedef typename Private::UnConst<T>::Result NonConstType;
        static const bool isVolatile = Private::UnVolatile<T>::isVolatile;
        typedef typename Private::UnVolatile<T>::Result NonVolatileType;
        typedef typename Private::UnVolatile<typename Private::UnConst<T>::Result>::Result
            UnqualifiedType;
    };

}   // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// July 16, 2002: Ported by Terje Slettebø and Pavel Vozenilek to BCC 5.6
////////////////////////////////////////////////////////////////////////////////

#endif // TYPETRAITS_INC_
