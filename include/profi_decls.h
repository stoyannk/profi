// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

#if (_MSC_VER >= 1600)
#define PROFI_OVERRIDE override

#else
#define PROFI_OVERRIDE

#endif

#ifdef _MSC_VER
	#ifdef PROFI_BUILD
	#define PROFI_EXPORTED_SYMBOL __declspec(dllexport)
	#else
	#define PROFI_EXPORTED_SYMBOL __declspec(dllimport)
	#endif
#endif

#if defined(PROFI_DYNAMIC_LINK)
#define PROFI_EXPORT PROFI_EXPORTED_SYMBOL
#else
#define PROFI_EXPORT
#endif

#define PROFI_TIMER_QPC 1
#define PROFI_TIMER_RDTSC 2

#define PROFI_SCOPE_HASH_ADDRESS 1

#define PROFI_LINEAR_ALLOCATOR 1
#define PROFI_TRANSPARENT_ALLOCATOR 2

// CONFIGURATION
#define PROFI_TIMING_METHOD PROFI_TIMER_QPC
#define PROFI_TIMING_FLUSH_CPU
#define PROFI_ALLOCATOR	PROFI_LINEAR_ALLOCATOR
#define PROFI_SCOPE_HASH PROFI_SCOPE_HASH_ADDRESS

#define PROFI_ENABLE
#define PROFI_MAX_SEVERITY 3
