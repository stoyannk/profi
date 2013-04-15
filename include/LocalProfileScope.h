#pragma once

namespace profi {

class LocalProfileScope {
public:
	LocalProfileScope(const char* name);
	~LocalProfileScope();
	
private:
	unsigned long long m_StartTime;
};

}