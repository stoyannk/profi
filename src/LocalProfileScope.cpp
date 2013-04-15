#include "precompiled.h"

#include <LocalProfileScope.h>

namespace profi {

LocalProfileScope::LocalProfileScope(const char* name)
	: m_StartTime(0)
{}

LocalProfileScope::~LocalProfileScope()
{}
  
}
