////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_factor_parameters_inline.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade factor parameters class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CTradeFactorParameters::CTradeFactorParameters			()
{
}

IC	void CTradeFactorParameters::clear						()
{
	m_factors.clear			();
        m_factors_re.clear();
}


IC void CTradeFactorParameters::enable ( const shared_str &section, const CTradeFactors &factors ) {
  static std::regex Reg( "^/([^/]+)/$" );
  std::smatch results;
  std::string str( section.c_str() );
  if ( std::regex_search( str, results, Reg ) ) {
    std::regex re( results[ 1 ].str() );
    m_factors_re.push_back({ re, factors });
  }
  else {
    VERIFY( m_factors.find( section ) == m_factors.end() );
    m_factors.insert( std::make_pair( section, factors ) );
  }
}


IC bool CTradeFactorParameters::enabled ( const shared_str &section ) const {
  FACTORS::const_iterator I = m_factors.find(section);
  if ( I != m_factors.end() )
    return true;
  for ( const auto &it : m_factors_re ) {
    std::smatch results;
    std::string str( section.c_str() );
    if ( std::regex_search( str, results, it.first ) ) {
      return true;
    }
  }
  return false;
}


IC const CTradeFactors &CTradeFactorParameters::factors ( const shared_str &section ) const {
  FACTORS::const_iterator I = m_factors.find( section );
  if ( I != m_factors.end() )
    return (*I).second;
  for ( const auto &it : m_factors_re ) {
    std::smatch results;
    std::string str( section.c_str() );
    if ( std::regex_search( str, results, it.first ) ) {
      return it.second;
    }
  }
  ASSERT_FMT( false, "[%s]: %s not found", __FUNCTION__, section.c_str() );
  return (*I).second;
}


IC bool CTradeFactorParameters::disabled ( const shared_str &section ) const {
  return enabled( section ) ? factors( section ).disabled() : false;
}
