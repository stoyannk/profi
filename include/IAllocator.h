#pragma once

#include <profi_decls.h>

namespace profi {

class IAllocator {
public:
	virtual void* Allocate(size_t size) = 0;
	virtual void Deallocate(void* ptr) = 0;
};

class DefaultAllocator : public IAllocator {
public:
	DefaultAllocator();
	virtual ~DefaultAllocator();

	virtual void* Allocate(size_t size) PROFI_OVERRIDE;
	virtual void Deallocate(void* ptr) PROFI_OVERRIDE;
private:
	DefaultAllocator(const DefaultAllocator&);
	DefaultAllocator& operator=(const DefaultAllocator&);
};

}