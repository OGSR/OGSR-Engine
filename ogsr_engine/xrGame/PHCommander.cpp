#include "stdafx.h"
#include "PHCommander.h"


CPHCall::CPHCall(CPHCondition* condition,CPHAction* action) {
  m_action    = action;
  m_condition = condition;
  paused      = 0;
  removed     = false;
}

CPHCall::~CPHCall()
{
	xr_delete(m_action);
	xr_delete(m_condition);
}
bool CPHCall::obsolete()
{
	return m_action->obsolete()||m_condition->obsolete();
}

void CPHCall::check()
{
	if(m_condition->is_true() && m_action)
		m_action->run();
}

bool CPHCall::equal(CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)
{
	return m_action->compare(cmp_action)&&m_condition->compare(cmp_condition);
}
bool CPHCall::is_any(CPHReqComparerV* v)
{
	return m_action->compare(v)||m_condition->compare(v);
}


void CPHCall::setPause( u32 ms ) {
  paused = Device.dwTimeGlobal + ms;
}

bool CPHCall::isPaused() {
  return paused > Device.dwTimeGlobal;
}


void CPHCall::removeLater() {
  removed = true;
}

bool CPHCall::isNeedRemove() {
  return removed;
}


void delete_call(CPHCall* &call)
{
	try{
		xr_delete(call);
	}
	catch(...)
	{
		call = nullptr;
	}
}
/////////////////////////////////////////////////////////////////////////////////
CPHCommander::~CPHCommander()
{
	clear();
}
void CPHCommander::clear	()
{
	while (m_calls.size())	{
		remove_call(m_calls.end()-1);
	}
}


void CPHCommander::update() {
  for( int i = 0; i < m_calls.size(); i++ ) {
    if ( !m_calls[ i ]->isNeedRemove() ) {
      if ( m_calls[ i ]->isPaused() ) continue;
      try {
        m_calls[ i ]->check();
      }
      catch( ... ) {
        remove_call( m_calls.begin() + i );
        i--;
        continue;
      }
    }
    if ( m_calls[ i ]->isNeedRemove() || m_calls[ i ]->obsolete() ) {
      remove_call( m_calls.begin() + i );
      i--;
      continue;
    }
  }
}


CPHCall* CPHCommander::add_call(CPHCondition* condition,CPHAction* action)
{
	m_calls.push_back(xr_new<CPHCall>(condition, action));
	return m_calls.back();
}

void CPHCommander::remove_call(PHCALL_I i)
{
	delete_call(*i);
	m_calls.erase(i);
}


PHCALL_I CPHCommander::find_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action)
{
	return std::find_if(m_calls.begin(), m_calls.end(), [cmp_condition, cmp_action](CPHCall* call) { return call->equal(cmp_condition, cmp_action); });
}

bool CPHCommander::has_call(CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)
{
	return find_call(cmp_condition,cmp_action) != m_calls.end();
}


void CPHCommander::remove_call( CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action ) {
  auto it = find_call( cmp_condition, cmp_action );
  if ( it != m_calls.end() ) {
    auto call = *it;
    call->removeLater();
  }
}


CPHCall* CPHCommander::add_call_unique( CPHCondition* condition, CPHReqComparerV* cmp_condition, CPHAction* action, CPHReqComparerV* cmp_action ) {
  auto it = find_call( cmp_condition, cmp_action );
  if ( it == m_calls.end() ) return add_call( condition, action );
  return *it;
}


void CPHCommander::remove_calls( CPHReqComparerV* cmp_object ) {
  for (const auto& call : m_calls)
	  if (call->is_any(cmp_object))
		  call->removeLater();
}
