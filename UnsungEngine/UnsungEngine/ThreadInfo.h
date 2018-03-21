#pragma once
#include <mutex>
#include <condition_variable>
#include "UVector.h"

class ThreadInfo {
	std::mutex * mMutex;
	std::condition_variable * mCond;
	bool LockFlag;
public:
	bool isJoined;
	void(*func)(UVector<void*>);
	UVector<void*> parameters;

	ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond);
	~ThreadInfo();
	void Job();
	void Signal();
	void Join();
};

