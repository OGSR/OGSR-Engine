////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef object_factory_inlineH
#define object_factory_inlineH

#pragma once

IC const CObjectFactory& object_factory()
{
    if (!g_object_factory)
    {
        g_object_factory = xr_new<CObjectFactory>();
        g_object_factory->init();
    }
    return (*g_object_factory);
}

IC bool CObjectFactory::CObjectItemPredicate::operator()(const CObjectItemAbstract* item1, const CObjectItemAbstract* item2) const { return (item1->clsid() < item2->clsid()); }

IC bool CObjectFactory::CObjectItemPredicate::operator()(const CObjectItemAbstract* item, const CLASS_ID& clsid) const { return (item->clsid() < clsid); }

IC const CObjectFactory::OBJECT_ITEM_STORAGE& CObjectFactory::clsids() const { return (m_clsids); }

IC const CObjectItemAbstract& CObjectFactory::item(const CLASS_ID& clsid) const
{
    actualize();
    const_iterator I = std::lower_bound(clsids().begin(), clsids().end(), clsid, CObjectItemPredicate());
    VERIFY((I != clsids().end()) && ((*I)->clsid() == clsid));
    return (**I);
}

IC void CObjectFactory::add(CObjectItemAbstract* item)
{
    ASSERT_FMT_DBG(std::find_if(clsids().cbegin(), clsids().cend(), [item](const CObjectItemAbstract* item_compare) { return item->clsid() == item_compare->clsid(); }) ==
                       clsids().end(),
                   "!![%s] Clsid [%u] already exists! Script clsid: [%s]", __FUNCTION__, item->clsid(), item->script_clsid().c_str());
    ASSERT_FMT_DBG(std::find_if(clsids().cbegin(), clsids().cend(),
                                [item](const CObjectItemAbstract* item_compare) { return item->script_clsid() == item_compare->script_clsid(); }) == clsids().end(),
                   "!![%s] Script clsid [%s] already exists! Clsid: [%u]", __FUNCTION__, item->script_clsid().c_str(), item->clsid());

    m_actual = false;
    m_clsids.push_back(item);
}

IC int CObjectFactory::script_clsid(const CLASS_ID& clsid) const
{
    actualize();
    const_iterator I = std::lower_bound(clsids().begin(), clsids().end(), clsid, CObjectItemPredicate());
    VERIFY((I != clsids().end()) && ((*I)->clsid() == clsid));
    return (int(I - clsids().begin()));
}

IC CObjectFactory::CLIENT_BASE_CLASS* CObjectFactory::client_object(const CLASS_ID& clsid) const { return (item(clsid).client_object()); }

IC CObjectFactory::SERVER_BASE_CLASS* CObjectFactory::server_object(const CLASS_ID& clsid, LPCSTR section) const { return (item(clsid).server_object(section)); }

IC void CObjectFactory::actualize() const
{
    if (m_actual)
        return;

    m_actual = true;
    std::sort(m_clsids.begin(), m_clsids.end(), CObjectItemPredicate());
}

#endif