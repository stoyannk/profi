#pragma once
#include <profi_decls.h>

#include <IAllocator.h>
#include <STLAllocator.h>

namespace profi {

class ProfileThread;

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

	ProfileThread* GetOrRegisterThreadProfile();
	
private:
	static Registry* s_Instance;

	Registry(IAllocator* allocator);
	~Registry();

	boost::mutex m_ThreadsMutex;
	typedef std::vector<ProfileThread*, STLAllocator<ProfileThread*>> ProfileThreadsVec;
	ProfileThreadsVec m_ProfiledThreads;

	boost::thread_specific_ptr<ProfileThread> m_TLSProfiles;

private:
	IAllocator* m_Allocator;
};


}