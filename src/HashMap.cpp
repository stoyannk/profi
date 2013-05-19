#include "precompiled.h"

#include <HashMap.h>
#include <ProfileScope.h>
#include <STLAllocator.h>

namespace profi
{

HashMap::HashMap()
{}

HashMap::HashMap(const HashMap& other)
{
	*this = other;
}

HashMap& HashMap::operator=(const HashMap& other)
{
	std::lock(m_Mutex, other.m_Mutex); // lock both mutexes simultaneously to avoid deadlocking
	// adopt locks to unlock them in case of an exception thrown later down
	std::lock_guard<std::mutex>(m_Mutex, std::adopt_lock);
	std::lock_guard<std::mutex>(other.m_Mutex, std::adopt_lock);

	m_InternalMap = other.m_InternalMap;

	return *this;
}

ProfileScope* HashMap::Get(const char* name) {
	std::lock_guard<std::mutex> lock(m_Mutex);
	auto it = m_InternalMap.find(name);
	
	return (it == m_InternalMap.end()) ? nullptr : it->second;
}

bool HashMap::Insert(ProfileScope* scope) {
	std::lock_guard<std::mutex> lock(m_Mutex);
	auto insRes = m_InternalMap.insert(std::make_pair(scope->GetName(), scope));

	return insRes.second;
}

}