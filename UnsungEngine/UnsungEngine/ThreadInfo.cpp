#include "stdafx.h"
#include "ThreadInfo.h"


ThreadInfo::ThreadInfo(std::mutex * mMutex, std::condition_variable * mCond, std::condition_variable * joinConds)
{
	func = nullptr;
	this->mMutex = mMutex;
	this->joinConds = joinConds;
	this->mCond = mCond;
	LockFlag = true;
	//func = (void(*)(UVector<void*>))testFunc;
}

ThreadInfo::~ThreadInfo()
{
	LockFlag = false;
	mCond->notify_all();
	joinConds->notify_all();
}

void ThreadInfo::Job()
{
	while (true)
	{
		std::unique_lock<std::mutex> uLock(*mMutex);
		mCond->wait(uLock);
		if (!LockFlag)
		{
			jobStart = false;
			return;
		}

		if (func)
			func(parameters);

		jobStart = false;
		joinConds->wait(uLock);
		if (!LockFlag)
			return;

		func = nullptr;
	}
}

void ThreadInfo::Signal()
{
	mCond->notify_one();
}

void ThreadInfo::Join() {
	joinConds->notify_one();
}
