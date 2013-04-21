#pragma once

#include <HashMap.h>

namespace profi {

class ProfileScope : boost::noncopyable {
public:
	ProfileScope(); // for hash map

	explicit ProfileScope(const char* name);
	~ProfileScope();

	HashMap& Children();

	void IncreaseTime(unsigned long long time);
	unsigned long long GetTime() const;
	
	const char* GetName() const { return m_Name; }

private:
	const char* m_Name;
	unsigned long long m_TotalTime;

	HashMap m_ChildProfiles;
};

}