#pragma once
#include <iostream>
#include <string>
#include <chrono>

////////////////////

class Timer
{
public:
	Timer(const std::string& name = "", bool consoleOutput = true)
		: m_Name(name), m_ConsoleOutput(consoleOutput)
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (m_ConsoleOutput) std::cout << "[TIMER] " << m_Name << " - " << ElapsedMillis() << "ms\n";
	}

	float GetTime() { return ElapsedMillis(); }

	float Elapsed() { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f; }

	float ElapsedMillis() { return Elapsed() * 1000.0f; }

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	std::string m_Name;
	bool m_ConsoleOutput = true;
};