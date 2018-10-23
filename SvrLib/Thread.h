#pragma once
#include "SvrObject.h"

class Thread : public SvrObject
{
public:
	Thread();
	virtual ~Thread();
public:
	void CreateThread();
	virtual void Run();
	static UINT WINAPI HandleRunner(LPVOID Param);
private:
	UINT m_hThread;
	UINT m_iThreadID;
};