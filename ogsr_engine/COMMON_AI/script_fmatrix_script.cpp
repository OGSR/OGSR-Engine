////////////////////////////////////////////////////////////////////////////
//	Module 		: script_fmatrix_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float matrix script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_fmatrix.h"

using namespace luabind;
void get_matrix_hpb(Fmatrix* self, float* h, float* p, float* b) { self->getHPB(*h, *p, *b); }
void matrix_transform(Fmatrix* self, Fvector* v) { self->transform(*v); }

#pragma optimize("s", on)
void CScriptFmatrix::script_register(lua_State* L)
{
    module(
        L)[class_<Fmatrix>("matrix")
               .def_readwrite("i", &Fmatrix::i)
               .def_readwrite("_14_", &Fmatrix::_14_)
               .def_readwrite("j", &Fmatrix::j)
               .def_readwrite("_24_", &Fmatrix::_24_)
               .def_readwrite("k", &Fmatrix::k)
               .def_readwrite("_34_", &Fmatrix::_34_)
               .def_readwrite("c", &Fmatrix::c)
               .def_readwrite("_44_", &Fmatrix::_44_)
               .def(constructor<>())

               .def("set", (Fmatrix & (Fmatrix::*)(const Fmatrix&))(&Fmatrix::set), return_reference_to<1>())
               .def("set", (Fmatrix & (Fmatrix::*)(const Fvector&, const Fvector&, const Fvector&, const Fvector&))(&Fmatrix::set), return_reference_to<1>())
               .def("identity", &Fmatrix::identity, return_reference_to<1>())
               .def("mk_xform", &Fmatrix::mk_xform, return_reference_to<1>())
               .def("mul", (Fmatrix & (Fmatrix::*)(const Fmatrix&, const Fmatrix&))(&Fmatrix::mul), return_reference_to<1>())
               .def("mul", (Fmatrix & (Fmatrix::*)(const Fmatrix&, float))(&Fmatrix::mul), return_reference_to<1>())
               .def("mul", (Fmatrix & (Fmatrix::*)(float))(&Fmatrix::mul), return_reference_to<1>())
               .def("div", (Fmatrix & (Fmatrix::*)(const Fmatrix&, float))(&Fmatrix::div), return_reference_to<1>())
               .def("div", (Fmatrix & (Fmatrix::*)(float))(&Fmatrix::div), return_reference_to<1>())

               .def("setHPB", &Fmatrix::setHPB, return_reference_to<1>())
               .def("setXYZ", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::setXYZ), return_reference_to<1>())
               .def("setXYZi", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::setXYZi), return_reference_to<1>())
               .def("getHPB", &get_matrix_hpb)
        	   .def("getXYZi", (void (Fmatrix::*)(float &, float &, float &) const)(&Fmatrix::getXYZi))
    ];
}
