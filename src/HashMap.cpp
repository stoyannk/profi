#include "precompiled.h"

#include <HashMap.h>
#include <ProfileScope.h>

namespace profi
{

ProfileScope* HashMap::Get(const char* name) {
	boost::unique_lock<boost::mutex> lock(m_Mutex);
	auto it = m_InternalMap.find(name);
	
	return (it == m_InternalMap.end()) ? nullptr : it->second;
}

bool HashMap::Insert(const char* name, ProfileScope* scope) {
	boost::unique_lock<boost::mutex> lock(m_Mutex);
	auto insRes = m_InternalMap.insert(std::make_pair(name, scope));

	return insRes.second;
}

}