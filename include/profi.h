#pragma once
#include <profi_decls.h>

#ifdef PROFI_ENABLE
	#include <IAllocator.h>
	#include <LocalProfileScope.h>
	
	#ifdef _MSC_VER
	#define __PRETTY_FUNCTION__ __FUNCSIG__
	#endif
	
	#define PROFI_FUNC profi::LocalProfileScope lps_##__LINE__##(__PRETTY_FUNCTION__);

	namespace profi {
		void Initialize(IAllocator* allocator);
		void Deinitialize();
	}

#else

	#define PROFI_FUNC

#endif