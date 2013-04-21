#pragma once

#include <HashMap.h>
#include <Timer.h>

namespace profi {

class ProfileThread : boost::noncopyable {
public:
	ProfileThread();
	~ProfileThread();
	
	void EnterScope(const char* name);
	void ExitScope(unsigned long long elapsedTime, ProfileScope* parentScope);

	const Timer& GetTimer() const { return m_Timer; }
	ProfileScope* GetActiveScope() const { return m_ActiveScope; }

	const HashMap& GetScopes() const { return m_Scopes; };

private:
	ProfileScope* m_ActiveScope;
	Timer m_Timer;

	HashMap m_Scopes;
};

}