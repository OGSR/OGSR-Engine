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

               .def("build_camera_dir", &Fmatrix::build_camera_dir, return_reference_to<1>())
               .def("build_projection", &Fmatrix::build_projection, return_reference_to<1>())

               .def("mulA_43", &Fmatrix::mulA_43, return_reference_to<1>())
               .def("mulA_44", &Fmatrix::mulA_44, return_reference_to<1>())
               .def("mulB_43", &Fmatrix::mulB_43, return_reference_to<1>())
               .def("mulB_44", &Fmatrix::mulB_44, return_reference_to<1>())
               .def("mul_43", &Fmatrix::mul_43, return_reference_to<1>())
               .def("translate", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::translate), return_reference_to<1>())
               .def("translate", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::translate), return_reference_to<1>())
               .def("translate_add", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::translate_add), return_reference_to<1>())
               .def("translate_add", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::translate_add), return_reference_to<1>())
               .def("translate_over", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::translate_over), return_reference_to<1>())
               .def("translate_over", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::translate_over), return_reference_to<1>())

               .def("mul", (Fmatrix & (Fmatrix::*)(const Fmatrix&, const Fmatrix&))(&Fmatrix::mul), return_reference_to<1>())
               .def("mul", (Fmatrix & (Fmatrix::*)(const Fmatrix&, float))(&Fmatrix::mul), return_reference_to<1>())
               .def("mul", (Fmatrix & (Fmatrix::*)(float))(&Fmatrix::mul), return_reference_to<1>())

               .def("invert", (Fmatrix & (Fmatrix::*)())(&Fmatrix::invert), return_reference_to<1>())
               .def("invert", (Fmatrix & (Fmatrix::*)(const Fmatrix&))(&Fmatrix::invert), return_reference_to<1>())
               .def("invert_b", &Fmatrix::invert_b, return_reference_to<1>())

               .def("div", (Fmatrix & (Fmatrix::*)(const Fmatrix&, float))(&Fmatrix::div), return_reference_to<1>())
               .def("div", (Fmatrix & (Fmatrix::*)(float))(&Fmatrix::div), return_reference_to<1>())

               .def("scale", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::scale), return_reference_to<1>())
               .def("scale", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::scale), return_reference_to<1>())

               .def("setHPB", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::setHPB), return_reference_to<1>())
               .def("setHPB", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::setHPB), return_reference_to<1>())
               .def("setXYZ", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::setXYZ), return_reference_to<1>())
               .def("setXYZ", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::setXYZ), return_reference_to<1>())
               .def("setXYZi", (Fmatrix & (Fmatrix::*)(float, float, float))(&Fmatrix::setXYZi), return_reference_to<1>())
               .def("setXYZi", (Fmatrix & (Fmatrix::*)(const Fvector&))(&Fmatrix::setXYZi), return_reference_to<1>())
               .def("getHPB", &get_matrix_hpb)
               .def("getXYZi", (void (Fmatrix::*)(float &, float &, float &) const)(&Fmatrix::getXYZi))
    ];
}
