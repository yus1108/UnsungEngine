#pragma once
#include <mutex>
#include <condition_variable>
#include "UVector.h"

class ThreadInfo {
	std::mutex * mMutex;
	std::condition_variable * mCond;
	bool LockFlag;
public:
	void(*func)(UVector<void*>);
	bool taskDone;
	bool isJoined;
	UVector<void*> parameters;

	ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond);
	~ThreadInfo();
	void Job();
	void Signal();
	void Join();
};

