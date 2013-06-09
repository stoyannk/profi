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

	namespace profi {
		PROFI_EXPORT void Initialize(IAllocator* allocator);
		PROFI_EXPORT void Deinitialize();

		PROFI_EXPORT IReport* GetReportJSON();
		PROFI_EXPORT unsigned GetTimerBaseLine();
		PROFI_EXPORT void NameThread(const char* name);
	}

#else

	#define PROFI_FUNC
	#define PROFI_SCOPE(NAME)
	#define PROFI_NAME_THREAD(NAME)

#endif