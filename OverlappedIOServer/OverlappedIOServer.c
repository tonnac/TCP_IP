#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

typedef struct
{
	SOCKET hClntSock;
	char buf[BUF_SIZE];
	WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main()
{
	const u_short port = 12345;

	WSADATA wsaData;
	SOCKET hLisnSock, hRecvSock;
	SOCKADDR_IN lisnAdr, recvAdr;
	LPWSAOVERLAPPED lpOvLp;
	DWORD recvBytes;
	LPPER_IO_DATA hbInfo;
	int mode = 1, recvAdrSz, flagInfo = 0;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hLisnSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ioctlsocket(hLisnSock, FIONBIO, &mode);
	ZeroMemory(&lisnAdr, sizeof(lisnAdr));
	lisnAdr.sin_family = AF_INET;
	lisnAdr.sin_port = htons(port);
	lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr));
	listen(hLisnSock, SOMAXCONN);

	recvAdrSz = sizeof(SOCKADDR_IN);
	while (1)
	{
		SleepEx(100, TRUE);
		hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);
		if (hRecvSock == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			else
				puts("Accept Error");
		}
		lpOvLp = (LPOVERLAPPED)malloc(sizeof(OVERLAPPED));
		ZeroMemory(lpOvLp, sizeof(OVERLAPPED));
		hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		ZeroMemory(hbInfo, sizeof(PER_IO_DATA));
		hbInfo->hClntSock = hRecvSock;
		hbInfo->wsaBuf.len = BUF_SIZE;
		hbInfo->wsaBuf.buf = hbInfo->buf;
		lpOvLp->hEvent = (HANDLE)hbInfo;
		char IPAddr[INET_ADDRSTRLEN];
		InetNtopA(AF_INET, &recvAdr.sin_addr, IPAddr, INET_ADDRSTRLEN);
		printf("Client Connectd IP : %s, Port : %d\n", IPAddr, ntohs(recvAdr.sin_port));
		WSARecv(hRecvSock, &hbInfo->wsaBuf, 1, &recvBytes, &flagInfo, lpOvLp, ReadCompRoutine);
	}

	closesocket(hLisnSock);
	WSACleanup();
	return 0;
}

void CALLBACK ReadCompRoutine(DWORD dwErr, DWORD Trans, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	LPPER_IO_DATA hbinfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
	SOCKET sock = hbinfo->hClntSock;
	LPWSABUF bufinfo = &hbinfo->wsaBuf;
	DWORD sentBytes;

	if (Trans == 0)
	{
		closesocket(sock);
		free(lpOverlapped->hEvent); free(lpOverlapped);
		puts("Clinet Disconnect");
	}
	else
	{
		bufinfo->len = Trans;
		WSASend(sock, bufinfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);
	}
}
void CALLBACK WriteCompRoutine(DWORD dwErr, DWORD Trans, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	LPPER_IO_DATA hbinfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
	SOCKET sock = hbinfo->hClntSock;
	LPWSABUF bufinfo = &hbinfo->wsaBuf;
	DWORD recvBytes;
	int flaginfo = 0;
	WSARecv(sock, bufinfo, 1, &recvBytes, &flaginfo, lpOverlapped, ReadCompRoutine);
}