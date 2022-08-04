////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_bool_parameters.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade bool parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CTradeBoolParameters::CTradeBoolParameters() {}

IC void CTradeBoolParameters::clear()
{
    m_sections.clear();
    m_sections_re.clear();
}

IC void CTradeBoolParameters::disable(const shared_str& section)
{
    static std::regex Reg("^/([^/]+)/$");
    std::smatch results;
    const xr_string str(section.c_str());
    if (std::regex_search(str, results, Reg))
    {
        std::regex re(results[1].str());
        m_sections_re.emplace_back(std::move(re));
    }
    else
    {
        VERIFY(std::find(m_sections.begin(), m_sections.end(), section) == m_sections.end());
        m_sections.push_back(section);
    }
}

IC bool CTradeBoolParameters::disabled(const shared_str& section) const
{
    SECTIONS::const_iterator I = std::find(m_sections.begin(), m_sections.end(), section);
    if (I != m_sections.end())
        return true;
    for (const auto& re : m_sections_re)
    {
        std::smatch results;
        const xr_string str(section.c_str());
        if (std::regex_search(str, results, re))
        {
            return true;
        }
    }
    return false;
}
