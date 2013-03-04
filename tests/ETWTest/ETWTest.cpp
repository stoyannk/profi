#include "stdafx.h"

typedef std::vector<DWORD> ThreadsVec;
ThreadsVec g_Threads;

unsigned g_EventsCount = 0;
unsigned g_ThisProcessThreadEvents = 0;

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

void WINAPI ProcessEvent(PEVENT_RECORD event) {
	++g_EventsCount;

	wchar_t ClassGUID[50] = {0};


}

void StartTraceConsume(TRACEHANDLE& handle) {
	::ProcessTrace(&handle, 1, 0, 0);
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
	
	// Open the trace
	EVENT_TRACE_LOGFILE logDesc = {0};
	logDesc.LoggerName = KERNEL_LOGGER_NAME;
	logDesc.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME;
	logDesc.EventRecordCallback = &ProcessEvent;

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

	::Sleep(1);

	// Stop the session
	properties->LoggerNameOffset = 0;
	error = ::ControlTrace(session, KERNEL_LOGGER_NAME, properties, EVENT_TRACE_CONTROL_STOP);

	if(error != ERROR_SUCCESS) {
		std::cerr << "Unable to stop trace; error: " << error << std::endl;
	}

	std::cout << "Free buffers: " << properties->FreeBuffers << std::endl;
	std::cout << "Events lost: " << properties->EventsLost << std::endl;
	std::cout << "Buffers written: " << properties->BuffersWritten << std::endl;
	std::cout << "Log buffers lost: " << properties->LogBuffersLost << std::endl;
	std::cout << "RealTime buffers lost: " << properties->RealTimeBuffersLost << std::endl;

	error = ::CloseTrace(consumeHandle);
	if(error != ERROR_SUCCESS) {
		std::cerr << "Unable to close consume trace; error: " << error << std::endl;
	}

	consumerThread.join();
	std::cout << "Event received: " << g_EventsCount << std::endl;

	return 0;
}

