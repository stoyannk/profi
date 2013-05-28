#include "precompiled.h"

#include <ProfileThread.h>
#include <ProfileScope.h>
#include <STLAllocator.h>

namespace profi {

ProfileThread::ProfileThread()
	: m_ActiveScope(nullptr)
{}

ProfileThread::~ProfileThread()
{
	std::for_each(m_Scopes.cbegin(), m_Scopes.cend(), [] (const HashMap::value_type& scope) {
		profi_delete(scope);
	});
}

void ProfileThread::EnterScope(const char* name)
{
	auto& map = m_ActiveScope ? m_ActiveScope->Children() : m_Scopes;

	ProfileScope* profile = map.Get(name);
	if(!profile) {
		profile = profi_new(ProfileScope, name);
		map.Insert(profile);
	}
	m_ActiveScope = profile;
}

void ProfileThread::ExitScope(unsigned long long elapsedTime, ProfileScope* parentScope)
{
	m_ActiveScope->IncreaseTimeAndCallCount(elapsedTime);
	m_ActiveScope = parentScope;
}

}
