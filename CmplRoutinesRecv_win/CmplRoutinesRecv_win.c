#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define BUF_SIZE 1024

void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

WSABUF dataBuf;
char buf[BUF_SIZE];
int recvBytes = 0;

int main()
{
	const u_short port = 12345;
	const char IPAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hLisnSock, hRecvSock;
	SOCKADDR_IN lisnAdr, recvAdr;

	WSAEVENT evObj;
	WSAOVERLAPPED overlapped;

	int idx, recvAdrSz, flags = 0;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hLisnSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&lisnAdr, sizeof(lisnAdr));
	lisnAdr.sin_family = AF_INET;
	lisnAdr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &lisnAdr.sin_addr);

	bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr));
	listen(hLisnSock, SOMAXCONN);

	recvAdrSz = sizeof(recvAdr);
	hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);

	evObj = WSACreateEvent();
	ZeroMemory(&overlapped, sizeof(overlapped));
	overlapped.hEvent = evObj;
	dataBuf.buf = buf;
	dataBuf.len = BUF_SIZE;

	if (WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, CompRoutine) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("Background data receive");
			//WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			//WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);
		}
	}

	idx = WSAWaitForMultipleEvents(1, &evObj, FALSE, WSA_INFINITE, TRUE);
	if (idx == WAIT_IO_COMPLETION)
		puts("Overlapped I/O Completed");
	else
		puts("WSARecv() error");

	WSACloseEvent(evObj);
	closesocket(hRecvSock);
	closesocket(hLisnSock);
	WSACleanup();
	return 0;
}

void CALLBACK CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	if (dwError != 0)
	{
		puts("CompRoutine error");
	}
	else
	{
		recvBytes = szRecvBytes;
		printf("Received message : %s\n", buf);
	}
}