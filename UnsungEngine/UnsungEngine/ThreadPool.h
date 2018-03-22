#pragma once
#include "Definition.h"
#include <mutex>
#include <condition_variable>
#include "UVector.h"
#include "ThreadInfo.h"

class ThreadPool
{
	std::mutex mMutexs[NUM_THREADS];
	std::condition_variable mConds[NUM_THREADS];
	std::vector<ThreadInfo> threadInfos;
	std::vector<std::thread> threads;

	void AddThread(int i);
public:
	ThreadPool();
	~ThreadPool();

	int AddTask(std::function<void()> func);
	static void TestMethod(int i);
	void Join(int i);
};

