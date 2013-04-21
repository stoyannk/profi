#include "precompiled.h"

#include <ProfileScope.h>

namespace profi {

ProfileScope::ProfileScope()
	: m_Name(nullptr)
	, m_TotalTime(0)
{}

ProfileScope::ProfileScope(const char* name)
	: m_Name(name)
	, m_TotalTime(0)
{}

ProfileScope::~ProfileScope()
{}

HashMap& ProfileScope::Children()
{
	return m_ChildProfiles;
}

void ProfileScope::IncreaseTime(unsigned long long time)
{
	m_TotalTime += time;
}

unsigned long long ProfileScope::GetTime() const
{
	return m_TotalTime;
}

}
