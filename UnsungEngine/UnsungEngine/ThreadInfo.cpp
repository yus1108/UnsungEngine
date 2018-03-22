#include "stdafx.h"
#include "ThreadInfo.h"


ThreadInfo::ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond)
{
	func = nullptr;
	this->mMutex = mMutex;
	this->mCond = mCond;
	isExit = true;
	isJoined = false;
}

ThreadInfo::~ThreadInfo()
{
	isExit = false;
	mCond->notify_all();
}

void ThreadInfo::Job()
{
	while (true)
	{
		std::unique_lock<std::mutex> uLock(*mMutex);
		lockFlag = true;
		isJoined = true;
		mCond->wait(uLock);
		lockFlag = false;
		if (!isExit)
			return;

		if (func)
			func();

		func = nullptr;
	}
}

void ThreadInfo::Signal()
{
	mCond->notify_one();
}

void ThreadInfo::Join() {
	while (!isJoined)
	{
		if (!lockFlag)
		{
			mCond->notify_one();
			std::unique_lock<std::mutex> uLock(*mMutex);
		}
	}
}
