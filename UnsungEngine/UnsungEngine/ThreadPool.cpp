#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadInfos.push_back(ThreadInfo(&mMutexs[i], &mConds[i]));
		threads.push_back(std::thread(&ThreadPool::AddThread, this, i));
	}
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

int ThreadPool::AddTask(void(*func)(UVector<void*>), UVector<void*> args) {
	for (int i = 0; i < NUM_THREADS; i++)
	{
		if (!threadInfos[i].func)
		{
			threadInfos[i].taskDone = false;
			threadInfos[i].func = func;
			threadInfos[i].parameters = args;
			threadInfos[i].Signal();
			return i;
		}
	}
	return -1;
}

void ThreadPool::TestMethod(UVector<void*> args) {
	std::cout << "test method" << std::endl;
}

void ThreadPool::Join(int i) {
	threadInfos[i].Join();
}


