// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

#include <profi_decls.h>

#define PROFI_TIMER_QPC 1
#define PROFI_TIMER_RDTSC 2

#define PROFI_SCOPE_HASH_ADDRESS 1

#define PROFI_LINEAR_ALLOCATOR 1
#define PROFI_TRANSPARENT_ALLOCATOR 2

// CONFIGURATION
#define PROFI_TIMING_METHOD PROFI_TIMER_QPC

#if !defined(PROFI_X64)
#define PROFI_TIMING_FLUSH_CPU
#endif

#define PROFI_ALLOCATOR	PROFI_LINEAR_ALLOCATOR
#define PROFI_SCOPE_HASH PROFI_SCOPE_HASH_ADDRESS

#define PROFI_MAX_SEVERITY 3
