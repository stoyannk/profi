// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once
#include <profi_decls.h>

#include <IAllocator.h>
#include <STLAllocator.h>
#include <Reports.h>

namespace profi {

class ProfileThread;

class InternalAllocator : public IAllocator
{
public:
	InternalAllocator(IAllocator* externalAllocator);
	virtual ~InternalAllocator();

	virtual void* Allocate(size_t size) override;
	virtual void Deallocate(void* ptr) override;

	void ReleaseAllMemory();

private:
	IAllocator* m_ExternalAllocator;
	class Page
	{
	public:
		static Page* Allocate(IAllocator* allocator, size_t sz);
		~Page();
		
		size_t GetSize() const { return m_Size; }
		Page*& GetPrevious() { return m_Previous; }
		char* GetStartPtr() const { return static_cast<char*>(m_Memory) + sizeof(Page); }
		void* GetMemory() const { return m_Memory; };

		std::atomic<char*> CurrentPtr;
		
	private:
		Page(void* memory, size_t sz);

		void* m_Memory;	// real adress memory WITH the instance data
		size_t m_Size; // size WITHOUT the instance data
		Page* m_Previous;
	};
	Page* m_Tail;
	std::mutex m_PagesMutex;
};

class Registry : Noncopyable {
public:
	static void Initialize(IAllocator* allocator);
	static void Deinitialize();

	static Registry* Get() {
		return s_Instance;
	}

	IAllocator* GetAllocator() const {
		return s_InternalAllocator;
	}

	ProfileThread* GetOrRegisterThreadProfile();
	
	void ResetProfiles();
	IReport* DumpDataJSON();

	void PauseProfiling()
	{
		m_IsActive = true;
	}

	void ResumeProfiling()
	{
		m_IsActive = false;
	}

	bool IsActive() const
	{
		return m_IsActive;
	}

	static thread_local const char* ThreadNames;

private:
	static Registry* s_Instance;

	Registry(IAllocator* allocator);
	~Registry();

	std::mutex m_ThreadsMutex;
	typedef std::vector<ProfileThread*, STLAllocator<ProfileThread*>> ProfileThreadsVec;
	ProfileThreadsVec m_ProfiledThreads;
	typedef std::list<pstring, STLAllocator<pstring>> ProfileNamesList;
	ProfileNamesList m_ThreadNames;

	static thread_local ProfileThread* m_TLSProfiles;

	std::atomic_bool m_IsActive;

private:
	class JSONReport : public IReport
	{
	public:
		JSONReport();

		virtual ~JSONReport();
		virtual const void* Data() override;
		virtual unsigned Size() override;
	
		virtual void Release() override;

		pstring& GetString();

	private:
		pstring m_Data;

	private:
		JSONReport(const JSONReport&);
		JSONReport& operator=(const JSONReport&);
	};

private:
	static IAllocator* s_Allocator;
	static IAllocator* s_InternalAllocator;
};


}