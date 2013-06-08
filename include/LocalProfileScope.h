#pragma once

namespace profi {
class ProfileThread;
class ProfileScope;

class PROFI_EXPORT LocalProfileScope {
public:
	LocalProfileScope(const char* name);
	~LocalProfileScope();
	
private:
	unsigned long long m_StartTime;
	ProfileThread* m_MyThread;
	ProfileScope* m_Parent;
};

}