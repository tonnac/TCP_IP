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
	SOCKET hServSock;
	char message[BUF_SIZE];
	int strLen;

	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSize;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() Error!");

	ZeroMemory(&servAdr, sizeof(SOCKADDR_IN));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPtonA(AF_INET, ipAddr, &servAdr.sin_addr);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	while(1)
	{
		clntAdrSize = sizeof(clntAdr);

		strLen = recvfrom(hServSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSize);
		if (strLen == 0 || strLen == SOCKET_ERROR)
			break;
		sendto(hServSock, message, strLen, 0, (SOCKADDR*)&clntAdr, sizeof(clntAdr));
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