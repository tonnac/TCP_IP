#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 1024

void ErrorHandling();


int main()
{
	const u_short port = 12345;
	const char ipAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	TIMEVAL timeout;
	fd_set reads, cpyReads;

	int adrSz;
	int strLen, fdNum, i;
	char buf[BUF_SIZE];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling();

	hServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling();

	ZeroMemory(&servAdr, sizeof(SOCKADDR_IN));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPtonA(AF_INET, ipAddr, &servAdr.sin_addr);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling();

	if (listen(hServSock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling();

	FD_ZERO(&reads);
	FD_SET(hServSock, &reads);

	while (1)
	{
		cpyReads = reads;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
			break;

		if (fdNum == 0)
			continue;

		for (i = 0; i < reads.fd_count; ++i)
		{
			if (FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				if (reads.fd_array[i] == hServSock)
				{
					adrSz = sizeof(clntAdr);
					hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
					FD_SET(hClntSock, &reads);
					char clntIP[BUF_SIZE];
					InetNtopA(AF_INET, &clntAdr.sin_addr, clntIP, sizeof(clntIP));
					printf("Connected client port : %d, IP : %s\n", ntohs(clntAdr.sin_port), clntIP);
				}
				else
				{
					strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
					if (strLen == 0)
					{
						FD_CLR(reads.fd_array[i], &reads);
						closesocket(cpyReads.fd_array[i]);
						printf("closed client: %d \n", cpyReads.fd_array[i]);
					}
					else
					{
						send(reads.fd_array[i], buf, strLen, 0);
					}
				}
			}
		}
	}

	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling()
{
	char* pMsg = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	printf("\n%s", pMsg);
	LocalFree(pMsg);
	exit(1);
}