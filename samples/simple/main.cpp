#include "precompiled.h"

int main()
{
	profi::DefaultAllocator allocator;
	profi::Initialize(&allocator);
	PROFI_FUNC

	profi::Deinitialize();
}
