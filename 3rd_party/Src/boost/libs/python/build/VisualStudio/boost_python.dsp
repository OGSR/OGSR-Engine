# Microsoft Developer Studio Project File - Name="boost_python" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=BOOST_PYTHON - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boost_python.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boost_python.mak" CFG="BOOST_PYTHON - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boost_python - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "boost_python - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boost_python - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../bin-stage"
# PROP Intermediate_Dir "release-obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BPL_EXPORTS" /YX /FD /Zm800 /Zm800 /Zm800 /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BOOST_PYTHON_DYNAMIC_LIB" /D "BOOST_PYTHON_SOURCE" /FD /Zm800 /Zm800 /Zm800 /Zm800 /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1409 /d "NDEBUG"
# ADD RSC /l 0x1409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "boost_python - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../bin-stage"
# PROP Intermediate_Dir "debug-obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BPL_EXPORTS" /YX /FD /Zm800 /Zm800 /Zm800 /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "../../../../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BOOST_PYTHON_DYNAMIC_LIB" /D "BOOST_PYTHON_SOURCE" /FD /Zm800 /Zm800 /Zm800 /Zm800 /Zm800 /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1409 /d "_DEBUG"
# ADD RSC /l 0x1409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../bin-stage/boost_python_debug.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "boost_python - Win32 Release"
# Name "boost_python - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\aix_init_module.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\converter\arg_to_python_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\converter\builtin_converters.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\class.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\enum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\errors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\converter\from_python.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\function.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\inheritance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\iterator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\life_support.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\list.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\long.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\module.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\numeric.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object_operators.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object_protocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\object\pickle_support.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\converter\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\str.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\tuple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\converter\type_id.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "detail"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\aix_init_module.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\api_placeholder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\arg_tuple_size.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\borrowed_ptr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\call_object.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\caller.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\char_array.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\config.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\construct.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\convertible.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\cv_category.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\decorated_type_id.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\def_helper.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\defaults_def.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\defaults_gen.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\dependent.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\destroy.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\exception_handler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\force_instantiate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\if_else.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\indirect_traits.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\make_keyword_range_fn.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\make_tuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\map_entry.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\member_function_cast.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\module_base.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\module_init.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\msvc_typeinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\none.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\not_specified.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\operator_id.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\overloads_fwd.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\pointee.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\preprocessor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\python22_fixed.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\raw_pyobject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\referent_storage.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\result.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\returning.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\scope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\string_literal.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\target.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\translate_exception.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\type_list.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\type_list_impl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\type_list_impl_no_pts.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\type_list_utils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\unwind_type.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\void_ptr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\void_return.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\detail\wrap_python.hpp
# End Source File
# End Group
# Begin Group "converter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\arg_from_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\arg_to_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\arg_to_python_base.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\builtin_converters.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\constructor_function.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\convertible_function.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\from_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\implicit.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\obj_mgr_arg_from_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\object_manager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\pointer_type_id.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\pyobject_traits.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\pyobject_type.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\pytype_arg_from_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\pytype_object_mgr_traits.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\registered.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\registered_pointee.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\registrations.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\registry.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\return_from_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\rvalue_from_python_data.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\converter\to_python_function_type.hpp
# End Source File
# End Group
# Begin Group "object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\add_to_namespace.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\class.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\class_converters.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\class_detail.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\class_wrapper.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\construct.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\enum_base.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\find_instance.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\forward.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\function.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\function_handle.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\function_object.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\inheritance.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\instance.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\iterator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\iterator_core.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\life_support.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\make_holder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\make_instance.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\pickle_support.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\pointer_holder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\py_function.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\select_holder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\value_holder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object\value_holder_fwd.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\boost\python\arg_from_python.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\args.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\args_fwd.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\back_reference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\base_type_traits.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\bases.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\borrowed.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\call.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\call_method.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\cast.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\class.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\class_fwd.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\copy_const_reference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\copy_non_const_reference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\data_members.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\def.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\default_call_policies.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\dict.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\enum.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\errors.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\exception_translator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\extract.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\handle.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\handle_fwd.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\has_back_reference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\implicit.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\init.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\instance_holder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\iterator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\list.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\long.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\lvalue_from_pytype.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\make_function.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\manage_new_object.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\module.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\module_init.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\numeric.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_attributes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_call.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_core.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_fwd.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_items.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_operators.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_protocol.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_protocol_core.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\object_slices.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\operators.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\operators2.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\other.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\overloads.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\pointee.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\proxy.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\ptr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\refcount.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\reference_existing_object.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\return_internal_reference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\return_value_policy.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\scope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\self.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\signature.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\slice_nil.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\str.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\tag.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\to_python_converter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\to_python_indirect.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\to_python_value.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\tuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\type_id.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\python\with_custodian_and_ward.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
