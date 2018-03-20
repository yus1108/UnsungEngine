#include "stdafx.h"
#include "ThreadInfo.h"


ThreadInfo::ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond)
{
	func = nullptr;
	this->mMutex = mMutex;
	this->mCond = mCond;
	LockFlag = true;
	taskDone = true;
	//func = (void(*)(UVector<void*>))testFunc;
}

ThreadInfo::~ThreadInfo()
{
	LockFlag = false;
	mCond->notify_all();
}

void ThreadInfo::Job()
{
	while (true)
	{
		std::unique_lock<std::mutex> uLock(*mMutex);
		isJoined = false;
		
		mCond->wait(uLock);
		if (!LockFlag)
			return;

		if (func)
			func(parameters);

		taskDone = true;
		uLock.unlock();
		uLock.lock();
		mCond->wait(uLock);
		if (!LockFlag)
			return;

		func = nullptr;
		isJoined = true;
	}
}

void ThreadInfo::Signal()
{
	mCond->notify_one();
}

void ThreadInfo::Join() {
	while (taskDone)
	{
		Signal();
	}
}
