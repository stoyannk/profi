// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

namespace profi {

class Timer : Noncopyable {
public:
	Timer();
	unsigned long long Now() const;

	static unsigned GetBaseLine();

private:
	unsigned long long m_Frequency;

	static unsigned s_BaseLine;
};

}