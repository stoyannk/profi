#include "precompiled.h"

#include <HashMap.h>

namespace profi
{

ProfileScope& HashMap::GetOrInsert(const char* name)
{
	boost::unique_lock<boost::mutex> lock(m_Mutex);
	return m_InternalMap[name];
}

}