#pragma once
class UTime
{
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point currentTime;
	std::chrono::high_resolution_clock::time_point prevTime;
	double totalTime, sinceLast, lastSecond, framePerSecond;
	unsigned int elapsedSignal;
public:
	UTime();
	~UTime();

	void Signal();
	const double DeltaTime();
	const double TotalTime();
	const double TotalTimeExact();
	const double FramePerSecond();
	void Restart();
	void Throttle(double targetHz);
};

