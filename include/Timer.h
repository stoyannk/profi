#pragma once

namespace profi {

class Timer {
public:
	static void Initialize();
	static unsigned long long Now();
private:
	static unsigned long long s_Frequency;

	Timer();
	Timer(const Timer&);
	Timer& operator=(const Timer&);
};

}