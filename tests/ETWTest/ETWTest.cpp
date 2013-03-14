#include "stdafx.h"

typedef std::vector<DWORD> ThreadsVec;
ThreadsVec g_Threads;

unsigned g_EventsCount = 0;
unsigned g_ThreadEventsCount = 0;
unsigned g_CSwitchEventsCount = 0;
unsigned g_ThisProcessThreadEvents = 0;

unsigned g_ThisProcessThreadsScheduled = 0;
unsigned g_ThisProcessThreadsUnscheduled = 0;

unsigned g_ThisProcessThreadsWaits = 0;

static const wchar_t* THREAD_V2_CLSID(L"{3d6fa8d1-fe05-11d0-9dda-00c04fd7ba7c}");
static const unsigned CSWITCH_EVENT_TYPE = 36;

template<typename T>
struct default_release
{
	void operator()(T* ptr) const {
		ptr->Release();
	}
};

struct free_variant
{
	void operator()(VARIANT* ptr) const {
		VariantClear(ptr);
		delete ptr;
	}
};

std::unique_ptr<IWbemClassObject, default_release<IWbemClassObject>> g_EventCategoryClass;

bool ListThreads(DWORD procId, ThreadsVec& outThreads) {
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32 = {0}; 
 
	// Take a snapshot of all running threads  
	hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
	if(hThreadSnap == INVALID_HANDLE_VALUE) {
		return false;
	}
	
	te32.dwSize = sizeof(THREADENTRY32); 
	
	if(!Thread32First(hThreadSnap, &te32)) 
	{
		std::cerr << "Unable to retrieve threads" << std::endl;
		::CloseHandle(hThreadSnap);
		return false;
	}
	
	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do 
	{ 
		if(te32.th32OwnerProcessID == procId) {
			outThreads.push_back(te32.th32ThreadID);
		}
	} while(Thread32Next(hThreadSnap, &te32 ));
	
	::CloseHandle(hThreadSnap);
	return true;;
}

struct CSWitchEventData
{
	CSWitchEventData(char* ptr) {
		FillData(ptr);
	}

	unsigned NewThreadId;
	unsigned OldThreadId;
	char  NewThreadPriority;
	char  OldThreadPriority;
	unsigned char PreviousCState;
	char  SpareByte;
	char  OldThreadWaitReason;
	char  OldThreadWaitMode;
	char  OldThreadState;
	char  OldThreadWaitIdealProcessor;
	unsigned NewThreadWaitTime;
	unsigned Reserved;

	void FillData(char* ptr) {
		NewThreadId = *(unsigned*)ptr; ptr += sizeof(unsigned);
		OldThreadId = *(unsigned*)ptr; ptr += sizeof(unsigned);
		NewThreadPriority = *(char*)ptr; ptr += sizeof(char);
		OldThreadPriority = *(char*)ptr; ptr += sizeof(char);
		PreviousCState = *(unsigned char*)ptr; ptr += sizeof(unsigned char);
		SpareByte = *(char*)ptr; ptr += sizeof(char);
		OldThreadWaitReason = *(char*)ptr; ptr += sizeof(char);
		OldThreadWaitMode = *(char*)ptr; ptr += sizeof(char);
		OldThreadState = *(char*)ptr; ptr += sizeof(char);
		OldThreadWaitIdealProcessor = *(char*)ptr; ptr += sizeof(char);
		NewThreadWaitTime = *(unsigned*)ptr; ptr += sizeof(unsigned);
		Reserved = *(unsigned*)ptr; ptr += sizeof(unsigned);
	}
};

void WINAPI ProcessEvent(PEVENT_TRACE event) {
	++g_EventsCount;

	if(event->MofLength == 0)
		return;

	wchar_t ClassGUID[50] = {0};
	StringFromGUID2(event->Header.Guid, ClassGUID, sizeof(ClassGUID));
		
	// we are interested in Thread_V2 class events
	if(lstrcmpiW(ClassGUID, THREAD_V2_CLSID) == 0 && event->Header.Class.Version)
	{
		++g_ThreadEventsCount;

		if(event->Header.Class.Type == CSWITCH_EVENT_TYPE)
		{
			++g_CSwitchEventsCount;

			CSWitchEventData evData((char*)event->MofData);

			auto oldThread = std::find(g_Threads.cbegin(), g_Threads.cend(), evData.OldThreadId);
			auto newThread = std::find(g_Threads.cbegin(), g_Threads.cend(), evData.NewThreadId);
			if(oldThread != g_Threads.cend() || newThread != g_Threads.cend()) {
				++g_ThisProcessThreadEvents;

				if(oldThread != g_Threads.cend()) {
					++g_ThisProcessThreadsUnscheduled;

					if(evData.OldThreadState == 5/*waiting*/) {
						++g_ThisProcessThreadsWaits;
					}
				}
				else {
					++g_ThisProcessThreadsScheduled;
				}
			}
		}
	}
}

void StartTraceConsume(TRACEHANDLE& handle) {
	::ProcessTrace(&handle, 1, 0, 0);
}

bool GetEventClass()
{
    HRESULT hr = S_OK;
   
    hr = CoInitialize(0);

	IWbemLocator* locatorTmp = nullptr;
    hr = CoCreateInstance(__uuidof(WbemLocator),
        0,
        CLSCTX_INPROC_SERVER,
        __uuidof(IWbemLocator),
        (LPVOID*)&locatorTmp);
	std::unique_ptr<IWbemLocator, default_release<IWbemLocator>> pLocator(locatorTmp);
	
    if (FAILED(hr)) {
        std::cerr << "CoCreateInstance failed with " << hr <<std::endl;
        return false;
    }
						
	IWbemServices* servicesTmp = nullptr;
	hr = pLocator->ConnectServer(L"root\\wmi",
        NULL, NULL, NULL,
        0L, NULL, NULL,
        &servicesTmp);
	std::unique_ptr<IWbemServices, default_release<IWbemServices>> service(servicesTmp);

    if (FAILED(hr)) {
        std::cerr << "ConnectServer failed with " << hr <<std::endl;
        return false;
    }

    hr = CoSetProxyBlanket(service.get(),
        RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE);

    if (FAILED(hr)) {
        std::cerr << "CoSetProxyBlanket failed with " << hr << std::endl;
		return false;
    }

	// Get the class category
	IEnumWbemClassObject* classesTmp = nullptr;
	hr = service->CreateClassEnum(_bstr_t(L"EventTrace"), 
        WBEM_FLAG_DEEP | WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_USE_AMENDED_QUALIFIERS,
        NULL, &classesTmp);
	std::unique_ptr<IEnumWbemClassObject, default_release<IEnumWbemClassObject>> classes(classesTmp);

	if(FAILED(hr)) {
		std::cerr << "CreateClassEnum failed with " << hr << std::endl;
		return false;
	}

	std::unique_ptr<IWbemClassObject, default_release<IWbemClassObject>> categoryClassObject;

	while (S_OK == hr) {
		ULONG cnt = 0;
		IWbemClassObject* clsTmp = nullptr;
		auto hrqual = classes->Next(WBEM_INFINITE, 1, &clsTmp, &cnt);
		std::unique_ptr<IWbemClassObject, default_release<IWbemClassObject>> cls(clsTmp);
		
		if(FAILED(hrqual)) {
			std::cerr << "Next category failed with " << hrqual << std::endl;
			return false;
		}

		IWbemQualifierSet* qualTmp = nullptr;
		cls->GetQualifierSet(&qualTmp);
		std::unique_ptr<IWbemQualifierSet, default_release<IWbemQualifierSet>> qualifiers(qualTmp);
		if(qualifiers) {
			std::unique_ptr<VARIANT, free_variant> varGuid(new VARIANT);
			hrqual = qualifiers->Get(L"Guid", 0, varGuid.get(), nullptr);
			if(FAILED(hrqual)) {
				continue;
			}

			if(lstrcmpiW(varGuid->bstrVal, THREAD_V2_CLSID) == 0) {
				categoryClassObject.reset(cls.release());
				break;
			}
		}
	}

	std::unique_ptr<VARIANT, free_variant> varClassName(new VARIANT);
	// Get the class we are interested in from the category
    hr = categoryClassObject->Get(L"__RELPATH", 0, varClassName.get(), nullptr, nullptr);
	if(FAILED(hr)) {
		std::cerr << "Unable to get relpath " << hr << std::endl;
		return false;
	}

	hr = service->CreateClassEnum(varClassName->bstrVal, 
        WBEM_FLAG_SHALLOW | WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_USE_AMENDED_QUALIFIERS,
        NULL, &classesTmp);
	classes.reset(classesTmp);
	if(FAILED(hr)) {
		std::cerr << "Unable to get classes for category " << hr << std::endl;
		return false;
	}

	bool found = false;
	std::unique_ptr<IWbemClassObject, default_release<IWbemClassObject>> cls;
	while(S_OK == hr) {
		ULONG cnt = 0;
		IWbemClassObject* clsTmp = nullptr;
		auto hrqual = classes->Next(WBEM_INFINITE, 1, &clsTmp, &cnt);
		cls.reset(clsTmp);

		if(FAILED(hrqual)) {
			std::cerr << "Next class failed with " << hrqual << std::endl;
			return false;
		}

		IWbemQualifierSet* qualTmp = nullptr;
		cls->GetQualifierSet(&qualTmp);
		std::unique_ptr<IWbemQualifierSet, default_release<IWbemQualifierSet>> qualifiers(qualTmp);

		std::unique_ptr<VARIANT, free_variant> varEventType(new VARIANT);
		qualifiers->Get(L"EventType", 0, varEventType.get(), nullptr);

		if (varEventType->vt & VT_ARRAY) {
            HRESULT hrSafe = S_OK;
            int ClassEventType;
            SAFEARRAY* pEventTypes = varEventType->parray;

            for (LONG i=0; (ULONG)i < pEventTypes->rgsabound->cElements; i++) {
                hrSafe = SafeArrayGetElement(pEventTypes, &i, &ClassEventType);

                if (ClassEventType == CSWITCH_EVENT_TYPE) {
                    found = true;
                    break;  //for loop
                }
            }
        } 
		else {
            if (varEventType->intVal == CSWITCH_EVENT_TYPE) {
                found = true;
            }
        }
		if (found) {
            break;  //while loop
        }
	}

	if(found) {
		g_EventCategoryClass = std::move(cls);
	}

	return found;
}

int main(int argc, char* argv[])
{
	if(!ListThreads(GetCurrentProcessId(), g_Threads)) {
		return 1;
	}

	std::cout << "Threads belonging to process: " << std::endl;
	std::for_each(g_Threads.cbegin(), g_Threads.cend(), [](DWORD threadId) {
		std::cout << threadId << std::endl;
	});

	// get the event class we are interested in
	if(!GetEventClass()) {
		return 1;
	}

	// Open the trace
	EVENT_TRACE_LOGFILE logDesc = {0};
	logDesc.LoggerName = KERNEL_LOGGER_NAME;
	logDesc.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME;
	logDesc.EventCallback = &ProcessEvent;

	TRACEHANDLE consumeHandle = ::OpenTrace(&logDesc);

	if(consumeHandle == INVALID_PROCESSTRACE_HANDLE) {
		std::cerr << "Unable to open trace!" << std::endl;
		return 1;
	}

	auto propsSize = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(KERNEL_LOGGER_NAME);
	std::unique_ptr<unsigned char[]> evtPropertiesMem(new unsigned char[propsSize]);
	::memset(evtPropertiesMem.get(), 0, propsSize);
	PEVENT_TRACE_PROPERTIES properties = reinterpret_cast<PEVENT_TRACE_PROPERTIES>(evtPropertiesMem.get());
	properties->Wnode.BufferSize = propsSize;
	properties->Wnode.Guid = SystemTraceControlGuid;
	properties->Wnode.ClientContext = 1/*QPC*/;
	properties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
	properties->EnableFlags = EVENT_TRACE_FLAG_CSWITCH;
	properties->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
	properties->LogFileNameOffset = 0;
	properties->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);

	TRACEHANDLE session;
	auto error = ::StartTrace(&session, KERNEL_LOGGER_NAME, properties);
	if(error != ERROR_SUCCESS) {
		std::cerr << "Unable to start trace; error: " << error << std::endl;
	}
	std::thread consumerThread(std::bind(&StartTraceConsume, consumeHandle));

	::Sleep(1000);

	// Stop the session
	properties->LoggerNameOffset = 0;
	error = ::ControlTrace(session, KERNEL_LOGGER_NAME, properties, EVENT_TRACE_CONTROL_STOP);

	if(error != ERROR_SUCCESS) {
		std::cerr << "Unable to stop trace; error: " << error << std::endl;
	}

	error = ::CloseTrace(consumeHandle);
	if(error != ERROR_SUCCESS) {
		std::cerr << "Unable to close consume trace; error: " << error << std::endl;
	}

	std::cout << "Free buffers: " << properties->FreeBuffers << std::endl;
	std::cout << "Events lost: " << properties->EventsLost << std::endl;
	std::cout << "Buffers written: " << properties->BuffersWritten << std::endl;
	std::cout << "Log buffers lost: " << properties->LogBuffersLost << std::endl;
	std::cout << "RealTime buffers lost: " << properties->RealTimeBuffersLost << std::endl;

	consumerThread.join();
	std::cout << "Events received: " << g_EventsCount << std::endl;
	std::cout << "Thread events received: " << g_ThreadEventsCount << std::endl;
	std::cout << "CSwitch events received: " << g_CSwitchEventsCount << std::endl;
	std::cout << "Thread events for this process received: " << g_ThisProcessThreadEvents << std::endl;
	std::cout << "Scheduled thread events for this process received: " << g_ThisProcessThreadsScheduled << std::endl;
	std::cout << "Unscheduled thread events for this process received: " << g_ThisProcessThreadsUnscheduled << std::endl;

	std::cout << "Wait thread events for this process received: " << g_ThisProcessThreadsWaits << std::endl;
		
	return 0;
}

