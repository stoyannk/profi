#include "precompiled.h"

#include <Timer.h>
#include <profi_decls.h>

#define UNKNOWN_BASELINE 0xFFFFFFFFFFFFFFFF

namespace profi {

	unsigned Timer::s_BaseLine = UNKNOWN_BASELINE;

	Timer::Timer()
		: m_Frequency(0)
	{
		GetBaseLine(); // warm-up CPUID
#if PROFI_TIMING_METHOD == PROFI_TIMER_QPC
		LARGE_INTEGER frequency;
		::QueryPerformanceFrequency(&frequency);
		m_Frequency = frequency.QuadPart;
#endif
	}

	unsigned long long Timer::Now()
	{
#if PROFI_TIMING_METHOD == PROFI_TIMER_QPC
		LARGE_INTEGER now;

#ifdef PROFI_TIMING_FLUSH_CPU
		int a[4], b = 0;
		__cpuid(a, b);
#endif

		::QueryPerformanceCounter(&now);

		return static_cast<unsigned long long>(((static_cast<double>(now.QuadPart) / m_Frequency) * 1000.0)); // ms
#elif PROFI_TIMING_METHOD == PROFI_TIMER_RDTSC
		unsigned cycles_high = 0, cycles_low = 0;

		__asm {
			pushad
#ifdef PROFI_TIMING_FLUSH_CPU
				CPUID
#endif
				RDTSC
				mov cycles_high, edx
				mov cycles_low, eax
				popad
		}
#else
#error Profi timing method not specified
#endif
	}

	unsigned Timer::GetBaseLine()
	{
		if(s_BaseLine == UNKNOWN_BASELINE)
		{
			//Taken Directly from Intel's docs
#ifdef PROFI_TIMING_FLUSH_CPU
			unsigned base, base_extra=0;
			unsigned cycles_low, cycles_high;
			// The following tests run the basic cycle counter to find
			// the overhead associated with each cycle measurement.
			// It is run multiple times simply because the first call
			// to CPUID normally takes longer than subsequent calls.
			// Typically after the second run the results are
			// consistent. It is run three times just to make sure.
			__asm {
					pushad
					CPUID
					RDTSC
					mov cycles_high, edx
					mov cycles_low, eax
					popad
					pushad
					CPUID
					RDTSC
					popad
					pushad
					CPUID
					RDTSC
					mov cycles_high, edx
					mov cycles_low, eax
					popad
					pushad
					CPUID
					RDTSC
					popad
					pushad
					CPUID
					RDTSC
					mov cycles_high, edx
					mov cycles_low, eax
					popad
					pushad
					CPUID
					RDTSC
					sub eax, cycles_low
					mov base_extra, eax
					popad
					pushad
					CPUID
					RDTSC
					mov cycles_high, edx
					mov cycles_low, eax
					popad
					pushad
					CPUID
					RDTSC
					sub eax, cycles_low
					mov base, eax
					popad
			} // End inline assembly
			// The following provides insurance for the above code,
			// in the instance the final test causes a miss to the
			// instruction cache.
			if (base_extra < base)
				base = base_extra;

			s_BaseLine = base;
#else
			s_BaseLine = 0;
#endif
		}

		return s_BaseLine;
	}

}
