#include "precompiled.h"

#include <ProfileThread.h>
#include <ProfileScope.h>
#include <STLAllocator.h>

namespace profi {

ProfileThread::ProfileThread()
	: m_ActiveScope(nullptr)
{}

ProfileThread::~ProfileThread()
{}

void ProfileThread::EnterScope(const char* name)
{
	auto& map = m_ActiveScope ? m_ActiveScope->Children() : m_Scopes;

	ProfileScope* profile = map.Get(name);
	if(!profile) {
		profile = profi_new(ProfileScope, name);
		map.Insert(name, profile);
	}
	m_ActiveScope = profile;
}

void ProfileThread::ExitScope(unsigned long long elapsedTime, ProfileScope* parentScope)
{
	m_ActiveScope->IncreaseTime(elapsedTime);
	m_ActiveScope = parentScope;
}

}
