#pragma once

#if (_MSC_VER >= 1600)
#define PROFI_OVERRIDE override

#else
#define PROFI_OVERRIDE

#endif

#define PROFI_TIMER_QPC 1
#define PROFI_TIMER_RDTSC 2

// CONFIGURATION
#define PROFI_ENABLE
#define PROFI_TIMING_METHOD PROFI_TIMER_QPC
#define PROFI_TIMING_FLUSH_CPU
