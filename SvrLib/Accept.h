#pragma once
#include "Thread.h"
#include "Network.h"

class Accept : public Thread
{
public:
	Accept();
	virtual ~Accept();
public:
	bool Set(const u_short& port, const char* address = nullptr);
	void Run() override;
private:
	Network m_network;
};