#pragma once

IC	CEntityCondition &CEntityAlive::conditions	() const
{
	VERIFY		(m_entity_condition);
	return		(*m_entity_condition);
}
