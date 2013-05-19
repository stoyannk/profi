#pragma once
#include <profi_decls.h>

#include <IAllocator.h>
#include <STLAllocator.h>
#include <Reports.h>

namespace profi {

class ProfileThread;

class Registry : Noncopyable {
public:
	static void Initialize(IAllocator* allocator);
	static void Deinitialize();

	static Registry* Get() {
		return s_Instance;
	}

	IAllocator* GetAllocator() const {
		return s_Allocator;
	}

	ProfileThread* GetOrRegisterThreadProfile();
	
	IReport* DumpDataJSON();
	
private:
	static Registry* s_Instance;

	Registry(IAllocator* allocator);
	~Registry();

	std::mutex m_ThreadsMutex;
	typedef std::vector<ProfileThread*, STLAllocator<ProfileThread*>> ProfileThreadsVec;
	ProfileThreadsVec m_ProfiledThreads;

	static thread_local ProfileThread* m_TLSProfiles;

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
};


}