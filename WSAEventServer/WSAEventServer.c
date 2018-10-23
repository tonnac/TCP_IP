#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define BUF_SIZE 512

WSAEVENT hWSAEvent[WSA_MAXIMUM_WAIT_EVENTS];
SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];

void DelEvent(int sigEventidx, int NumofSock);

int main()
{
	const u_short port = 12345;

	WSADATA wsaData;
	SOCKET hLisnSock, hclntSock;
	SOCKADDR_IN hServAddr, hclntAddr;
	int addrlen = sizeof(SOCKADDR_IN);
	ZeroMemory(&hServAddr, sizeof(hServAddr));
	int iRet, i;
	int NumofSock = 0;
	int idx = 0, startidx = 0;
	int strLen = 0;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hLisnSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hLisnSock == INVALID_SOCKET)
	{
		return -1;
	}

	hServAddr.sin_family = AF_INET;
	hServAddr.sin_port = htons(port);
	hServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iRet = bind(hLisnSock, (SOCKADDR*)&hServAddr, sizeof(hServAddr));
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}
	iRet = listen(hLisnSock, SOMAXCONN);
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	WSAEVENT wEvent = WSACreateEvent();
	WSAEventSelect(hLisnSock, wEvent, FD_ACCEPT);

	hSockArr[NumofSock] = hLisnSock;
	hWSAEvent[NumofSock] = wEvent;
	++NumofSock;

	while (1)
	{
		idx = WSAWaitForMultipleEvents(NumofSock, hWSAEvent, FALSE, INFINITE, FALSE);
		startidx = idx - WSA_WAIT_EVENT_0;

		for (i = startidx; i < NumofSock; ++i)
		{
			int sigEventidx = WSAWaitForMultipleEvents(1, hWSAEvent, TRUE, 0, FALSE);
			if (sigEventidx == WSA_WAIT_FAILED) continue;
			else
			{
				sigEventidx = i;
				WSANETWORKEVENTS netevent;
				WSAEnumNetworkEvents(hSockArr[sigEventidx], hWSAEvent[sigEventidx], &netevent);
				if (netevent.lNetworkEvents & FD_ACCEPT)
				{
					if (netevent.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						puts("Accept Error");
						break;
					}
					ZeroMemory(&hclntAddr, sizeof(hclntAddr));
					hclntSock = accept(hSockArr[sigEventidx], (SOCKADDR*)&hclntAddr, &addrlen);
					wEvent = WSACreateEvent();
					hSockArr[NumofSock] = hclntSock;
					hWSAEvent[NumofSock] = wEvent;
					++NumofSock;
					WSAEventSelect(hclntSock, wEvent, FD_READ | FD_CLOSE);
					char IPAddr[INET_ADDRSTRLEN];
					InetNtopA(AF_INET, &hclntAddr.sin_addr, IPAddr, INET_ADDRSTRLEN);
					printf("Client Connectd IP : %s, Port : %d\n", IPAddr, ntohs(hclntAddr.sin_port));
				}

				if (netevent.lNetworkEvents & FD_READ)
				{
					if (netevent.iErrorCode[FD_READ_BIT] != 0)
					{
						puts("Recv Error");
						break;
					}
					char buffer[BUF_SIZE];
					strLen = recv(hSockArr[sigEventidx], buffer, BUF_SIZE, 0);
					send(hSockArr[sigEventidx], buffer, strLen, 0);
				}

				if (netevent.lNetworkEvents & FD_CLOSE)
				{
					if (netevent.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						puts("Close Error");
						break;
					}
					WSACloseEvent(hWSAEvent[sigEventidx]);
					closesocket(hSockArr[sigEventidx]);

					DelEvent(sigEventidx, NumofSock);
					--NumofSock;
				}
			}
		}
	}


	return 0;
}

void DelEvent(int sigEventidx, int NumofSock)
{
	for (int i = sigEventidx; i < NumofSock; ++i)
	{
		hSockArr[i] = hSockArr[i + 1];
		hWSAEvent[i] = hWSAEvent[i + 1];
	}
}