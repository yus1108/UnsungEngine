#pragma once
#include <mutex>
#include <condition_variable>
#include "UVector.h"

class ThreadInfo {
	std::mutex * mMutex;
	std::condition_variable * joinConds;
	std::condition_variable * mCond;
	bool LockFlag;
public:
	bool jobStart;
	void(*func)(UVector<void*>);
	UVector<void*> parameters;

	ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond, std::condition_variable * joinConds);
	~ThreadInfo();
	void Job();
	void Signal();
	void Join();
};

