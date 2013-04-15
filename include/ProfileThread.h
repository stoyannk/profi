#pragma once

#include <Registry.h>
#include <HashMap.h>
#include <Timer.h>

namespace profi {

class ProfileThread : boost::noncopyable {
public:
	ProfileThread();
	~ProfileThread();
	
	void EnterScope(const char* name);
	void ExitScope(unsigned long long elapsedTime);

private:
	ProfileScope* m_ActiveScope;
	Timer m_Timer;

	HashMap m_Scopes;
};

}