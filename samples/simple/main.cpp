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
	PROFI_FUNC_S1
	SIMUL_SLEEP(100)

	if(times == 0)
		return;
	recurse(--times);
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
		PROFI_FUNC
		{
			for(int i = 0; i < 2; ++i) {
				PROFI_SCOPE("TestScope")
				foo();
				
				recurse(3);
			}
		}
	}

	// dump the report
	profi::IReport* report(profi::GetReportJSON());
	
	std::ofstream fout("output.json");
	fout.write((const char*)report->Data(), report->Size());

	report->Release();
	profi::Deinitialize();
}
