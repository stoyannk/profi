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
	
	namespace profi {
		void Initialize(IAllocator* allocator);
		void Deinitialize();

		IReport* GetReportJSON();
	}

#else

	#define PROFI_FUNC

#endif