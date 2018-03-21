#include "stdafx.h"
#include "ThreadPool.h"
#include <functional>
ThreadPool::ThreadPool()
{
	for (int i = 0; i < NUM_THREADS; i++)
		threadInfos.push_back(ThreadInfo(&mMutexs[i], &mConds[i]));
	for (int i = 0; i < NUM_THREADS; i++)
		threads.push_back(std::thread(&ThreadPool::AddThread, this, i));

	auto test = std::bind(TestMethod, std::placeholders::_1);
	UVector<void*> arg;
	test(arg);
	test(arg);
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
		if (!threadInfos[i].func && !threadInfos[i].isJoined)
		{
			std::unique_lock<std::mutex> uLock(mMutexs[i]);
			threadInfos[i].isJoined = false;
			threadInfos[i].func = func;
			threadInfos[i].parameters = args;
			threadInfos[i].Signal();
			return i;
		}
	}
	return -1;
}

void ThreadPool::TestMethod(UVector<void*> args) {
	//Sleep(1);
	//std::cout << "test method" << std::endl;
}

void ThreadPool::Join(int i) {
	threadInfos[i].Join();
	threadInfos[i].isJoined = true;
}


