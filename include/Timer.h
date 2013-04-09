#pragma once

namespace profi {

class Timer : boost::noncopyable {
public:
	Timer();
	unsigned long long Now();

	static unsigned GetBaseLine();

private:
	unsigned long long m_Frequency;

	static unsigned s_BaseLine;
};

}