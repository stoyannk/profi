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
	SIMUL_SLEEP(1000)
}

void recurse(unsigned times)
{
	PROFI_FUNC
	SIMUL_SLEEP(500)

	if(times == 0)
		return;
	recurse(--times);
}

int main()
{
	profi::DefaultAllocator allocator;
	profi::Initialize(&allocator);
	{
		PROFI_FUNC
		{
			PROFI_SCOPE("TestScope")
			foo();
			
			recurse(3);
		}
	}

	// dump the report
	profi::IReport* report(profi::GetReportJSON());
	
	std::ofstream fout("output.json");
	fout.write((const char*)report->Data(), report->Size());

	report->Release();
	profi::Deinitialize();
}
