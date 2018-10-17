#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 1024

void ErrorHandling(const char * message);


int main()
{
	const u_short port = 12345;
	const char ipAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	char message[BUF_SIZE];
	int strLen, i;

	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSize;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() Error!");

	ZeroMemory(&servAdr, sizeof(SOCKADDR_IN));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPtonA(AF_INET, ipAddr, &servAdr.sin_addr);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if (listen(hServSock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	clntAdrSize = sizeof(clntAdr);

	for (i = 0; i < 5; ++i)
	{
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
		if (hClntSock == SOCKET_ERROR)
			ErrorHandling("accept() error");
		else
		{
			char clntIP[BUF_SIZE];
			InetNtopA(AF_INET, &clntAdr.sin_addr, clntIP, sizeof(clntIP));
			printf("Connected client port : %d, IP : %s, Idx : %d \n", ntohs(clntAdr.sin_port), clntIP, i + 1);
		}

		while ((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0)
			send(hClntSock, message, strLen, 0);

		closesocket(hClntSock);
	}

	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* message)
{
	char* pMsg = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	printf("\n%s", pMsg);
	LocalFree(pMsg);
	exit(1);
}