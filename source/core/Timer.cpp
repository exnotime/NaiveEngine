#include "Timer.h"
#include <GLFW/glfw3.h>
Timer::Timer() {
	m_Start = glfwGetTime();
	m_LastTick = 0;
}

Timer::~Timer() {

}

double Timer::Tick() {
	double time = glfwGetTime();
	double t = time - m_LastTick;
	m_LastTick = time;
	return t;
}

double Timer::Reset() {
	double time = glfwGetTime();
	double t = time - m_Start;
	m_Start = time;
	return t;
}