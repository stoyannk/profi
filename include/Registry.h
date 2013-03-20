﻿#pragma once
#include <profi_decls.h>

#include <IAllocator.h>

namespace profi {

class DefaultAllocator : public IAllocator, boost::noncopyable {
	virtual void* Allocate(size_t size) PROFI_OVERRIDE;
	virtual void Deallocate(void* ptr) PROFI_OVERRIDE;
};

class Registry : boost::noncopyable {
public:
	static void Initialize(IAllocator* allocator);
	static void Deinitialize();

	static Registry* Get() {
		return s_Instance;
	}

	IAllocator* GetAllocator() const {
		return m_Allocator;
	}

private:
	static Registry* s_Instance;

	Registry(IAllocator* allocator);
	~Registry();

private:
	IAllocator* m_Allocator;
};

template<typename T>
struct profi_deleter 
{
	void operator()(T* ptr) {
		ptr->~T();
		Registry::Get()->GetAllocator()->Deallocate(ptr);
	}
};

}