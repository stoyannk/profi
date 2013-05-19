#pragma once

#include <profi_decls.h>

//TODO: This is an experimental implementation with a normal hash map + lock; Provide custom high-perf impl
#include <unordered_map>

namespace profi {

class ProfileScope;

class HashMap
{
public:
	typedef std::unordered_map<const char*, ProfileScope*> InternalMap;

	HashMap();
	HashMap(const HashMap& other);
	HashMap& operator=(const HashMap& other);

	ProfileScope* Get(const char* name);
	// Takes ownership
	bool Insert(ProfileScope* scope);
	
	InternalMap::const_iterator cbegin() const { return m_InternalMap.cbegin(); };
	InternalMap::const_iterator cend() const { return m_InternalMap.cend(); };
	size_t size() const { return m_InternalMap.size(); }

private:
	//TODO: This is an experimental implementation with a normal hash map + lock; Provide custom high-perf impl
	InternalMap m_InternalMap;
	mutable std::mutex m_Mutex;
};

}