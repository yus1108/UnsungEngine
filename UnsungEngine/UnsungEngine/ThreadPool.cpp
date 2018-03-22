#include "stdafx.h"
#include "ThreadPool.h"
#include <functional>
ThreadPool::ThreadPool()
{
	for (int i = 0; i < NUM_THREADS; i++)
		threadInfos.push_back(ThreadInfo(&mMutexs[i], &mConds[i]));
	for (int i = 0; i < NUM_THREADS; i++)
		threads.push_back(std::thread(&ThreadPool::AddThread, this, i));
}
ThreadPool::~ThreadPool()
{
	threadInfos.clear();
	for (std::thread & thread : threads)
		thread.join();
}

void ThreadPool::AddThread(int i) {
	threadInfos[i].Job();
}

int ThreadPool::AddTask(std::function<void()> func) {
	for (int i = 0; i < NUM_THREADS; i++)
	{
		if (!threadInfos[i].func && threadInfos[i].isJoined)
		{
			if (threadInfos[i].lockFlag)
			{
				std::unique_lock<std::mutex> uLock(mMutexs[i]);
				threadInfos[i].isJoined = false;
				threadInfos[i].func = func;
				threadInfos[i].Signal();
				return i;
			}
		}
	}
	return -1;
}

void ThreadPool::TestMethod(int i) {
	//Sleep(1);
	std::cout << "test method " << i << std::endl;
}

void ThreadPool::Join(int i) {
	threadInfos[i].Join();
}


