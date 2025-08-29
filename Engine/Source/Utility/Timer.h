#pragma once
#include <iostream>
#include <string>
#include <chrono>

////////////////////

class Timer
{
public:
	Timer() {}

	~Timer() {}

	void Start() { m_Start = std::chrono::high_resolution_clock::now(); }

	float ElapsedSeconds() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() / 1000000000.f; }
	float ElapsedMilliseconds() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() / 1000000.f; }
	float ElapsedNanoseconds() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 1.f; }

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

////////////////////

class ScopedTimer
{
public:
	ScopedTimer(const std::string& name = "", bool consoleOutput = true)
		: m_Name(name), m_ConsoleOutput(consoleOutput)
	{
		m_Timer.Start();
	}

	~ScopedTimer()
	{
		if (m_ConsoleOutput) std::cout << "TIME " << m_Name << " - " << m_Timer.ElapsedMilliseconds() << "ms\n";
	}

private:
	Timer m_Timer;
	std::string m_Name;
	bool m_ConsoleOutput = true;
};