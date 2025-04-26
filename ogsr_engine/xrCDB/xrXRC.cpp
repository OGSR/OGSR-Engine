// xrXRC.cpp: implementation of the xrXRC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrXRC.h"

XRCDB_API xrXRC XRC;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStatTimer _clRAY; // total: ray-testing
CStatTimer _clBOX; // total: box query
CStatTimer _clFRUSTUM; // total: frustum query
CStatTimer* cdb_clRAY = &_clRAY; // total: ray-testing
CStatTimer* cdb_clBOX = &_clBOX; // total: box query
CStatTimer* cdb_clFRUSTUM = &_clFRUSTUM; // total: frustum query

xrXRC::xrXRC() {}

xrXRC::~xrXRC() {}
