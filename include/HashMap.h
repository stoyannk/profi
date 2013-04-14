#pragma once

#include <profi_decls.h>
#include <ProfileScope.h>

//TODO: This is an experimental implementation with a normal hash map + lock; Provide custom high-perf impl
#include <unordered_map>
#include <boost/thread/mutex.hpp>

namespace profi {

class HashMap
{
	ProfileScope& GetOrInsert(const char* name);
	
private:
	//TODO: This is an experimental implementation with a normal hash map + lock; Provide custom high-perf impl
	std::unordered_map<const char*, ProfileScope> m_InternalMap;
	boost::mutex m_Mutex;
};

}