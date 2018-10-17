#include "Util.h"

int main(void)
{
	const u_short port = 15000;
	const char* IPAddr = "127.0.0.1";

	if (WSAStart() != 0)
	{
		return -1;
	}

	SOCKET ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSock == INVALID_SOCKET)
	{
		return -1;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	InetPtonA(AF_INET, IPAddr, &addr.sin_addr.s_addr);

	int iRet = bind(ListenSock, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	iRet = listen(ListenSock, SOMAXCONN);
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	u_long on = TRUE;
	ioctlsocket(ListenSock, FIONBIO, &on);

	bool bConnect = true;
	SOCKET ClientSock;
	SOCKADDR_IN ClientAddr;
	memset(&ClientAddr, 0, sizeof(SOCKADDR_IN));
	int ClientAddrlen = sizeof(SOCKADDR_IN);


	while (g_UserList.size() != 2)
	{
		ClientSock = accept(ListenSock, (sockaddr*)&ClientAddr, &ClientAddrlen);
		if (ClientSock == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				break;
			}
		}
		else
		{
			User user;
			user.sock = ClientSock;
			user.addr = ClientAddr;
			g_UserList.push_back(user);
			int SendByte = send(user.sock, "¾È³ç\n", strlen("¾È³ç\n"), 0);
		}
	}

	User_Iter iter = g_UserList.begin();
	while (true)
	{
		char buffer[256] = { 0, };
		int RecvByte = recv(iter->sock, buffer, sizeof(buffer), 0);
		buffer[strlen(buffer)] = '\n';
		if (RecvByte == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				break;
			}
			++iter;
			if (iter == g_UserList.end())
			{
				iter = g_UserList.begin();
			}
			continue;
		}
		std::cout << "¼ö½Å ¹®ÀÚ¿­: " << buffer;
		for (int i = 0; i < g_UserList.size(); ++i)
		{
			send(g_UserList[i].sock, buffer, strlen(buffer), 0);
		}
	}

	closesocket(ListenSock);
	if (WSAClean() != 0)
	{
		return -1;
	}
	return 0;
}