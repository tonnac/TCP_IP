#pragma once
#include "SvrObject.h"

class IOCP : public SvrObject
{
public:
	IOCP();
	virtual ~IOCP();
public:
	bool Init();
	void Add(const HANDLE& sock, const ULONG_PTR& Key);
private:
	HANDLE m_hComport;
	static const int NUM_PROCESS;
};
