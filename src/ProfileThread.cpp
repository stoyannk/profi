// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#include "precompiled.h"

#include <ProfileThread.h>
#include <ProfileScope.h>
#include <STLAllocator.h>

namespace profi {

ProfileThread::ProfileThread(const char* name, std::mutex& threadHashMutex)
	: ProfileScope(name, threadHashMutex)
	, m_ActiveScope(nullptr)
	, m_ThreadsOwnMutex(&threadHashMutex)
{}

ProfileThread::~ProfileThread()
{
	// delete the mutex associated with this thread
	profi_delete(m_ThreadsOwnMutex);
}

void ProfileThread::EnterScope(const char* name)
{
	auto& map = m_ActiveScope ? m_ActiveScope->Children() : Children();

	ProfileScope* profile = map.Get(name);
	if(!profile) {
		profile = profi_new(ProfileScope, name, *m_ThreadsOwnMutex);
		map.Insert(profile);
	}
	m_ActiveScope = profile;
}

void ProfileThread::ExitScope(unsigned long long elapsedTime, ProfileScope* parentScope)
{
	m_ActiveScope->IncreaseTimeAndCallCount(elapsedTime);
	
	if(!parentScope) {
		IncreaseTimeAndCallCount(elapsedTime); // count all 1-st level calls as well as total time spent in the thread
	}

	m_ActiveScope = parentScope;
}

}
