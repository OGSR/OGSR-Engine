////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container.cpp
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_registry_container.h"
#include "object_interfaces.h"
#include "alife_space.h"
#include "object_type_traits.h"

template <typename List, typename Handler>
void process(Handler&& handler)
{
    imdexlib::ts_apply<imdexlib::ts_reverse_t<List>>([&](auto item) { handler(item); });
}

void CALifeRegistryContainer::load(IReader& file_stream)
{
    R_ASSERT2(file_stream.find_chunk(REGISTRY_CHUNK_DATA), "Can't find chunk REGISTRY_CHUNK_DATA!");
    process<TYPE_LIST>([&](auto item) {
        using type = typename decltype(item)::type;
        if constexpr (object_type_traits::is_base_and_derived_v<IPureLoadableObject<IReader>, type>)
        {
            this->type::load(file_stream);
        }
    });
}

void CALifeRegistryContainer::save(IWriter& memory_stream)
{
    memory_stream.open_chunk(REGISTRY_CHUNK_DATA);
    process<TYPE_LIST>([&](auto item) {
        using type = typename decltype(item)::type;
        if constexpr (object_type_traits::is_base_and_derived_v<IPureSavableObject<IWriter>, type>)
        {
            this->type::save(memory_stream);
        }
    });
    memory_stream.close_chunk();
}
