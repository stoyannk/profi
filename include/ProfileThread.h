#pragma once

#include <Registry.h>
#include <ProfileScope.h>
#include <Timer.h>

namespace profi {

class ProfileThread : boost::noncopyable {
public:
	ProfileThread();
	
private:
	ProfileScope* m_ActiveScope;
	Timer m_Timer;
};

}