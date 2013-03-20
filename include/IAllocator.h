#pragma once

namespace profi {

class IAllocator {
public:
	virtual void* Allocate(size_t size) = 0;
	virtual void Deallocate(void* ptr) = 0;
};

}