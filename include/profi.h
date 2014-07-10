// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once
#include <profi_decls.h>
#include <IAllocator.h>
#include <LocalProfileScope.h>
#include <Reports.h>

#ifdef PROFI_ENABLE
	#ifdef _MSC_VER
		#define __PRETTY_FUNCTION__ __FUNCSIG__
	#endif

	#define PROFI_CONCAT(x, y) x ## y
	#define PROFI_CONCAT2(x, y) PROFI_CONCAT(x, y)
	
	#define PROFI_FUNC profi::LocalProfileScope PROFI_CONCAT2(lps_, __LINE__)(__PRETTY_FUNCTION__);
	#define PROFI_SCOPE(NAME) profi::LocalProfileScope PROFI_CONCAT2(lpss_, __LINE__)(NAME);
	#define PROFI_NAME_THREAD(NAME) profi::NameThread(NAME);

#else

	#define PROFI_FUNC
	#define PROFI_SCOPE(NAME)
	#define PROFI_NAME_THREAD(NAME)

#endif

#if defined PROFI_ENABLE && (PROFI_MAX_SEVERITY >= 1)
	#define PROFI_FUNC_S1 PROFI_FUNC
	#define PROFI_SCOPE_S1(NAME) PROFI_SCOPE(NAME)
#else
	#define PROFI_FUNC_S1
	#define PROFI_SCOPE_S1(NAME)
#endif
#if defined PROFI_ENABLE && (PROFI_MAX_SEVERITY >= 2)
	#define PROFI_FUNC_S2 PROFI_FUNC
	#define PROFI_SCOPE_S2(NAME) PROFI_SCOPE(NAME)
#else
	#define PROFI_FUNC_S2
	#define PROFI_SCOPE_S2(NAME)
#endif
#if defined PROFI_ENABLE && (PROFI_MAX_SEVERITY >= 3)
	#define PROFI_FUNC_S3 PROFI_FUNC
	#define PROFI_SCOPE_S3(NAME) PROFI_SCOPE(NAME)
#else
	#define PROFI_FUNC_S3
	#define PROFI_SCOPE_S3(NAME)
#endif


namespace profi {
	PROFI_EXPORT void Initialize(IAllocator* allocator);
	PROFI_EXPORT void Deinitialize();

	PROFI_EXPORT IReport* GetReportJSON();
	PROFI_EXPORT unsigned GetTimerBaseLine();
	PROFI_EXPORT void NameThread(const char* name);

	PROFI_EXPORT void PauseProfiling();
	PROFI_EXPORT void ResumeProfiling();
	
	PROFI_EXPORT void ResetProfiles();
}
