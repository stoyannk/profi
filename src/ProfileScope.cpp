#include "precompiled.h"

#include <ProfileScope.h>
#include <STLAllocator.h>

namespace profi {

ProfileScope::ProfileScope()
	: m_Name(nullptr)
	, m_TotalTime(0)
	, m_CallCount(0)
{}

ProfileScope::ProfileScope(const char* name)
	: m_Name(name)
	, m_TotalTime(0)
	, m_CallCount(0)
{}

ProfileScope::~ProfileScope()
{
	std::for_each(m_ChildProfiles.cbegin(), m_ChildProfiles.cend(), [] (const HashMap::InternalMap::value_type& scope) {
		profi_delete(scope.second);
	});
}

HashMap& ProfileScope::Children()
{
	return m_ChildProfiles;
}

void ProfileScope::IncreaseTimeAndCallCount(unsigned long long time)
{
	m_TotalTime += time;
	++m_CallCount;
}

unsigned long long ProfileScope::GetTime() const
{
	return m_TotalTime;
}

unsigned ProfileScope::GetCallCount() const
{
	return m_CallCount;
}

}
