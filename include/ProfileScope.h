#pragma once

#include <HashMap.h>
#include <STLAllocator.h>

namespace profi {

class ProfileScope : Noncopyable {
public:
	explicit ProfileScope(const char* name, std::mutex& threadHashMutex);
	~ProfileScope();

	HashMap& Children();

	void IncreaseTimeAndCallCount(unsigned long long time);
	unsigned long long GetTime() const { return m_TotalTime; }
	unsigned GetCallCount() const { return m_CallCount; }
	
	const char* GetName() const { return m_Name; }

private:
	const char* m_Name;
	unsigned long long m_TotalTime;
	unsigned m_CallCount;

	HashMap m_ChildProfiles;
};

}