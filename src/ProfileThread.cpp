#include "precompiled.h"

#include <ProfileThread.h>

namespace profi {

ProfileThread::ProfileThread()
	: m_ActiveScope(nullptr)
{}

ProfileThread::~ProfileThread()
{}

void ProfileThread::EnterScope(const char* name)
{
}

void ProfileThread::ExitScope(unsigned long long elapsedTime)
{
}

}
