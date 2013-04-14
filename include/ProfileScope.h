#pragma once

namespace profi {

class ProfileScope {
public:
	ProfileScope(const char* name);
	
private:
	const char* m_Name;
};

}