#pragma once
#include <profi_decls.h>

#ifdef PROFI_ENABLE
	#include <IAllocator.h>
	#include <LocalProfileScope.h>
	#include <Reports.h>

	#ifdef _MSC_VER
		#define __PRETTY_FUNCTION__ __FUNCSIG__
	#endif
	
	#define PROFI_FUNC profi::LocalProfileScope lps_##__LINE__##(__PRETTY_FUNCTION__);
	#define PROFI_SCOPE(NAME) profi::LocalProfileScope lpss_##__LINE__##(NAME);
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
}
