////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_impl.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory.h"
#include "object_item_single.h"

#include "object_type_traits.h"
#include "object_item_client_server.h"

template <typename _client_type, typename _server_type>
IC void CObjectFactory::add(const CLASS_ID& clsid, LPCSTR script_clsid)
{
    {
        typedef object_type_traits::is_base_and_derived<CLIENT_BASE_CLASS, _client_type> a;
        STATIC_CHECK(a::value, Client_class_must_be_derived_from_the_CLIENT_BASE_CLASS);
    }
    {
        typedef object_type_traits::is_base_and_derived<SERVER_BASE_CLASS, _server_type> a;
        STATIC_CHECK(a::value, Server_class_must_be_derived_from_the_SERVER_BASE_CLASS);
    }
    add(xr_new<CObjectItemClientServer<_client_type, _server_type>>(clsid, script_clsid));
}

template <typename _unknown_type>
IC void CObjectFactory::add(const CLASS_ID& clsid, LPCSTR script_clsid)
{
    {
        typedef object_type_traits::is_base_and_derived<CLIENT_BASE_CLASS, _unknown_type> a;
        typedef object_type_traits::is_base_and_derived<SERVER_BASE_CLASS, _unknown_type> b;
        STATIC_CHECK(a::value || b::value, Class_must_be_derived_from_the_CLIENT_BASE_CLASS_or_SERVER_BASE_CLASS);
    }
    add(xr_new<CObjectItemSingle<_unknown_type, object_type_traits::is_base_and_derived<CLIENT_BASE_CLASS, _unknown_type>::value>>(clsid, script_clsid));
}
