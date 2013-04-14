#pragma once

#include <Registry.h>
#include <HashMap.h>
#include <Timer.h>

namespace profi {

class ProfileThread : boost::noncopyable {
public:
	ProfileThread();
	
	void EnterScope(const char* name);
	void ExitScope();

private:
	ProfileScope* m_ActiveScope;
	Timer m_Timer;

	HashMap m_Scopes;
};

}