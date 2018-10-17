#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 30

void ErrorHandling();


int main()
{
	const u_short port = 12345;
	const char ipAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hAcptSock, hRecvSock;
	SOCKADDR_IN recvAdr, sendAdr;
	int sendAdrSize, strLen;
	char buf[BUF_SIZE];
	int result;

	fd_set read, except, readCopy, exceptCopy;
	struct timeval timeout;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling();

	hAcptSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hAcptSock == INVALID_SOCKET)
		ErrorHandling();

	ZeroMemory(&recvAdr, sizeof(SOCKADDR_IN));
	recvAdr.sin_family = AF_INET;
	recvAdr.sin_port = htons(port);
	InetPtonA(AF_INET, ipAddr, &recvAdr.sin_addr);

	if (bind(hAcptSock, (SOCKADDR*)&recvAdr, sizeof(recvAdr)) == SOCKET_ERROR)
		ErrorHandling();

	if (listen(hAcptSock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling();

	sendAdrSize = sizeof(sendAdr);
	
	hRecvSock = accept(hAcptSock, (SOCKADDR*)&sendAdr, &sendAdrSize);
	FD_ZERO(&read);
	FD_ZERO(&except);

	FD_SET(hRecvSock, &read);
	FD_SET(hRecvSock, &except);

	while (1)
	{
		readCopy = read;
		exceptCopy = except;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		result = select(0, &readCopy, NULL, &exceptCopy, &timeout);

		if (result > 0)
		{
			if (FD_ISSET(hRecvSock, &exceptCopy))
			{
				strLen = recv(hRecvSock, buf, BUF_SIZE - 1, MSG_OOB);
				buf[strLen] = 0;
				printf("Urgent message: %s \n", buf);
			}


			if (FD_ISSET(hRecvSock, &readCopy))
			{
				strLen = recv(hRecvSock, buf, BUF_SIZE - 1, 0);
				if (strLen == 0)
				{
					break;
					closesocket(hRecvSock);
				}
				else
				{
					buf[strLen] = 0;
					puts(buf);
				}
			}
		}

	}

	closesocket(hAcptSock);
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