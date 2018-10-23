#include "Thread.h"

Thread::Thread()
{
	CreateThread();
}
Thread::~Thread()
{
	CloseHandle((HANDLE)m_hThread);
}
void Thread::CreateThread()
{
	m_hThread = _beginthreadex(NULL, 0, HandleRunner, this, 0, &m_iThreadID);
}
void Thread::Run()
{
	return;
}
UINT WINAPI Thread::HandleRunner(LPVOID Param)
{
	Thread * lpThread = (Thread*)Param;
	if (lpThread) lpThread->Run();
	return 0;
}