#pragma once
#include <mutex>
#include <condition_variable>
#include "UVector.h"

class ThreadInfo {
	std::mutex * mMutex;
	std::condition_variable * mCond;
	bool isExit;
public:
	bool lockFlag;
	bool isJoined;
	std::function<void()> func;

	ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond);
	~ThreadInfo();
	void Job();
	void Signal();
	void Join();
};

