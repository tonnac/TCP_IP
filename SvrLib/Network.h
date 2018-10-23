#pragma once
#include "SvrObject.h"

class Network : public SvrObject
{
public:
	Network();
	virtual ~Network();
public:
	bool Set(const u_short& port, const char* address);
	void Release();
public:
	SOCKET getSock() const;
private:
	SOCKET m_Sock;
};