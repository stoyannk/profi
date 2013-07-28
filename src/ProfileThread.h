// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

#include <HashMap.h>
#include <Timer.h>
#include <ProfileScope.h>

namespace profi {

class ProfileThread : public ProfileScope {
public:
	ProfileThread(const char* name, std::mutex& threadHashMutex);
	virtual ~ProfileThread();
	
	void EnterScope(const char* name);
	void ExitScope(unsigned long long elapsedTime, ProfileScope* parentScope);

	const Timer& GetTimer() const { return m_Timer; }
	ProfileScope* GetActiveScope() const { return m_ActiveScope; }

private:
	ProfileScope* m_ActiveScope;
	Timer m_Timer;
	std::mutex* m_ThreadsOwnMutex;
};

}