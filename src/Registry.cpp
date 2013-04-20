#include "precompiled.h"

#include <Registry.h>
#include <ScopeExit.h>
#include <ProfileThread.h>

#include <stdlib.h>

namespace profi {

Registry* Registry::s_Instance = nullptr;
IAllocator* Registry::s_Allocator = nullptr;

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
	Registry::s_Allocator = allocator;
	auto instance = static_cast<Registry*>(allocator->Allocate(sizeof(Registry)));
	auto ex = make_scope_exit([instance, allocator] {
		allocator->Deallocate(instance);
		Registry::s_Allocator = nullptr;
	});
	new(instance) Registry(allocator);
	ex.dismiss();

	s_Instance = instance;
}

void Registry::Deinitialize() {
	s_Instance->~Registry();
	s_Instance->s_Allocator->Deallocate(s_Instance);
	s_Instance = nullptr;
}

Registry::Registry(IAllocator* allocator)
{}

Registry::~Registry() {

}

ProfileThread* Registry::GetOrRegisterThreadProfile() {
	auto tlsProfile = m_TLSProfiles.get();

	if(!tlsProfile) {
		tlsProfile = profi_new(ProfileThread);
		m_TLSProfiles.reset(tlsProfile);

		std::lock_guard<std::mutex> lock(m_ThreadsMutex);
		m_ProfiledThreads.push_back(tlsProfile);
	}

	return tlsProfile;
}

IReport* Registry::DumpDataJSON()
{
	ProfileThreadsVec allThreads;
	{
		std::lock_guard<std::mutex> lock(m_ThreadsMutex);
		allThreads = m_ProfiledThreads;
	}

	unsigned threadId = 0;
	opstringstream ostream;
	for(auto threadIt = allThreads.cbegin(); threadIt != allThreads.cend(); ++threadIt) {
		ostream << "\"Thread" << threadId << "\": {" << std::endl;
		 
		ostream << "}" << std::endl;
		++threadId;
	}

	std::unique_ptr<JSONReport, profi_deleter<JSONReport>> report(profi_new(JSONReport));

	report->GetString() = ostream.str();

	return report.release();
}

Registry::JSONReport::JSONReport()
{}

Registry::JSONReport::~JSONReport()
{}

void Registry::JSONReport::Release()
{
	profi_delete(this);
}

const void* Registry::JSONReport::Data()
{
	return m_Data.c_str();
}

unsigned Registry::JSONReport::Size()
{
	return m_Data.size();
}

pstring& Registry::JSONReport::GetString()
{
	return m_Data;
}
 
void Initialize(IAllocator* allocator) {
	Registry::Initialize(allocator);
}

void Deinitialize() {
	Registry::Deinitialize();
}

IReport* GetReportJSON() {
	return Registry::Get()->DumpDataJSON();
}

}