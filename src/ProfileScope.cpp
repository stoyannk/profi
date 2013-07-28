// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#include "precompiled.h"

#include <ProfileScope.h>
#include <STLAllocator.h>

namespace profi {

ProfileScope::ProfileScope(const char* name, std::mutex& threadHashMutex)
	: m_Name(name)
	, m_TotalTime(0)
	, m_CallCount(0)
	, m_ChildProfiles(threadHashMutex)
{}

ProfileScope::~ProfileScope()
{
	std::for_each(m_ChildProfiles.begin(), m_ChildProfiles.end(), [] (const HashMap::value_type& scope) {
		profi_delete(scope);
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

void ProfileScope::ResetProfile()
{
	m_TotalTime = 0;
	m_CallCount = 0;
}

}
