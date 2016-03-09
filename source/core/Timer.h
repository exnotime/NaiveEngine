#pragma once
class Timer {
public:
	Timer();
	~Timer();
	double Tick();
	double Reset();

private:
	double m_Start;
	double m_LastTick;
};