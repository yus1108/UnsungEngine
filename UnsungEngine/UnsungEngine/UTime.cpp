#include "stdafx.h"
#include "UTime.h"

using namespace std::chrono;

UTime::UTime()
{
	startTime = high_resolution_clock::now();
	prevTime = startTime;
	elapsedSignal = 0;
	totalTime = sinceLast = lastSecond = framePerSecond = 0.0;
}


UTime::~UTime()
{
}

void UTime::Signal()
{
	prevTime = currentTime;
	currentTime = high_resolution_clock::now();
	duration<double, std::milli> time_span = currentTime - startTime;
	totalTime = time_span.count() / 1000.0;

	elapsedSignal++;
	double sinceLast = totalTime - lastSecond;
	if (sinceLast >= 0.1)
	{
		framePerSecond = (double)elapsedSignal / sinceLast;
		lastSecond = totalTime;
		elapsedSignal = 0;
	}
}

const double UTime::DeltaTime()
{
	duration<double, std::milli> time_span = currentTime - prevTime;
	return time_span.count() / 1000.0;
}

const double UTime::TotalTime()
{
	return totalTime;
}

const double UTime::TotalTimeExact()
{
	currentTime = high_resolution_clock::now();
	duration<double, std::milli> time_span = currentTime - startTime;
	return  time_span.count() / 1000.0;
}

const double UTime::FramePerSecond()
{
	return framePerSecond;
}

void UTime::Restart()
{
	startTime = high_resolution_clock::now();
	prevTime = startTime;
	elapsedSignal = 0;
	totalTime = sinceLast = lastSecond = framePerSecond = 0.0;
}

// Slow down or speed up a thread to match a desired cycle rate(Hz)
// Ver 1.2: Now utilizes 10Hz FPS counter for more granular slow down.
void UTime::Throttle(double targetHz)
{
	if (targetHz > 1) // SOLVED!!!!!!
	{
		// if we are going too fast slow down
		unsigned int slow = 0;
		while (elapsedSignal / (TotalTimeExact() - lastSecond) > targetHz)
		{
			Sleep(slow++);
		}
	}
}