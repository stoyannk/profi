#include "precompiled.h"

#include <Registry.h>
#include <ScopeExit.h>

#include <stdlib.h>

namespace profi {

Registry* Registry::s_Instance = nullptr;

IAllocator* GetGlobalAllocator()
{
	return Registry::Get()->GetAllocator();
}

void* DefaultAllocator::Allocate(size_t size) {
	return malloc(size);
}

void DefaultAllocator::Deallocate(void* ptr) {
	free(ptr);
}

DefaultAllocator::DefaultAllocator()
{}

DefaultAllocator::~DefaultAllocator()
{}
  
void Registry::Initialize(IAllocator* allocator) {
	auto instance = static_cast<Registry*>(allocator->Allocate(sizeof(Registry)));
	auto ex = make_scope_exit([instance, allocator] {
		allocator->Deallocate(instance);
	});
	new(instance) Registry(allocator);
	ex.dismiss();

	s_Instance = instance;
}

void Registry::Deinitialize() {
	s_Instance->~Registry();
	s_Instance->m_Allocator->Deallocate(s_Instance);
	s_Instance = nullptr;
}

Registry::Registry(IAllocator* allocator)
	: m_Allocator(allocator) {
}

Registry::~Registry() {

}

void Initialize(IAllocator* allocator) {
	Registry::Initialize(allocator);
}

void Deinitialize() {
	Registry::Deinitialize();
}

}