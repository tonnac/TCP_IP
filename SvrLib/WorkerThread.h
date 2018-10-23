#pragma once
#include "Thread.h"

class WorkerThread : public Thread
{
public:
	WorkerThread();
	virtual ~WorkerThread();
public:
	void Run() override;
};