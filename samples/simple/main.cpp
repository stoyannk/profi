#include "precompiled.h"
#include <fstream>

void foo() {
	PROFI_FUNC
}

int main()
{
	profi::DefaultAllocator allocator;
	profi::Initialize(&allocator);
	{
		PROFI_FUNC
		foo();
	}

	// dump the report
	profi::IReport* report(profi::GetReportJSON());
	
	std::ofstream fout("output.json");
	fout.write((const char*)report->Data(), report->Size());

	report->Release();
	profi::Deinitialize();
}
