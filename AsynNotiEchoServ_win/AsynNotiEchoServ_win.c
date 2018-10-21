#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define BUF_SIZE 100

void CompressSockets(SOCKET hSockArr[], int idx, int total);
void CompressEvents(WSAEVENT hEventArr[], int idx, int total);

int main()
{
	const u_short port = 12345;
	const char IPAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;

	SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT newEvent;
	WSANETWORKEVENTS netEvents;

	int numofClntSock = 0;
	int strLen, i;
	int posInfo, startIdx;
	int clntAdrLen;
	char msg[BUF_SIZE];

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	hServSock = socket(AF_INET, SOCK_STREAM, 0);

	ZeroMemory(&servAdr, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &servAdr.sin_addr);

	int iRet = bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	iRet = listen(hServSock, SOMAXCONN);

	newEvent = WSACreateEvent();
	WSAEventSelect(hServSock, newEvent, FD_ACCEPT);

	hSockArr[numofClntSock] = hServSock;
	hEventArr[numofClntSock] = newEvent;
	++numofClntSock;

	while (1)
	{
		posInfo = WSAWaitForMultipleEvents(numofClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
		startIdx = posInfo - WSA_WAIT_EVENT_0;

		for (i = startIdx; i < numofClntSock; ++i)
		{
			int sigEventIdx = WSAWaitForMultipleEvents(1, hEventArr, TRUE, 0, FALSE);
			if ((sigEventIdx == WSA_WAIT_FAILED)) continue;
			else
			{
				sigEventIdx = i;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);
				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						puts("Accept Error()");
						break;
					}
					clntAdrLen = sizeof(clntAdr);
					hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &clntAdrLen);
					newEvent = WSACreateEvent();
					WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);

					hEventArr[numofClntSock] = newEvent;
					hSockArr[numofClntSock] = hClntSock;
					++numofClntSock;
					puts("connected new client....");
				}
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						puts("Read Error()");
						break;
					}
					strLen = recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);
					send(hSockArr[sigEventIdx], msg, strLen, 0);
				}

				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						puts("Close Error");
						break;
					}
					WSACloseEvent(hEventArr[sigEventIdx]);
					closesocket(hSockArr[sigEventIdx]);

					--numofClntSock;
					CompressEvents(hEventArr, sigEventIdx, numofClntSock);
					CompressSockets(hSockArr, sigEventIdx, numofClntSock);
				}
			}
		}
	}
	WSACleanup();

	return 0;
}


void CompressSockets(SOCKET hSockArr[], int idx, int total)
{
	int i;
	for (i = idx; i < total; ++i)
		hSockArr[i] = hSockArr[i + 1];
	
}
void CompressEvents(WSAEVENT hEventArr[], int idx, int total)
{
	int i;
	for (i = idx; i < total; ++i)
		hEventArr[i] = hEventArr[i + 1];
}