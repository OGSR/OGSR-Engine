////////////////////////////////////////////////////////////////////////////
//	Module 		: script_fvector_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_fvector.h"

using namespace luabind;

#pragma optimize("s", on)
void CScriptFvector::script_register(lua_State* L)
{
    module(L)[
//Для инициализации векторов нулями
           def("vector", [] { return Fvector{}; }), 
           def("vector2", [] { return Fvector2{}; }), 
           def("vector4", [] { return Fvector4{}; }),

           class_<Fvector>("___VECTOR")
               .def_readwrite("x", &Fvector::x)
               .def_readwrite("y", &Fvector::y)
               .def_readwrite("z", &Fvector::z)
               .def(constructor<>())
#
               .def("set", (Fvector & (Fvector::*)(float, float, float))(&Fvector::set), return_reference_to<1>())
               .def("set", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::set), return_reference_to<1>())
               .def("add", (Fvector & (Fvector::*)(float))(&Fvector::add), return_reference_to<1>())
               .def("add", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::add), return_reference_to<1>())
               .def("add", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::add), return_reference_to<1>())
               .def("add", (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::add), return_reference_to<1>())
               .def("sub", (Fvector & (Fvector::*)(float))(&Fvector::sub), return_reference_to<1>())
               .def("sub", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::sub), return_reference_to<1>())
               .def("sub", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::sub), return_reference_to<1>())
               .def("sub", (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::sub), return_reference_to<1>())
               .def("mul", (Fvector & (Fvector::*)(float))(&Fvector::mul), return_reference_to<1>())
               .def("mul", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::mul), return_reference_to<1>())
               .def("mul", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::mul), return_reference_to<1>())
               .def("mul", (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::mul), return_reference_to<1>())
               .def("div", (Fvector & (Fvector::*)(float))(&Fvector::div), return_reference_to<1>())
               .def("div", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::div), return_reference_to<1>())
               .def("div", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::div), return_reference_to<1>())
               .def("div", (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::div), return_reference_to<1>())
               .def("invert", (Fvector & (Fvector::*)())(&Fvector::invert), return_reference_to<1>())
               .def("invert", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::invert), return_reference_to<1>())
               .def("min", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::min), return_reference_to<1>())
               .def("min", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::min), return_reference_to<1>())
               .def("max", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::max), return_reference_to<1>())
               .def("max", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::max), return_reference_to<1>())
               .def("abs", &Fvector::abs, return_reference_to<1>())
               .def("similar", &Fvector::similar)
               .def("set_length", &Fvector::set_length, return_reference_to<1>())
               .def("align", &Fvector::align, return_reference_to<1>())

               .def("clamp", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::clamp), return_reference_to<1>())
               .def("clamp", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::clamp), return_reference_to<1>())
               .def("inertion", &Fvector::inertion, return_reference_to<1>())
               .def("average", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::average), return_reference_to<1>())
               .def("average", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::average), return_reference_to<1>())
               .def("lerp", &Fvector::lerp, return_reference_to<1>())
               .def("mad", (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::mad), return_reference_to<1>())
               .def("mad", (Fvector & (Fvector::*)(const Fvector&, const Fvector&, float))(&Fvector::mad), return_reference_to<1>())
               .def("mad", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::mad), return_reference_to<1>())
               .def("mad", (Fvector & (Fvector::*)(const Fvector&, const Fvector&, const Fvector&))(&Fvector::mad), return_reference_to<1>())

               .def("magnitude", &Fvector::magnitude)

               .def("normalize", (Fvector & (Fvector::*)())(&Fvector::normalize_safe), return_reference_to<1>())
               .def("normalize", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::normalize_safe), return_reference_to<1>())
               .def("normalize_safe", (Fvector & (Fvector::*)())(&Fvector::normalize_safe), return_reference_to<1>())
               .def("normalize_safe", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::normalize_safe), return_reference_to<1>())

               .def("dotproduct", &Fvector::dotproduct)
               .def("crossproduct", &Fvector::crossproduct, return_reference_to<1>())
               .def("distance_to_xz", &Fvector::distance_to_xz)
               .def("distance_to_sqr", &Fvector::distance_to_sqr)
               .def("distance_to", &Fvector::distance_to)

               .def("setHP", &Fvector::setHP, return_reference_to<1>())

               .def("getH", &Fvector::getH)
               .def("getP", &Fvector::getP)

               .def("reflect", &Fvector::reflect, return_reference_to<1>())
               .def("slide", &Fvector::slide, return_reference_to<1>()),

           class_<Fvector2>("___VECTOR2")
               .def_readwrite("x", &Fvector2::x)
               .def_readwrite("y", &Fvector2::y)
               .def(constructor<>())

               .def("set", (Fvector2 & (Fvector2::*)(float, float))(&Fvector2::set), return_reference_to<1>())
               .def("set", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::set), return_reference_to<1>())
               .def("add", (Fvector2 & (Fvector2::*)(float))(&Fvector2::add), return_reference_to<1>())
               .def("add", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::add), return_reference_to<1>())
               .def("add", (Fvector2 & (Fvector2::*)(const Fvector2&, const Fvector2&))(&Fvector2::add), return_reference_to<1>())
               .def("add", (Fvector2 & (Fvector2::*)(const Fvector2&, float))(&Fvector2::add), return_reference_to<1>())
               .def("sub", (Fvector2 & (Fvector2::*)(float))(&Fvector2::sub), return_reference_to<1>())
               .def("sub", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::sub), return_reference_to<1>())
               .def("sub", (Fvector2 & (Fvector2::*)(const Fvector2&, const Fvector2&))(&Fvector2::sub), return_reference_to<1>())
               .def("sub", (Fvector2 & (Fvector2::*)(const Fvector2&, float))(&Fvector2::sub), return_reference_to<1>())
               .def("mul", (Fvector2 & (Fvector2::*)(float))(&Fvector2::mul), return_reference_to<1>())
               .def("mul", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::mul), return_reference_to<1>())
               .def("div", (Fvector2 & (Fvector2::*)(float))(&Fvector2::div), return_reference_to<1>())
               .def("div", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::div), return_reference_to<1>())
               .def("min", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::min), return_reference_to<1>())
               .def("max", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::max), return_reference_to<1>())
               .def("abs", &Fvector2::abs, return_reference_to<1>())
               //.def("similar", &Fvector2::similar)

               .def("mad", (Fvector2 & (Fvector2::*)(const Fvector2&, float))(&Fvector2::mad), return_reference_to<1>())
               .def("mad", (Fvector2 & (Fvector2::*)(const Fvector2&, const Fvector2&, float))(&Fvector2::mad), return_reference_to<1>())
               .def("mad", (Fvector2 & (Fvector2::*)(const Fvector2&, const Fvector2&))(&Fvector2::mad), return_reference_to<1>())
               .def("mad", (Fvector2 & (Fvector2::*)(const Fvector2&, const Fvector2&, const Fvector2&))(&Fvector2::mad), return_reference_to<1>())

               .def("magnitude", &Fvector2::magnitude)

               .def("normalize", (Fvector2 & (Fvector2::*)())(&Fvector2::normalize_safe), return_reference_to<1>())
               .def("normalize", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::normalize_safe), return_reference_to<1>())
               .def("normalize_safe", (Fvector2 & (Fvector2::*)())(&Fvector2::normalize_safe), return_reference_to<1>())
               .def("normalize_safe", (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::normalize_safe), return_reference_to<1>())

               .def("dotproduct", &Fvector2::dotproduct)
               .def("crossproduct", &Fvector2::crossproduct, return_reference_to<1>())
               .def("distance_to", &Fvector2::distance_to)

               .def("getH", &Fvector2::getH),

            class_<Fvector4>("___VECTOR4")
               .def_readwrite("x", &Fvector4::x)
               .def_readwrite("y", &Fvector4::y)
               .def_readwrite("z", &Fvector4::y)
               .def_readwrite("w", &Fvector4::w)
               .def(constructor<>())

               .def("set", (Fvector4 & (Fvector4::*)(float, float, float, float))(&Fvector4::set), return_reference_to<1>())
               .def("set", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::set), return_reference_to<1>())
               .def("add", (Fvector4 & (Fvector4::*)(float))(&Fvector4::add), return_reference_to<1>())
               .def("add", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::add), return_reference_to<1>())
               .def("add", (Fvector4 & (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::add), return_reference_to<1>())
               .def("add", (Fvector4 & (Fvector4::*)(const Fvector4&, float))(&Fvector4::add), return_reference_to<1>())
               .def("sub", (Fvector4 & (Fvector4::*)(float))(&Fvector4::sub), return_reference_to<1>())
               .def("sub", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::sub), return_reference_to<1>())
               .def("sub", (Fvector4 & (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::sub), return_reference_to<1>())
               .def("sub", (Fvector4 & (Fvector4::*)(const Fvector4&, float))(&Fvector4::sub), return_reference_to<1>())
               .def("mul", (Fvector4 & (Fvector4::*)(float))(&Fvector4::mul), return_reference_to<1>())
               .def("mul", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::mul), return_reference_to<1>())
               .def("div", (Fvector4 & (Fvector4::*)(float))(&Fvector4::div), return_reference_to<1>())
               .def("div", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::div), return_reference_to<1>())
               //.def("min", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::min), return_reference_to<1>())
               //.def("max", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::max), return_reference_to<1>())
               //.def("abs", &Fvector4::abs, return_reference_to<1>())
               //.def("similar", &Fvector4::similar)

               //.def("mad", (Fvector4 & (Fvector4::*)(const Fvector4&, float))(&Fvector4::mad), return_reference_to<1>())
               //.def("mad", (Fvector4 & (Fvector4::*)(const Fvector4&, const Fvector4&, float))(&Fvector4::mad), return_reference_to<1>())
               //.def("mad", (Fvector4 & (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::mad), return_reference_to<1>())
               //.def("mad", (Fvector4 & (Fvector4::*)(const Fvector4&, const Fvector4&, const Fvector4&))(&Fvector4::mad), return_reference_to<1>())

               .def("magnitude", &Fvector4::magnitude)

               .def("normalize", (Fvector4 & (Fvector4::*)())(&Fvector4::normalize), return_reference_to<1>())
               .def("normalize", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::normalize), return_reference_to<1>())
               //.def("normalize_safe", (Fvector4 & (Fvector4::*)())(&Fvector4::normalize_safe), return_reference_to<1>())
               //.def("normalize_safe", (Fvector4 & (Fvector4::*)(const Fvector4&))(&Fvector4::normalize_safe), return_reference_to<1>())

               //.def("dotproduct", &Fvector4::dotproduct)
               //.def("crossproduct", &Fvector4::crossproduct, return_reference_to<1>())
               //.def("distance_to", &Fvector4::distance_to)

               //.def("getH", &Fvector4::getH)
               ,

            class_<Fbox>("Fbox").def(constructor<>()).def_readwrite("min", &Fbox::min).def_readwrite("max", &Fbox::max),

            class_<Frect>("Frect")
               .def(constructor<>())
               .def("set", (Frect & (Frect::*)(float, float, float, float))(&Frect::set), return_reference_to<1>())
               .def_readwrite("lt", &Frect::lt)
               .def_readwrite("rb", &Frect::rb)
               .def_readwrite("x1", &Frect::x1)
               .def_readwrite("x2", &Frect::x2)
               .def_readwrite("y1", &Frect::y1)
               .def_readwrite("y2", &Frect::y2)
    ];
}
