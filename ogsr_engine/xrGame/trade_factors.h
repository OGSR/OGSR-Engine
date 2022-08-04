////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_factors.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade factors class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CTradeFactors
{
private:
    float m_friend_factor;
    float m_enemy_factor;
    float m_min_condition;
    bool m_disabled;

public:
    IC CTradeFactors(const float& = 1.f, const float& = 1.f, const float& = 0.f, const bool& = false);
    IC const float& friend_factor() const;
    IC const float& enemy_factor() const;
    IC const float& min_condition() const;
    IC const bool& disabled() const;
};

#include "trade_factors_inline.h"
