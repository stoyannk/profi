#pragma once

#include <profi_decls.h>
#include <cassert>

namespace profi {

// Must provide thread-safe allocation
class IAllocator {
public:
	virtual void* Allocate(size_t size) = 0;
	virtual void Deallocate(void* ptr) = 0;
};

class CountMallocPolicy
{
public:
	CountMallocPolicy() : m_Allocated(0u) {}
	~CountMallocPolicy() 
	{
		assert(!m_Allocated && "Memory leaked!");
	}

	void Allocated(void* ptr, size_t)
	{
		// get the real allocation size
		#ifdef _MSC_VER
		m_Allocated += _msize(ptr);
		#endif
	}

	void Deallocate(void* ptr)
	{
		// get the real allocation size
		#ifdef _MSC_VER
		m_Allocated -= _msize(ptr);
		#endif
	}

private:
	CountMallocPolicy(const CountMallocPolicy&);
	CountMallocPolicy& operator=(const CountMallocPolicy&);

	unsigned m_Allocated;
};

class NoCountPolicy
{
public:
	NoCountPolicy() {}
	void Allocated(void* ptr, size_t size) {}
	void Deallocate(void* ptr) {}
private:
	NoCountPolicy(const NoCountPolicy&);
	NoCountPolicy& operator=(const NoCountPolicy&);
};

template<typename CountPolicy>
class DefaultAllocator : public IAllocator, public CountPolicy {
public:
	DefaultAllocator();
	virtual ~DefaultAllocator();

	virtual void* Allocate(size_t size) PROFI_OVERRIDE;
	virtual void Deallocate(void* ptr) PROFI_OVERRIDE;
private:
	DefaultAllocator(const DefaultAllocator&);
	DefaultAllocator& operator=(const DefaultAllocator&);
};

template<typename CountPolicy>
void* DefaultAllocator<CountPolicy>::Allocate(size_t size) {
	auto ptr = malloc(size);
	CountPolicy::Allocated(ptr, size);
	return ptr;
}

template<typename CountPolicy>
void DefaultAllocator<CountPolicy>::Deallocate(void* ptr) {
	CountPolicy::Deallocate(ptr);
	free(ptr);
}

template<typename CountPolicy>
DefaultAllocator<CountPolicy>::DefaultAllocator()
{}

template<typename CountPolicy>
DefaultAllocator<CountPolicy>::~DefaultAllocator()
{}

typedef DefaultAllocator<CountMallocPolicy>	DebugMallocAllocator;
typedef DefaultAllocator<CountMallocPolicy>	DefaultMallocAllocator;

}