// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

#include <profi_config.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <list>
#include <sstream>
#include <algorithm>
#include <atomic>

class Noncopyable
{
	Noncopyable(const Noncopyable&);
	Noncopyable& operator=(const Noncopyable&);
protected:
	Noncopyable(){}
	~Noncopyable(){}
};

#ifdef _MSC_VER
#define thread_local __declspec(thread)
#endif
