#include "Accept.h"


Accept::Accept()
{}
Accept::~Accept()
{}

bool Accept::Set(const u_short& port, const char* address)
{
	return m_network.Set(port, address);
}
void Accept::Run()
{
	SOCKET clntSock;
	SOCKADDR_IN clntAdr;
	ZeroMemory(&clntAdr, sizeof(clntAdr));
	int clntAdrsz = sizeof(clntAdr);
	SOCKET hServSock = m_network.getSock();
	while (true)
	{
		clntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrsz);
		if (clntSock == INVALID_SOCKET)
		{
			continue;
		}

		//AddUser;

		Sleep(1);
	}
}