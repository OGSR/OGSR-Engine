#include "stdafx.h"

#include "../../xr_3da/Render.h"
#include "../xrRender/ResourceManager.h"
#include "../xrRender/tss.h"
#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_recorder.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include <format>

class adopt_dx10options
{
public:
    bool _dx10_msaa_alphatest_atoc() { return (RImplementation.o.dx10_msaa_alphatest == CRender::MSAA_ATEST_DX10_0_ATOC); }
};

// wrapper
class adopt_dx10sampler
{
    CBlender_Compile* m_pC;
    u32 m_SI; //	Sampler index
public:
    adopt_dx10sampler(CBlender_Compile* C, u32 SamplerIndex) : m_pC(C), m_SI(SamplerIndex)
    {
        if (u32(-1) == m_SI)
            m_pC = 0;
    }
    adopt_dx10sampler(const adopt_dx10sampler& _C) : m_pC(_C.m_pC), m_SI(_C.m_SI)
    {
        if (u32(-1) == m_SI)
            m_pC = 0;
    }

    //	adopt_sampler&			_texture		(LPCSTR texture)		{ if (C) C->i_Texture	(stage,texture);											return *this;	}
    //	adopt_sampler&			_projective		(bool _b)				{ if (C) C->i_Projective(stage,_b);													return *this;	}
    //	adopt_sampler&			_clamp			()						{ if (C) C->i_Address	(stage,D3DTADDRESS_CLAMP);									return *this;	}
    //	adopt_sampler&			_wrap			()						{ if (C) C->i_Address	(stage,D3DTADDRESS_WRAP);									return *this;	}
    //	adopt_sampler&			_mirror			()						{ if (C) C->i_Address	(stage,D3DTADDRESS_MIRROR);									return *this;	}
    //	adopt_sampler&			_f_anisotropic	()						{ if (C) C->i_Filter	(stage,D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,D3DTEXF_ANISOTROPIC);	return *this;	}
    //	adopt_sampler&			_f_trilinear	()						{ if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_LINEAR);		return *this;	}
    //	adopt_sampler&			_f_bilinear		()						{ if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_POINT, D3DTEXF_LINEAR);		return *this;	}
    //	adopt_sampler&			_f_linear		()						{ if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_NONE,  D3DTEXF_LINEAR);		return *this;	}
    //	adopt_sampler&			_f_none			()						{ if (C) C->i_Filter	(stage,D3DTEXF_POINT, D3DTEXF_NONE,  D3DTEXF_POINT);		return *this;	}
    //	adopt_sampler&			_fmin_none		()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_NONE);										return *this;	}
    //	adopt_sampler&			_fmin_point		()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_POINT);										return *this;	}
    //	adopt_sampler&			_fmin_linear	()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_LINEAR);										return *this;	}
    //	adopt_sampler&			_fmin_aniso		()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_ANISOTROPIC);								return *this;	}
    //	adopt_sampler&			_fmip_none		()						{ if (C) C->i_Filter_Mip(stage,D3DTEXF_NONE);										return *this;	}
    //	adopt_sampler&			_fmip_point		()						{ if (C) C->i_Filter_Mip(stage,D3DTEXF_POINT);										return *this;	}
    //	adopt_sampler&			_fmip_linear	()						{ if (C) C->i_Filter_Mip(stage,D3DTEXF_LINEAR);										return *this;	}
    //	adopt_sampler&			_fmag_none		()						{ if (C) C->i_Filter_Mag(stage,D3DTEXF_NONE);										return *this;	}
    //	adopt_sampler&			_fmag_point		()						{ if (C) C->i_Filter_Mag(stage,D3DTEXF_POINT);										return *this;	}
    //	adopt_sampler&			_fmag_linear	()						{ if (C) C->i_Filter_Mag(stage,D3DTEXF_LINEAR);										return *this;	}
};
/*
class	adopt_dx10texture
{
    CBlender_Compile*		m_pC;
    u32						m_TI;	//	Sampler index
public:
    adopt_dx10texture	(CBlender_Compile*	C, u32 TextureIndex)	: m_pC(C), m_TI(TextureIndex)		{ if (u32(-1)==m_TI) m_pC=0;}
    adopt_dx10texture	(const adopt_dx10texture&	_C)				: m_pC(_C.m_pC), m_TI(_C.m_TI)	{ if (u32(-1)==m_TI) m_pC=0;}
};
*/

#pragma warning(push)
#pragma warning(disable : 4512)
// wrapper
class adopt_compiler
{
    CBlender_Compile* C;
    bool& m_bFirstPass;

    void TryEndPass()
    {
        if (!m_bFirstPass)
            C->r_End();
        m_bFirstPass = false;
    }

public:
    adopt_compiler(CBlender_Compile* _C, bool& bFirstPass) : C(_C), m_bFirstPass(bFirstPass) { m_bFirstPass = true; }
    adopt_compiler(const adopt_compiler& _C) : C(_C.C), m_bFirstPass(_C.m_bFirstPass) {}

    adopt_compiler& _options(int P, bool S)
    {
        C->SetParams(P, S);
        return *this;
    }
    adopt_compiler& _o_emissive(bool E)
    {
        C->SH->flags.bEmissive = E;
        return *this;
    }
    adopt_compiler& _o_distort(bool E)
    {
        C->SH->flags.bDistort = E;
        return *this;
    }
    adopt_compiler& _o_wmark(bool E)
    {
        C->SH->flags.bWmark = E;
        return *this;
    }
    adopt_compiler& _pass(LPCSTR vs, LPCSTR ps)
    {
        TryEndPass();
        C->r_Pass(vs, ps, true);
        return *this;
    }
    adopt_compiler& _passgs(LPCSTR vs, LPCSTR gs, LPCSTR ps)
    {
        TryEndPass();
        C->r_Pass(vs, gs, ps, true);
        return *this;
    }
    adopt_compiler& _fog(bool _fog)
    {
        C->PassSET_LightFog(FALSE, _fog);
        return *this;
    }
    adopt_compiler& _ZB(bool _test, bool _write)
    {
        C->PassSET_ZB(_test, _write);
        return *this;
    }
    adopt_compiler& _blend(bool _blend, u32 abSRC, u32 abDST)
    {
        C->PassSET_ablend_mode(_blend, abSRC, abDST);
        return *this;
    }
    adopt_compiler& _aref(bool _aref, u32 aref)
    {
        C->PassSET_ablend_aref(_aref, aref);
        return *this;
    }
    adopt_compiler& _dx10texture(LPCSTR _resname, LPCSTR _texname)
    {
        C->r_dx10Texture(_resname, _texname);
        return *this;
    }
    adopt_dx10sampler _dx10sampler(LPCSTR _name)
    {
        u32 s = C->r_dx10Sampler(_name);
        return adopt_dx10sampler(C, s);
    }

    //	DX10 specific
    adopt_compiler& _dx10color_write_enable(bool cR, bool cG, bool cB, bool cA)
    {
        C->r_ColorWriteEnable(cR, cG, cB, cA);
        return *this;
    }
    adopt_compiler& _dx10Stencil(bool Enable, u32 Func, u32 Mask, u32 WriteMask, u32 Fail, u32 Pass, u32 ZFail)
    {
        C->r_Stencil(Enable, Func, Mask, WriteMask, Fail, Pass, ZFail);
        return *this;
    }
    adopt_compiler& _dx10StencilRef(u32 Ref)
    {
        C->r_StencilRef(Ref);
        return *this;
    }
		adopt_compiler& _dx10CullMode(u32 Ref)
	{
		C->r_CullMode((D3DCULL)Ref);
		return *this;
	}
    adopt_compiler& _dx10ATOC(bool Enable)
    {
        C->RS.SetRS(XRDX10RS_ALPHATOCOVERAGE, Enable);
        return *this;
    }
    adopt_compiler& _dx10ZFunc(u32 Func)
    {
        C->RS.SetRS(D3DRS_ZFUNC, Func);
        return *this;
    }
    // adopt_dx10texture		_dx10texture	(LPCSTR _name)							{	u32 s = C->r_dx10Texture(_name,0);			return	adopt_dx10sampler(C,s);	}

    adopt_dx10options _dx10Options() { return adopt_dx10options(); };
};
#pragma warning(pop)

class adopt_blend
{
public:
};

class adopt_cmp_func
{
public:
};

class adopt_stencil_op
{
public:
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../../xr_3da/ai_script_space.h"

lua_State* LSVM = nullptr;
constexpr const char* GlobalNamespace = "_G";
static constexpr const char* FILE_HEADER =
    "\
local function script_name() \
return '{0}' \
end; \
local this; \
module('{0}', package.seeall, function(m) this = m end); \
{1}";

static const char* get_lua_traceback(lua_State* L)
{
    luaL_traceback(L, L, nullptr, 0);
    auto tb = lua_tostring(L, -1);
    lua_pop(L, 1);
    return tb;
}

bool print_output(const char* caScriptFileName, int errorCode)
{
    auto Prefix = "";
    if (errorCode)
    {
        switch (errorCode)
        {
        case LUA_ERRRUN: {
            Prefix = "SCRIPT RUNTIME ERROR";
            break;
        }
        case LUA_ERRMEM: {
            Prefix = "SCRIPT ERROR (memory allocation)";
            break;
        }
        case LUA_ERRERR: {
            Prefix = "SCRIPT ERROR (while running the error handler function)";
            break;
        }
        case LUA_ERRFILE: {
            Prefix = "SCRIPT ERROR (while running file)";
            break;
        }
        case LUA_ERRSYNTAX: {
            Prefix = "SCRIPT SYNTAX ERROR";
            break;
        }
        case LUA_YIELD: {
            Prefix = "Thread is yielded";
            break;
        }
        default: NODEFAULT;
        }
    }
    auto traceback = get_lua_traceback(LSVM);
    if (!lua_isstring(LSVM, -1)) //НЕ УДАЛЯТЬ! Иначе будут вылeты без лога!
    {
        Msg("*********************************************************************************");
        Msg("[ResourceManager_Scripting.print_output(%s)] %s!\n%s", caScriptFileName, Prefix, traceback);
        Msg("*********************************************************************************");
        return false;
    }
    auto S = lua_tostring(LSVM, -1);
    Msg("*********************************************************************************");
    Msg("[ResourceManager_Scripting.print_output(%s)] %s:\n%s\n%s", caScriptFileName, Prefix, S, traceback);
    Msg("*********************************************************************************");
    return true;
}

bool load_buffer(const char* caBuffer, size_t tSize, const char* caScriptName, const char* caNameSpaceName)
{
    const std::string_view strbuf{caBuffer, tSize};
    const std::string script = std::format(FILE_HEADER, caNameSpaceName, strbuf);

    // Log("[CResourceManager::load_buffer] Loading buffer:");
    // Log(script.c_str());

    int l_iErrorCode = luaL_loadbuffer(LSVM, script.c_str(), script.size(), caScriptName);
    if (l_iErrorCode)
    {
        print_output(caScriptName, l_iErrorCode);
        R_ASSERT(false); //НЕ ЗАКОММЕНТИРОВАТЬ!
        return false;
    }
    return true;
}

bool do_file(const char* caScriptName, const char* caNameSpaceName)
{
    auto l_tpFileReader = FS.r_open(caScriptName);
    if (!l_tpFileReader)
    {
        //заменить на ассерт?
        Msg("!![CResourceManager::do_file] Cannot open file [%s]", caScriptName);
        return false;
    }

    l_tpFileReader->skip_bom(caScriptName);

    string_path l_caLuaFileName;
    strconcat(sizeof(l_caLuaFileName), l_caLuaFileName, "@", caScriptName); // KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\class_registrator.script

    load_buffer(reinterpret_cast<const char*>(l_tpFileReader->pointer()), (size_t)l_tpFileReader->elapsed(), l_caLuaFileName, caNameSpaceName);

    FS.r_close(l_tpFileReader);

    int l_iErrorCode = lua_pcall(LSVM, 0, 0, 0); // KRodin: без этого скрипты не работают!
    if (l_iErrorCode)
    {
        print_output(caScriptName, l_iErrorCode);
        R_ASSERT(false); //НЕ ЗАКОММЕНТИРОВАТЬ!
        return false;
    }
    return true;
}

bool namespace_loaded(const char* name, bool remove_from_stack)
{
#ifdef DEBUG
    int start = lua_gettop(LSVM);
#endif
    lua_pushstring(LSVM, GlobalNamespace);
    lua_rawget(LSVM, LUA_GLOBALSINDEX);
    string256 S2;
    xr_strcpy(S2, name);
    auto S = S2;
    for (;;)
    {
        if (!xr_strlen(S))
        {
            VERIFY(lua_gettop(LSVM) >= 1);
            lua_pop(LSVM, 1);
            VERIFY(start == lua_gettop(LSVM));
            return false;
        }
        auto S1 = strchr(S, '.');
        if (S1)
            *S1 = 0;
        lua_pushstring(LSVM, S);
        lua_rawget(LSVM, -2);
        if (lua_isnil(LSVM, -1))
        {
            // lua_settop(LSVM,0);
            VERIFY(lua_gettop(LSVM) >= 2);
            lua_pop(LSVM, 2);
            VERIFY(start == lua_gettop(LSVM));
            return false; // there is no namespace!
        }
        else if (!lua_istable(LSVM, -1))
        {
            // lua_settop(LSVM, 0);
            VERIFY(lua_gettop(LSVM) >= 1);
            lua_pop(LSVM, 1);
            VERIFY(start == lua_gettop(LSVM));
            R_ASSERT3(false, "Error : the namespace is already being used by the non-table object! Name: ", S);
            return false;
        }
        lua_remove(LSVM, -2);
        if (S1)
            S = ++S1;
        else
            break;
    }
    if (!remove_from_stack)
        VERIFY(lua_gettop(LSVM) == start + 1);
    else
    {
        VERIFY(lua_gettop(LSVM) >= 1);
        lua_pop(LSVM, 1);
        VERIFY(lua_gettop(LSVM) == start);
    }
    return true;
}

bool OBJECT_1(const char* identifier, int type)
{
#ifdef DEBUG
    int start = lua_gettop(LSVM);
#endif
    lua_pushnil(LSVM);
    while (lua_next(LSVM, -2))
    {
        if (lua_type(LSVM, -1) == type && !xr_strcmp(identifier, lua_tostring(LSVM, -2)))
        {
            VERIFY(lua_gettop(LSVM) >= 3);
            lua_pop(LSVM, 3);
            VERIFY(lua_gettop(LSVM) == start - 1);
            return true;
        }
        lua_pop(LSVM, 1);
    }
    VERIFY(lua_gettop(LSVM) >= 1);
    lua_pop(LSVM, 1);
    VERIFY(lua_gettop(LSVM) == start - 1);
    return false;
}

bool OBJECT_2(const char* namespace_name, const char* identifier, int type)
{
#ifdef DEBUG
    int start = lua_gettop(LSVM);
#endif
    if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name, false))
    {
        VERIFY(lua_gettop(LSVM) == start);
        return false;
    }
    bool result = OBJECT_1(identifier, type);
    VERIFY(lua_gettop(LSVM) == start);
    return result;
}

#ifdef LUABIND_NO_EXCEPTIONS
void LuaError(lua_State* L)
{
    print_output("[ResourceManager.lua_error]", LUA_ERRRUN);
    Debug.fatal(DEBUG_INFO, "[ResourceManager.lua_error]: %s", lua_isstring(L, -1) ? lua_tostring(L, -1) : "");
}

static void lua_cast_failed(lua_State* L, LUABIND_TYPE_INFO info)
{
    print_output("[ResourceManager.lua_cast_failed]", LUA_ERRRUN);

    Msg("LUA error: cannot cast lua value to %s", info->name());

}
#endif

int lua_pcall_failed(lua_State* L)
{
    print_output("[ResourceManager.lua_pcall_failed]", LUA_ERRRUN);
    Debug.fatal(DEBUG_INFO, "[ResourceManager.lua_pcall_failed]: %s", lua_isstring(L, -1) ? lua_tostring(L, -1) : "");
    if (lua_isstring(L, -1))
        lua_pop(L, 1);
    return LUA_ERRRUN;
}

int lua_panic(lua_State* L)
{
    print_output("[ResourceManager.lua_panic]", LUA_ERRRUN);
    Debug.fatal(DEBUG_INFO, "[ResourceManager.lua_panic]: %s", lua_isstring(L, -1) ? lua_tostring(L, -1) : "");
    return 0;
}

static void* __cdecl luabind_allocator(luabind::memory_allocation_function_parameter, const void* pointer,
                                       size_t const size) //Раньше всего инитится здесь, поэтому пусть здесь и будет
{
    if (!size)
    {
        void* non_const_pointer = const_cast<LPVOID>(pointer);
        xr_free(non_const_pointer);
        return nullptr;
    }

    if (!pointer)
        return Memory.mem_alloc(size);

    void* non_const_pointer = const_cast<LPVOID>(pointer);
    return Memory.mem_realloc(non_const_pointer, size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void LuaLog(const char* caMessage) { Log(caMessage); }

// export
void CResourceManager::LS_Load()
{
    //**************************************************************//
    // Msg("[CResourceManager] Starting LuaJIT");
    R_ASSERT2(!LSVM, "! LuaJIT is already running"); //На всякий случай
    //
    luabind::allocator = &luabind_allocator; //Аллокатор инитится только здесь и только один раз!
    luabind::allocator_parameter = nullptr;

    LSVM = luaL_newstate(); //Запускаем LuaJIT. Память себе он выделит сам.
    luaL_openlibs(LSVM); //Инициализация функций LuaJIT
    R_ASSERT2(LSVM, "! ERROR : Cannot initialize LUA VM!"); //Надо проверить, случается ли такое.
    luabind::open(LSVM); //Запуск луабинда
    //
    //--------------Установка калбеков------------------//
#ifdef LUABIND_NO_EXCEPTIONS
    luabind::set_error_callback(LuaError); //Калбек на ошибки.
    luabind::set_cast_failed_callback(lua_cast_failed);
#endif
    luabind::set_pcall_callback(lua_pcall_failed); // KRodin: НЕ ЗАКОММЕНТИРОВАТЬ НИ В КОЕМ СЛУЧАЕ!!!
    lua_atpanic(LSVM, lua_panic);
    // Msg("[CResourceManager] LuaJIT Started!");
    //-----------------------------------------------------//
    //***************************************************************//

    using namespace luabind;

    module(LSVM)[def("log", &LuaLog),

                 class_<adopt_dx10options>("_dx10options").def("dx10_msaa_alphatest_atoc", &adopt_dx10options::_dx10_msaa_alphatest_atoc)
                     .def("getLevel", [](adopt_dx10options*){ return g_pGameLevel->name().c_str(); })
                 //.def("",					&adopt_dx10options::_dx10Options		),	// returns options-object
                 ,

                 class_<adopt_dx10sampler>("_dx10sampler")
                 //.def("texture",						&adopt_sampler::_texture		,return_reference_to(_1))
                 //.def("project",						&adopt_sampler::_projective		,return_reference_to(_1))
                 //.def("clamp",						&adopt_sampler::_clamp			,return_reference_to(_1))
                 //.def("wrap",						    &adopt_sampler::_wrap			,return_reference_to(_1))
                 //.def("mirror",						&adopt_sampler::_mirror			,return_reference_to(_1))
                 //.def("f_anisotropic",				&adopt_sampler::_f_anisotropic	,return_reference_to(_1))
                 //.def("f_trilinear",					&adopt_sampler::_f_trilinear	,return_reference_to(_1))
                 //.def("f_bilinear",					&adopt_sampler::_f_bilinear		,return_reference_to(_1))
                 //.def("f_linear",					    &adopt_sampler::_f_linear		,return_reference_to(_1))
                 //.def("f_none",						&adopt_sampler::_f_none			,return_reference_to(_1))
                 //.def("fmin_none",					&adopt_sampler::_fmin_none		,return_reference_to(_1))
                 //.def("fmin_point",					&adopt_sampler::_fmin_point		,return_reference_to(_1))
                 //.def("fmin_linear",					&adopt_sampler::_fmin_linear	,return_reference_to(_1))
                 //.def("fmin_aniso",					&adopt_sampler::_fmin_aniso		,return_reference_to(_1))
                 //.def("fmip_none",					&adopt_sampler::_fmip_none		,return_reference_to(_1))
                 //.def("fmip_point",					&adopt_sampler::_fmip_point		,return_reference_to(_1))
                 //.def("fmip_linear",					&adopt_sampler::_fmip_linear	,return_reference_to(_1))
                 //.def("fmag_none",					&adopt_sampler::_fmag_none		,return_reference_to(_1))
                 //.def("fmag_point",					&adopt_sampler::_fmag_point		,return_reference_to(_1))
                 //.def("fmag_linear",					&adopt_sampler::_fmag_linear	,return_reference_to(_1))
                 ,

                 class_<adopt_compiler>("_compiler")
                     .def(constructor<const adopt_compiler&>())
                     .def("begin", &adopt_compiler::_pass, return_reference_to<1>())
                     .def("begin", &adopt_compiler::_passgs, return_reference_to<1>())
                     .def("sorting", &adopt_compiler::_options, return_reference_to<1>())
                     .def("emissive", &adopt_compiler::_o_emissive, return_reference_to<1>())
                     .def("distort", &adopt_compiler::_o_distort, return_reference_to<1>())
                     .def("wmark", &adopt_compiler::_o_wmark, return_reference_to<1>())
                     .def("fog", &adopt_compiler::_fog, return_reference_to<1>())
                     .def("zb", &adopt_compiler::_ZB, return_reference_to<1>())
                     .def("blend", &adopt_compiler::_blend, return_reference_to<1>())
                     .def("aref", &adopt_compiler::_aref, return_reference_to<1>())

                     //	For compatibility only
                     .def("dx10color_write_enable", &adopt_compiler::_dx10color_write_enable, return_reference_to<1>())
                     .def("color_write_enable", &adopt_compiler::_dx10color_write_enable, return_reference_to<1>())
                     .def("dx10texture", &adopt_compiler::_dx10texture, return_reference_to<1>())
                     .def("dx10stencil", &adopt_compiler::_dx10Stencil, return_reference_to<1>())
                     .def("dx10stencil_ref", &adopt_compiler::_dx10StencilRef, return_reference_to<1>())
                     .def("dx10cullmode", &adopt_compiler::_dx10CullMode, return_reference_to<1>())
                     .def("dx10atoc", &adopt_compiler::_dx10ATOC, return_reference_to<1>())
                     .def("dx10zfunc", &adopt_compiler::_dx10ZFunc, return_reference_to<1>())

                     .def("dx10sampler", &adopt_compiler::_dx10sampler) // returns sampler-object
                     .def("dx10Options", &adopt_compiler::_dx10Options), // returns options-object

                 class_<adopt_blend>("blend").enum_("blend")[
                        value("zero", int(D3DBLEND_ZERO)), 
                        value("one", int(D3DBLEND_ONE)), 
                        value("srccolor", int(D3DBLEND_SRCCOLOR)),
                        value("invsrccolor", int(D3DBLEND_INVSRCCOLOR)), 
                        value("srcalpha", int(D3DBLEND_SRCALPHA)), 
                        value("invsrcalpha", int(D3DBLEND_INVSRCALPHA)),
                        value("destalpha", int(D3DBLEND_DESTALPHA)), 
                        value("invdestalpha", int(D3DBLEND_INVDESTALPHA)), 
                        value("destcolor", int(D3DBLEND_DESTCOLOR)),
                        value("invdestcolor", int(D3DBLEND_INVDESTCOLOR)), 
                        value("srcalphasat", int(D3DBLEND_SRCALPHASAT))],

                 class_<adopt_cmp_func>("cmp_func").enum_("cmp_func")[
                        value("never", int(D3DCMP_NEVER)), 
                        value("less", int(D3DCMP_LESS)), 
                        value("equal", int(D3DCMP_EQUAL)),
                        value("lessequal", int(D3DCMP_LESSEQUAL)), 
                        value("greater", int(D3DCMP_GREATER)), 
                        value("notequal", int(D3DCMP_NOTEQUAL)),
                        value("greaterequal", int(D3DCMP_GREATEREQUAL)), 
                        value("always", int(D3DCMP_ALWAYS))],

                 class_<adopt_stencil_op>("stencil_op").enum_("stencil_op")[
                        value("keep", int(D3DSTENCILOP_KEEP)), 
                        value("zero", int(D3DSTENCILOP_ZERO)), 
                        value("replace", int(D3DSTENCILOP_REPLACE)),
                        value("incrsat", int(D3DSTENCILOP_INCRSAT)), 
                        value("decrsat", int(D3DSTENCILOP_DECRSAT)), 
                        value("invert", int(D3DSTENCILOP_INVERT)),
                        value("incr", int(D3DSTENCILOP_INCR)), 
                        value("decr", int(D3DSTENCILOP_DECR))]];

    // load shaders
    xr_vector<char*>* folder = FS.file_list_open("$game_shaders$", ::Render->getShaderPath(), FS_ListFiles | FS_RootOnly);
    VERIFY(folder);
    for (u32 it = 0; it < folder->size(); it++)
    {
        string_path namesp, fn;
        xr_strcpy(namesp, (*folder)[it]);
        if (0 == strext(namesp) || 0 != xr_strcmp(strext(namesp), ".s"))
            continue;
        *strext(namesp) = 0;
        if (0 == namesp[0])
            xr_strcpy(namesp, "_G");
        strconcat(sizeof(fn), fn, ::Render->getShaderPath(), (*folder)[it]);
        FS.update_path(fn, "$game_shaders$", fn);
        do_file(fn, namesp);
    }
    FS.file_list_close(folder);
}

void CResourceManager::LS_Unload()
{
    lua_close(LSVM);
    LSVM = nullptr;
}

BOOL CResourceManager::_lua_HasShader(LPCSTR s_shader)
{
    string256 undercorated;
    for (int i = 0, l = xr_strlen(s_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == s_shader[i]) ? '_' : s_shader[i];

    bool bHasShader = OBJECT_2(undercorated, "normal", LUA_TFUNCTION) || OBJECT_2(undercorated, "l_special", LUA_TFUNCTION);

    // If not found - try to find new ones
    if (!bHasShader)
    {
        for (int i = 0; i < SHADER_ELEMENTS_MAX; ++i)
        {
            string16 buff;
            std::snprintf(buff, sizeof(buff), "element_%d", i);
            if (OBJECT_2(undercorated, buff, LUA_TFUNCTION))
            {
                bHasShader = true;
                break;
            }
        }
    }
    return bHasShader;
}

Shader* CResourceManager::_lua_Create(LPCSTR d_shader, LPCSTR s_textures)
{
    CBlender_Compile C;
    Shader S;

    // undecorate
    string256 undercorated;
    for (int i = 0, l = xr_strlen(d_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == d_shader[i]) ? '_' : d_shader[i];
    LPCSTR s_shader = undercorated;

    // Access to template
    C.BT = NULL;
    C.bEditor = FALSE;
    C.bDetail = FALSE;

    // Prepare
    _ParseList(C.L_textures, s_textures);
    C.detail_texture = NULL;
    C.detail_scaler = NULL;

    // Choose workflow here: old (using named stages) or new (explicitly declaring stage number)
    bool bUseNewWorkflow = false;

    for (int i = 0; i < SHADER_ELEMENTS_MAX; ++i)
    {
        string16 buff;
        std::snprintf(buff, sizeof(buff), "element_%d", i);
        if (OBJECT_2(s_shader, buff, LUA_TFUNCTION))
        {
            C.iElement = i;
            C.bDetail = dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[i] = C._lua_Compile(s_shader, buff);

            bUseNewWorkflow = true;
        }
    }

    if (!bUseNewWorkflow)
    {
        // Compile element	(LOD0 - HQ)
        if (OBJECT_2(s_shader, "normal_hq", LUA_TFUNCTION))
        {
            // Analyze possibility to detail this shader
            C.iElement = 0;
            //.	C.bDetail			= dxRenderDeviceRender::Instance().Resources->_GetDetailTexture(*C.L_textures[0],C.detail_texture,C.detail_scaler);
            // C.bDetail			= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);
            C.bDetail = dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);

            if (C.bDetail)
                S.E[0] = C._lua_Compile(s_shader, "normal_hq");
            else
                S.E[0] = C._lua_Compile(s_shader, "normal");
        }
        else
        {
            if (OBJECT_2(s_shader, "normal", LUA_TFUNCTION))
            {
                C.iElement = 0;
                //.	C.bDetail			= dxRenderDeviceRender::Instance().Resources->_GetDetailTexture(*C.L_textures[0],C.detail_texture,C.detail_scaler);
                // C.bDetail			= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);
                C.bDetail = dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
                S.E[0] = C._lua_Compile(s_shader, "normal");
            }
        }

        // Compile element	(LOD1)
        if (OBJECT_2(s_shader, "normal", LUA_TFUNCTION))
        {
            C.iElement = 1;
            //.	C.bDetail			= dxRenderDeviceRender::Instance().Resources->_GetDetailTexture(*C.L_textures[0],C.detail_texture,C.detail_scaler);
            // C.bDetail			= dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);
            C.bDetail = dxRenderDeviceRender::Instance().Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[1] = C._lua_Compile(s_shader, "normal");
        }

        // Compile element
        if (OBJECT_2(s_shader, "l_point", LUA_TFUNCTION))
        {
            C.iElement = 2;
            C.bDetail = FALSE;
            S.E[2] = C._lua_Compile(s_shader, "l_point");
        }

        // Compile element
        if (OBJECT_2(s_shader, "l_spot", LUA_TFUNCTION))
        {
            C.iElement = 3;
            C.bDetail = FALSE;
            S.E[3] = C._lua_Compile(s_shader, "l_spot");
        }

        // Compile element
        if (OBJECT_2(s_shader, "l_special", LUA_TFUNCTION))
        {
            C.iElement = 4;
            C.bDetail = FALSE;
            S.E[4] = C._lua_Compile(s_shader, "l_special");
        }
    }

    // Search equal in shaders array
    for (u32 it = 0; it < v_shaders.size(); it++)
        if (S.equal(v_shaders[it]))
            return v_shaders[it];

    // Create _new_ entry
    Shader* N = xr_new<Shader>(S);
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_shaders.push_back(N);
    return N;
}

ShaderElement* CBlender_Compile::_lua_Compile(LPCSTR namesp, LPCSTR name)
{
    ShaderElement E;
    SH = &E;
    RS.Invalidate();

    // Compile
    LPCSTR t_0 = *L_textures[0] ? *L_textures[0] : "null";
    LPCSTR t_1 = (L_textures.size() > 1) ? *L_textures[1] : "null";
    LPCSTR t_d = detail_texture ? detail_texture : "null";

    luabind::object shader = luabind::get_globals(LSVM)[namesp];
    luabind::object element = shader[name];

    bool bFirstPass = false;
    adopt_compiler ac = adopt_compiler(this, bFirstPass);
    element(ac, t_0, t_1, t_d);

    r_End();
    ShaderElement* _r = dxRenderDeviceRender::Instance().Resources->_CreateElement(E);
    return _r;
}
