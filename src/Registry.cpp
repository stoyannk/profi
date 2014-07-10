// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#include "precompiled.h"

#include <profi.h>
#include <Registry.h>
#include <ScopeExit.h>
#include <ProfileThread.h>
#include <ProfileScope.h>
#include <Timer.h>

#include <stdlib.h>

namespace profi {

Registry* Registry::s_Instance = nullptr;
IAllocator* Registry::s_Allocator = nullptr;
IAllocator* Registry::s_InternalAllocator = nullptr;

ProfileThread* Registry::m_TLSProfiles = nullptr;
const char* Registry::ThreadNames = nullptr;

InternalAllocator::Page* InternalAllocator::Page::Allocate(IAllocator* allocator, size_t sz)
{
	const size_t pageTotalSize = sz + sizeof(Page);
	Page* memory = static_cast<Page*>(allocator->Allocate(pageTotalSize));
	new(memory) Page(memory, sz);

	return memory;
}

InternalAllocator::Page::Page(void* memory, size_t sz)
	: m_Memory(memory)
	, m_Size(sz)
	, CurrentPtr(static_cast<char*>(memory) + sizeof(Page))
	, m_Previous(nullptr)
{}

InternalAllocator::Page::~Page()
{}

namespace {
	const size_t PAGE_SIZE = 1024 * 1024; // 1MB
}

InternalAllocator::InternalAllocator(IAllocator* externalAllocator)
	: m_ExternalAllocator(externalAllocator)
{
	// allocate a page
	Page* page = Page::Allocate(externalAllocator, PAGE_SIZE);
	
	std::lock_guard<std::mutex> l(m_PagesMutex);
	m_Tail = page;
}

InternalAllocator::~InternalAllocator()
{}

void* InternalAllocator::Allocate(size_t size)
{
	char* returnPtr = m_Tail->CurrentPtr;
	const auto roundedSize = size + (4 - size & 3);
	for(;;)
	{
		auto tail = m_Tail;
		returnPtr = tail->CurrentPtr;
		char* newPtr = returnPtr + roundedSize; // round to 4 bytes
		char* startPtr = tail->GetStartPtr();

		if(size_t(newPtr - startPtr) <= tail->GetSize()) {
			if(tail->CurrentPtr.compare_exchange_weak(returnPtr, newPtr))
			{
				break;
			}
		} else {
			// not enough memory - we need a new page
			{
				std::lock_guard<std::mutex> l(m_PagesMutex);
				if(tail == m_Tail) // check if tail hasn't changed in the mean time
				{
					Page* page = Page::Allocate(m_ExternalAllocator, std::max(PAGE_SIZE, roundedSize));
					page->GetPrevious() = tail;
					m_Tail = page;
				}
			}
		}
	}

	return returnPtr;
}

void InternalAllocator::Deallocate(void* ptr)
{
	// do nothing!
}

void InternalAllocator::ReleaseAllMemory()
{
	std::lock_guard<std::mutex> l(m_PagesMutex);
	// walk all pages backwards and kill them
	Page* next = m_Tail;
	while(next) {
		auto current = next;
		next = current->GetPrevious();
		current->~Page();
		m_ExternalAllocator->Deallocate(current->GetMemory());
	}
}
 
IAllocator* GetGlobalAllocator()
{
	return Registry::Get()->GetAllocator();
}
 
void Registry::Initialize(IAllocator* allocator) {
	s_Allocator = allocator;
	#if PROFI_ALLOCATOR == PROFI_LINEAR_ALLOCATOR
	// allocate the internal allocator (ugh)
	auto internalAllocator = static_cast<InternalAllocator*>(allocator->Allocate(sizeof(InternalAllocator)));
	new(internalAllocator) InternalAllocator(allocator);
	s_InternalAllocator = internalAllocator;
	#else
	s_InternalAllocator = s_Allocator;
	#endif

	auto instance = static_cast<Registry*>(s_InternalAllocator->Allocate(sizeof(Registry)));
	auto ex = make_scope_exit([&] {
		s_InternalAllocator->Deallocate(instance);
		s_Allocator = s_InternalAllocator = nullptr;
	});

	new(instance) Registry(allocator);
	ex.dismiss();

	s_Instance = instance;
}

void Registry::Deinitialize() {
	s_Instance->~Registry();
	s_Instance->s_InternalAllocator->Deallocate(s_Instance);
	s_Instance = nullptr;

	#if PROFI_ALLOCATOR == PROFI_LINEAR_ALLOCATOR
	static_cast<InternalAllocator*>(s_InternalAllocator)->ReleaseAllMemory();
	s_Allocator->Deallocate(s_InternalAllocator);
	#endif
}

Registry::Registry(IAllocator* allocator)
{
	m_IsActive.store(true);
}

Registry::~Registry() {
	std::for_each(m_ProfiledThreads.begin(), m_ProfiledThreads.end(), [](ProfileThread* tlsData){
		profi_delete(tlsData);
	});
}

ProfileThread* Registry::GetOrRegisterThreadProfile() {
	if(!m_TLSProfiles) {
		opstringstream name;
		std::lock_guard<std::mutex> lock(m_ThreadsMutex);
		if(!ThreadNames) {
			name << "Thread " << m_ProfiledThreads.size();
		} else {
			name << ThreadNames;
		}
		m_ThreadNames.push_back(name.str());
		m_TLSProfiles = profi_new(ProfileThread, m_ThreadNames.back().c_str(), *profi_new(std::mutex));
		m_ProfiledThreads.push_back(m_TLSProfiles);
	}

	return m_TLSProfiles;
}

void Registry::ResetProfiles()
{
	ProfileThreadsVec allThreads;
	{
		std::lock_guard<std::mutex> lock(m_ThreadsMutex);
		allThreads = m_ProfiledThreads;
	}

	std::function<void (ProfileScope*)> resetProfilesRecursive = [&](ProfileScope* scope) {
		HashMap childrenCopy = scope->Children();
		std::for_each(childrenCopy.begin(), childrenCopy.end(), [&](ProfileScope* scope){
			resetProfilesRecursive(scope);
		});
		scope->ResetProfile();
	};

	std::for_each(allThreads.begin(), allThreads.end(), [&](ProfileThread* thread){
		resetProfilesRecursive(thread);
	});
}

struct indent {
	indent() : Level(0) {}
	unsigned Level;
};

struct indent_scope {
public:
	static const unsigned DEFAULT_INDENT = 2;
	explicit indent_scope(indent& in)
		: m_Indent(in)
	{
		m_Indent.Level += DEFAULT_INDENT;
	}
	~indent_scope() {
		m_Indent.Level -= DEFAULT_INDENT;
	}

private:
	indent& m_Indent;
};

std::ostream& operator<<(std::ostream& stream, const indent& val) {
    for(unsigned i = 0; i < val.Level; i++) {
        stream << " ";
    }
    return stream;
}

IReport* Registry::DumpDataJSON()
{
	ProfileThreadsVec allThreads;
	{
		std::lock_guard<std::mutex> lock(m_ThreadsMutex);
		allThreads = m_ProfiledThreads;
	}

	std::function<void (indent, const unsigned long long, ProfileScope*, unsigned&, opstringstream&)> dumpScope = [&] (indent in, const unsigned long long totalTime, ProfileScope* scope, unsigned& row_id, opstringstream& output) {
		const auto count = scope->GetCallCount();
		
		if(!count)
			return;

		indent_scope insc(in);
		output << in << "\"id\": " << row_id++ << "," << std::endl;
		output << in << "\"name\": \"" << scope->GetName() << "\", " << std::endl;
		HashMap childrenCopy = scope->Children();
		unsigned long long childrenTimes = 0;
		if(childrenCopy.size()) {
			output << in << "\"children\": [ " << std::endl;
			{
				indent_scope insc(in);
				for(auto scopeIt = childrenCopy.begin(); scopeIt != childrenCopy.end();) {
					childrenTimes += (*scopeIt)->GetTime();
					output << in << "{" << std::endl;
					dumpScope(in, totalTime, *scopeIt, row_id, output);
					output << in << "}" << (++scopeIt != childrenCopy.end() ? "," : "") << std::endl;
				}
			}
			output << in << "]," << std::endl;
		}
		const auto time = scope->GetTime();
		const auto excl_time = time - childrenTimes;
		output << in << "\"excl_time\": " << excl_time << "," << std::endl;
		output << in << "\"excl_time_perc\": " << (excl_time / double(totalTime)) * 100.0 << "," << std::endl;
		output << in << "\"time\": " << time /* not sync */ << "," << std::endl;
		output << in << "\"time_perc\": " << (time / double(totalTime)) * 100.0 /* not sync */ << "," << std::endl;
		output << in << "\"call_count\": " << count /* not sync */ << "," << std::endl;
		output << in << "\"avg_call_incl\": " << time / (double)count << "," << std::endl;
		output << in << "\"avg_call_excl\": " << excl_time / (double)count << std::endl;
	};

	unsigned threadId = 0;
	opstringstream ostream;
	indent in;
	unsigned row_id = 0;
	ostream << "[" << std::endl;
	indent_scope insc(in);
	unsigned long long totalTime = 0;
	for(auto threadIt = allThreads.cbegin(); threadIt != allThreads.cend();) {
		ostream << in << "{" << std::endl;
		totalTime = (*threadIt)->GetTime();
		dumpScope(in, totalTime, *threadIt, row_id, ostream);
		ostream << in << "}" << (++threadIt != allThreads.cend() ? "," : "") << std::endl;
	}
	ostream << "]" << std::endl;
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
	return unsigned(m_Data.size());
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

unsigned GetTimerBaseLine() {
	return Timer::GetBaseLine();
}

void NameThread(const char* name) {
	Registry::ThreadNames = name;
}

void ResetProfiles() {
	Registry::Get()->ResetProfiles();
}

void PauseProfiling() {
	Registry::Get()->PauseProfiling();
}

void ResumeProfiling() {
	Registry::Get()->ResumeProfiling();
}

}