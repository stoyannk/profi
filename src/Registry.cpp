#include "precompiled.h"

#include <Registry.h>
#include <ScopeExit.h>
#include <ProfileThread.h>
#include <ProfileScope.h>

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

	std::function<void (indent, ProfileScope*, unsigned&, opstringstream&)> dumpScope = [&] (indent in, ProfileScope* scope, unsigned& row_id, opstringstream& output) {
		{
			indent_scope insc(in);
			output << in << "\"id\": " << row_id++ << "," << std::endl;
			output << in << "\"name\": \"" << scope->GetName() << "\", " << std::endl;
			HashMap childrenCopy = scope->Children();
			unsigned long long childrenTimes = 0;
			if(childrenCopy.size()) {
				output << in << "\"children\": [ " << std::endl;
				{
					indent_scope insc(in);
					for(auto scopeIt = childrenCopy.cbegin(); scopeIt != childrenCopy.cend();) {
						childrenTimes += scopeIt->second->GetTime();
						output << in << "{" << std::endl;
						dumpScope(in, scopeIt->second, row_id, output);
						output << in << "}" << (++scopeIt != childrenCopy.cend() ? "," : "") << std::endl;
					}
				}
				output << in << "]," << std::endl;
			}
			const auto time = scope->GetTime();
			const auto count = scope->GetCallCount();
			const auto excl_time = time - childrenTimes;
			output << in << "\"excl_time\": " << excl_time << "," << std::endl;
			output << in << "\"time\": " << time /* not sync */ << "," << std::endl;
			output << in << "\"call_count\": " << count /* not sync */ << "," << std::endl;
			output << in << "\"avg_call_incl\": " << time / (double)count << "," << std::endl;
			output << in << "\"avg_call_excl\": " << excl_time / (double)count << std::endl;
		}
	};

	unsigned threadId = 0;
	opstringstream ostream;
	indent in;
	unsigned row_id = 0;
	ostream << "[" << std::endl;
	indent_scope insc(in);
	for(auto threadIt = allThreads.cbegin(); threadIt != allThreads.cend(); ++threadIt) {
		ostream << in << "{" << std::endl;
		const auto& scopes = (*threadIt)->GetScopes();
		{
			indent_scope insc(in);
			ostream << in << "\"id\": " << row_id++ << "," << std::endl;
			ostream << in << "\"time\": \"\"," << std::endl;
			ostream << in << "\"excl_time\": \"\"," << std::endl;
			ostream << in << "\"call_count\": \"\"," << std::endl;
			ostream << in << "\"avg_call_incl\": \"\"," << std::endl;
			ostream << in << "\"avg_call_excl\": \"\"," << std::endl;

			ostream << in << "\"name\": " << "\"Thread " << threadId << "\"" << (scopes.size() ? "," : "") << std::endl;
			if(scopes.size()) {
				ostream << in << "\"children\": [ " << std::endl;
				{
					indent_scope insc(in);
					for(auto scopeIt = scopes.cbegin(); scopeIt != scopes.cend();) {
						ostream << in << "{" << std::endl;
						dumpScope(in, scopeIt->second, row_id, ostream);
						ostream << in << "}" << (++scopeIt != scopes.cend() ? "," : "") << std::endl;
					}
				}
				ostream << in << "] " << std::endl;
			}
		}
		ostream << in << "}" << ((threadIt+1 != allThreads.cend()) ? "," : "") << std::endl;
		++threadId;
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