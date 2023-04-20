////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes_wrappers.h
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes wrappers
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"
#include "base_client_classes.h"
#include "../xr_3da/engineapi.h"
#include "../xrcdb/ispatial.h"
#include "../xr_3da/isheduled.h"
#include "../xr_3da/irenderable.h"
#include "../xr_3da/icollidable.h"
#include "..\xr_3da\xr_object.h"
#include "entity.h"
#include "ai_space.h"
#include "script_engine.h"
#include "xrServer_Object_Base.h"

template <typename Base, typename... Ts>
class DLL_PureWrapper : public Base, public Ts...
{
public:
    DLL_PureWrapper() = default;
    virtual ~DLL_PureWrapper() = default;

    DLL_Pure* _construct() override { return (luabind::call_member<DLL_Pure*>(this, "_construct")); }

    static DLL_Pure* _construct_static(Base* self) { return (self->Base::_construct()); }
};

using CDLL_PureWrapper = DLL_PureWrapper<DLL_Pure, luabind::wrap_base>;

template <typename Base, typename... Ts>
class ISheduledWrapper : public Base, public Ts...
{
public:
    ISheduledWrapper() = default;
    virtual ~ISheduledWrapper() = default;

    float shedule_Scale() override { return 1; }

    void shedule_Update(u32 dt) override { Base::shedule_Update(dt); }
};

using CISheduledWrapper = ISheduledWrapper<ISheduled, luabind::wrap_base>;

template <typename Base, typename... Ts>
class IRenderableWrapper : public Base, public Ts...
{
public:
    IRenderableWrapper() = default;
    virtual ~IRenderableWrapper() = default;
};

using CIRenderableWrapper = IRenderableWrapper<IRenderable, luabind::wrap_base>;

using CGameObjectDLL_Pure = DLL_PureWrapper<CGameObject, luabind::wrap_base>;
using CGameObjectISheduled = ISheduledWrapper<CGameObjectDLL_Pure>;
using CGameObjectIRenderable = IRenderableWrapper<CGameObjectISheduled>;

class CGameObjectWrapper : public CGameObjectIRenderable
{
public:
    CGameObjectWrapper() = default;
    virtual ~CGameObjectWrapper() = default;

    virtual bool use(CGameObject* who_use) { return call<bool>("use", who_use); }

    static bool use_static(CGameObject* self, CGameObject* who_use) { return self->CGameObject::use(who_use); }

    virtual BOOL net_Spawn(CSE_Abstract* data) { return (luabind::call_member<bool>(this, "net_Spawn", data)); }

    static bool net_Spawn_static(CGameObject* self, CSE_Abstract* abstract) { return (!!self->CGameObject::net_Spawn(abstract)); }
};