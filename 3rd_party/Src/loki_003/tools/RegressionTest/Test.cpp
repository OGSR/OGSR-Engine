///////////////////////////////////////////////////////////////////////////////
// Unit Test for Loki
//
// Copyright Terje Slettebø and Pavel Vozenilek 2002.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.
//
// Last update: October 12, 2002
///////////////////////////////////////////////////////////////////////////////

#ifdef __INTEL_COMPILER
# pragma warning(disable: 111 193 304 383 444 488 981 1418)
#elif defined(_MSC_VER) && !defined(__MWERKS__)
# pragma warning(disable: 4018 4097 4100 4213 4290 4512 4514 4700 4702 4710 4786 4800)
#endif

// Some platforms might have difficulty with this
// Need to ifdef around those cases.
// TODO SGB

#include "UnitTest.h"

// static variable defintion, do not remove

Test::tests_type Test::tests;

// Merely comment out any of the following headers to
// prevent thier execution during the test.
//
// A pluggable-factory-like method is used to 
// auto-register the test, so all that is needed
// is the header inclusion to execute the correspond
// unit test.

#include "TypelistTest.h"
#include "TypeManipTest.h"
#include "TypeTraitsTest.h"
#include "SmallObjectTest.h"
#include "SingletonTest.h"
#include "SmartPtrTest.h"
#include "FactoryTest.h"
#include "AbstractFactoryTest.h"
#include "AssocVectorTest.h"
#include "FunctorTest.h"
#include "DataGeneratorsTest.h"

/*
 * AP - All Pass
 * FC - Fails to Compile
 * ?  - Unknown/Not Tested/Not Recorded
 *
 *               TypelistTest    TypeManipTest  TypeTraitsTest  SmallObjectTest  SingletonTest
 * gcc   2.95.3  ?               ?              ?               ?                ?
 * gcc   3.2     AP              AP             AP              AP               P (Only SingleThreaded)
 * MSVC  6.0     P               AP             FC              FC               AP
 * MSVC  7.0     AP              Conversion     FC              AP               P (Only SingleThreaded) ?
 * Intel 5.0     AP              AP             AP              FC               FC
 * Intel 6.0     AP              AP             AP              FC               P (Only SingleThreaded)
 * Intel 7.0     AP              AP             AP              FC               P (Only SingleThreaded)
 * BCC   5.5     ?               ?              ?               ?                ?
 * BCC   5.6     ?               ?              ?               ?                ?
 * CW    6.0     ?               ?              ?               ?                ?
 *
 *               SmartPtrTest  FactoryTest  AbstractFactoryTest  AssocVectorTest  FunctorTest
 * gcc   2.95.3  ?             ?            ?                    ?                ?
 * gcc   3.2     AP            AP           AP                   AP               AP
 * MSVC  6.0     FC            AP           FC                   FC               FC
 * MSVC  7.0     FC            AP           AP                   FC               AP
 * Intel 5.0     FC            FC           FC                   FC               FC
 * Intel 6.0     FC            AP           AP                   FC               FC
 * Intel 7.0     FC            AP           AP                   FC               FC
 * BCC   5.5     ?             ?            ?                    ?                ?
 * CW    6.0     ?             ?            ?                    ?                ?
 *
 *               DataGeneratorsTest
 * gcc   2.95.3  ?
 * gcc   3.2     AP
 * MSVC  6.0     FC
 * MSVC  7.0     AP
 * Intel 5.0     FC
 * Intel 6.0     AP
 * Intel 7.0     AP
 * BCC   5.5     ?
 * BCC   5.6     ?
 * CW    6.0     ?                 

 */ 

int main()
{

int result = Test::run("Loki Unit Test");

#if defined(__BORLANDC__) || defined(__GNUC__) || defined(_MSC_VER)
  system("pause"); // Stop console window from closing if run from IDE.
#endif

return result;
}
