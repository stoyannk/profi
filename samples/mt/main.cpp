// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#include "precompiled.h"
#include <fstream>

#define SIMULATE_WORK
#ifdef SIMULATE_WORK
	#define SIMUL_SLEEP(TIME) std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
#else
	#define SIMUL_SLEEP(TIME)
#endif

void foo() {
	PROFI_FUNC
	SIMUL_SLEEP(500)
}

void recurse(unsigned times)
{
	PROFI_FUNC
	SIMUL_SLEEP(100)

	if(times == 0)
		return;
	recurse(--times);
}

std::atomic<bool> threadStart = false;

void thread_run1() {
	// wait for all threads to start
	while(!threadStart);
	PROFI_NAME_THREAD("Worker #1")
	PROFI_SCOPE("ThreadScope1")
	foo();
	SIMUL_SLEEP(200)
}

void thread_run2() {
	// wait for all threads to start
	while(!threadStart);
	PROFI_NAME_THREAD("Worker #2")
	PROFI_FUNC
	SIMUL_SLEEP(500)
	recurse(2);
}

#ifdef DEBUG
typedef profi::DebugMallocAllocator ProfiAllocator;
#else
typedef profi::DefaultMallocAllocator ProfiAllocator;
#endif

int main()
{
	ProfiAllocator allocator;
	profi::Initialize(&allocator);
	{
		PROFI_NAME_THREAD("MainThread")
		PROFI_FUNC
		{
			PROFI_SCOPE("StartThreads")
			std::thread t1(&thread_run1);
			std::thread t2(&thread_run2);

			threadStart = true;

			t1.join();
			t2.join();
		}
	}

	// dump the report
	profi::IReport* report(profi::GetReportJSON());
	
	std::ofstream fout("output.json");
	fout.write((const char*)report->Data(), report->Size());

	report->Release();
	profi::Deinitialize();
}
