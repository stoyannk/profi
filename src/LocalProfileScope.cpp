// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#include "precompiled.h"

#include <LocalProfileScope.h>
#include <Registry.h>
#include <ProfileThread.h>

namespace profi {

LocalProfileScope::LocalProfileScope(const char* name)
	: m_StartTime(0)
	, m_MyThread(Registry::Get()->GetOrRegisterThreadProfile())
{
	m_Parent = m_MyThread->GetActiveScope();
	m_MyThread->EnterScope(name);

	m_StartTime = m_MyThread->GetTimer().Now();
}

LocalProfileScope::~LocalProfileScope()
{
	m_MyThread->ExitScope(m_MyThread->GetTimer().Now() - m_StartTime, m_Parent);
}
  
}
