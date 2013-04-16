#pragma once

#include <profi_decls.h>

//TODO: This is an experimental implementation with a normal hash map + lock; Provide custom high-perf impl
#include <unordered_map>

namespace profi {

class ProfileScope;

class HashMap : boost::noncopyable
{
public:
	ProfileScope* Get(const char* name);
	bool Insert(const char* name, ProfileScope* scope);
	
private:
	//TODO: This is an experimental implementation with a normal hash map + lock; Provide custom high-perf impl
	std::unordered_map<const char*, ProfileScope*> m_InternalMap;
	std::mutex m_Mutex;
};

}