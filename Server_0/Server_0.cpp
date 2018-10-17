#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

int main()
{
	const u_short port = 15000;
	const char IPAddr[] = "219.254.48.7";

	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}

	SOCKET ListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSock == INVALID_SOCKET)
	{
		return -1;
	}

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &addr.sin_addr.s_addr);

	int iRet;
	iRet = bind(ListenSock, (sockaddr*)&addr, sizeof(addr));
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	iRet = listen(ListenSock, SOMAXCONN);
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	SOCKET ClientSock;
	SOCKADDR_IN ClientAddr;
	int addlen = sizeof(ClientAddr);
	bool bConnect = true;
	while (bConnect == true)
	{
		ClientSock = accept(ListenSock, (sockaddr*)&ClientAddr, &addlen);
		if (ClientSock == INVALID_SOCKET)
		{
			break;
		}
		char ClientIP[256] = { 0, };
		InetNtopA(AF_INET, &ClientAddr.sin_addr, ClientIP, sizeof(ClientIP));
		std::cout << ClientIP << ", " << ntohs(ClientAddr.sin_port) << "연결" << std::endl;

		int iSendByte = 0;
		while (bConnect == true)
		{
			char buffer[256] = { 0, };
			int iRecvByte = recv(ClientSock, buffer, sizeof(buffer), 0);
			if (iRecvByte == 0 || iRecvByte == SOCKET_ERROR)
			{
				bConnect = false;
				std::cout << ClientIP << ", " << ntohs(ClientAddr.sin_port) << "접속종료" << std::endl;
				break;
			}
			std::cout << "수신 문자열: " << buffer;
			do
			{
				int iSend = send(ClientSock, buffer, iRecvByte, 0);
				if (iSend == 0 || iSend == SOCKET_ERROR)
				{
					bConnect = false;
					std::cout << ClientIP << ", " << ntohs(ClientAddr.sin_port) << "접속종료" << std::endl;
					break;
				}
				iSendByte += iSend;
			} while (iSendByte < iRecvByte);
			std::cout << "전송 문자열: " << buffer;
		}
		closesocket(ClientSock);
	}


	closesocket(ListenSock);
	WSACleanup();
	return 0;
}