#include "IOCP.h"


IOCP::IOCP()
{

}
IOCP::~IOCP()
{
}

bool IOCP::Init()
{
	m_hComport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
}
void IOCP::Add(const HANDLE& sock, const ULONG_PTR& Key)
{

}