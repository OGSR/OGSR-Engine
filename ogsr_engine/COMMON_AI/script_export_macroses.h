////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_macroses.h
//	Created 	: 24.06.2004
//  Modified 	: 24.06.2004
//	Author		: Andy Kolomiets, Dmitriy Iassenev
//	Description : XRay Script export macroses
////////////////////////////////////////////////////////////////////////////

#pragma once

#define DEFINE_LUABIND_CLASS_WRAPPER_0(a,b,c) \
	luabind::class_<a,b >(c)

#define DEFINE_LUABIND_CLASS_WRAPPER_1(a,b,c,d) \
	luabind::class_<a,b,d >(c)

#define DEFINE_LUABIND_CLASS_WRAPPER_2(a,b,c,d,e) \
	luabind::class_<a,b,bases<d,e > >(c)

#define DEFINE_LUABIND_CLASS_WRAPPER_3(a,b,c,d,e,f) \
	luabind::class_<a,b,bases<d,e,f > >(c)

#define DEFINE_LUABIND_CLASS_WRAPPER_4(a,b,c,d,e,f,g) \
	luabind::class_<a,b,bases<d,e,f,g > >(c)

#define DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,c) \
	.def(#c, &a::c, &b::c##_static)

#define DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_CONST_0(a,b,c,d) \
	.def(#c, (d (a::*)() const)(&a::c), (d (*)(const a*))(&b::c##_static))

#ifndef LUABIND_NO_EXCEPTIONS
#	define CAST_FAILED(v_func_name,ret_type) \
		catch(luabind::cast_failed exception) {										\
			ai().script_engine().script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT RUNTIME ERROR : luabind::cast_failed in function %s (%s)!",#v_func_name,#ret_type);\
			return ((ret_type)(0));													\
		}
#else
#	define CAST_FAILED(v_func_name,ret_type)
#endif

#define DEFINE_LUA_WRAPPER_CONST_METHOD_0(v_func_name,ret_type)							\
		virtual ret_type v_func_name() const											\
		{																				\
			try {																		\
				return luabind::call_member<ret_type>(this,#v_func_name);						\
			}																			\
			CAST_FAILED(v_func_name,ret_type)											\
			catch(...) {																\
				return ((ret_type)(0));													\
			}																			\
		}																				\
		static ret_type v_func_name##_static(const inherited* ptr)						\
		{                                                                               \
			return ptr->self_type::inherited::v_func_name();							\
		}

#ifdef DEBUG
#define DEFINE_LUA_WRAPPER_CONST_METHOD_1(v_func_name,ret_type,t1)						\
		virtual ret_type v_func_name(t1 p1) const										\
		{																				\
			try {																		\
				return luabind::call_member<ret_type>(this,#v_func_name,p1);						\
			}																			\
			CAST_FAILED(v_func_name,ret_type)											\
			catch(...) {																\
				return ((ret_type)(0));													\
			}																			\
		}																				\
		static ret_type v_func_name##_static(const inherited* ptr, t1 p1)				\
		{                                                                               \
			return ptr->self_type::inherited::v_func_name(p1);							\
		}
#else // DEBUG
#define DEFINE_LUA_WRAPPER_CONST_METHOD_1(v_func_name,ret_type,t1)						\
		virtual ret_type v_func_name(t1 p1) const										\
		{																				\
			return luabind::call_member<ret_type>(this,#v_func_name,p1);							\
		}																				\
		static ret_type v_func_name##_static(const inherited* ptr, t1 p1)				\
		{                                                                               \
			return ptr->self_type::inherited::v_func_name(p1);							\
		}
#endif // DEBUG

#define DEFINE_LUA_WRAPPER_METHOD_V0(v_func_name)										\
		virtual void v_func_name()														\
		{																				\
			try {																		\
				luabind::call_member<void>(this,#v_func_name);									\
			}																			\
			catch(...) {																\
			}																			\
		}																				\
		static void v_func_name##_static(inherited* ptr)								\
		{                                                                               \
			ptr->self_type::inherited::v_func_name();									\
		}

#define DEFINE_LUA_WRAPPER_METHOD_V1(v_func_name,t1)									\
		virtual void v_func_name(t1 p1)													\
		{																				\
			try {																		\
				luabind::call_member<void>(this,#v_func_name,p1);								\
			}																			\
			catch(...) {																\
			}																			\
		}																				\
		static void v_func_name##_static(inherited* ptr, t1 p1)							\
		{                                                                               \
			ptr->self_type::inherited::v_func_name(p1);									\
		}

#define DEFINE_LUA_WRAPPER_METHOD_0(v_func_name,ret_type)								\
		virtual ret_type v_func_name()													\
		{																				\
			try {																		\
				return luabind::call_member<ret_type>(this,#v_func_name);						\
			}																			\
			CAST_FAILED(v_func_name,ret_type)											\
			catch(...) {																\
				return ((ret_type)(0));													\
			}																			\
		}																				\
		static ret_type v_func_name##_static(inherited* ptr)							\
		{                                                                               \
			return ptr->self_type::inherited::v_func_name();							\
		}

#define DEFINE_LUA_WRAPPER_METHOD_1(v_func_name,ret_type,t1)							\
		virtual ret_type v_func_name(t1 p1)                                    			\
		{																				\
			try {																		\
				return luabind::call_member<ret_type>(this,#v_func_name,p1);						\
			}																			\
			CAST_FAILED(v_func_name,ret_type)											\
			catch(...) {																\
				return ((ret_type)(0));													\
			}																			\
		}                                   											\
		static  ret_type v_func_name##_static(inherited* ptr, t1 p1)            		\
		{																				\
			return ptr->self_type::inherited::v_func_name(p1);                          \
		}

//////////////////////////////////////////////////////////////////////////

#define DEFINE_LUA_WRAPPER_METHOD_R2P1_V1(v_func_name,t1)								\
		virtual void v_func_name(t1& p1)												\
		{																				\
			try {																		\
				this->call<void>(#v_func_name,&p1);											\
			}																			\
			catch(...) {																\
			}																			\
		}                                   											\
		static  void v_func_name##_static(inherited* ptr, t1* p1)						\
		{																				\
			ptr->self_type::inherited::v_func_name(*p1);								\
		}

#define DEFINE_LUA_WRAPPER_METHOD_R2P1_V2(v_func_name,t1,t2)							\
		virtual void v_func_name(t1& p1, t2 p2)											\
		{																				\
			try {																		\
				this->call<void>(#v_func_name,&p1,p2);										\
			}																			\
			catch(...) {																\
			}																			\
		}                                   											\
		static  void v_func_name##_static(inherited* ptr, t1* p1, t2 p2)				\
		{																				\
			ptr->self_type::inherited::v_func_name(*p1,p2);								\
		}
