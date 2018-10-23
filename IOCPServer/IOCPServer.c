#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

enum MODE { RECV, SEND };

#define BUF_SIZE 1024

typedef struct
{
	SOCKET sock;
	SOCKADDR_IN sockaddr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED overlapped;
	WSABUF wsabuf;
	char buffer[BUF_SIZE];
	enum MODE rwMode;
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI ThreadFunc(LPVOID arg);

int main()
{
	const u_short port = 12345;
	HANDLE iocp;
	WSADATA wsaData;
	SOCKET hRecvSock, hclntSock;
	SOCKADDR_IN hRecvAddr, hclntAddr;
	int addrlen = sizeof(SOCKADDR_IN);
	DWORD recvbytes, flaginfo = 0;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	for (int k = 0; k < (int)sysinfo.dwNumberOfProcessors; ++k)
	{
		_beginthreadex(NULL, 0, ThreadFunc, iocp, 0, NULL);
	}

	hRecvSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&hRecvAddr, sizeof(hRecvAddr));
	hRecvAddr.sin_port = htons(port);
	hRecvAddr.sin_family = AF_INET;
	hRecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int iRet = bind(hRecvSock, (SOCKADDR*)&hRecvAddr, sizeof(hRecvAddr));
	iRet = listen(hRecvSock, SOMAXCONN);


	while (1)
	{
		hclntSock = accept(hRecvSock, (SOCKADDR*)&hclntAddr, &addrlen);
		if (hclntSock == SOCKET_ERROR)
		{
			puts("accept error");
			break;
		}

		LPPER_HANDLE_DATA handleinfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		LPPER_IO_DATA ioinfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		handleinfo->sock = hclntSock;
		memcpy(&handleinfo->sockaddr, &hclntAddr, sizeof(SOCKADDR_IN));
		ioinfo->wsabuf.buf = ioinfo->buffer;
		ioinfo->wsabuf.len = BUF_SIZE;
		ioinfo->rwMode = RECV;
		memset(&ioinfo->overlapped, 0, sizeof(OVERLAPPED));
		char IPAddr[INET_ADDRSTRLEN];
		InetNtopA(AF_INET, &hclntAddr.sin_addr, IPAddr, INET_ADDRSTRLEN);
		printf("Client Connectd IP : %s, Port : %d\n", IPAddr, ntohs(hclntAddr.sin_port));
		CreateIoCompletionPort((HANDLE)hclntSock, iocp, (ULONG_PTR)handleinfo, 0);
		WSARecv(hclntSock, &ioinfo->wsabuf, 1, &recvbytes, &flaginfo, &ioinfo->overlapped, NULL);
	}


	closesocket(hRecvSock);
	WSACleanup();
	return 0;
}

DWORD WINAPI ThreadFunc(LPVOID arg)
{
	HANDLE iocp = (HANDLE)arg;
	LPPER_IO_DATA ioinfo = NULL;
	LPPER_HANDLE_DATA handleinfo = NULL;
	SOCKET sock;
	SOCKADDR_IN sockaddr;
	DWORD RecvBytes;
	int flags = 0;
	while (1)
	{
		BOOL Flag = GetQueuedCompletionStatus(iocp, &RecvBytes, (PULONG_PTR)&handleinfo, (LPOVERLAPPED*)&ioinfo, INFINITE);
		sock = handleinfo->sock;
		sockaddr = handleinfo->sockaddr;
		if (RecvBytes == 0)
		{
			char IPAddr[INET_ADDRSTRLEN];
			InetNtopA(AF_INET, &sockaddr.sin_addr, IPAddr, INET_ADDRSTRLEN);
			printf("Client Disconnected IP : %s, Port : %d\n", IPAddr, ntohs(sockaddr.sin_port));
			closesocket(sock);
			free(handleinfo); free(ioinfo);
			continue;
		}

		if (ioinfo->rwMode == RECV)
		{
			puts("Message Receive");
			ioinfo->rwMode = SEND;
			ioinfo->wsabuf.len = RecvBytes;
			memset(&ioinfo->overlapped, 0, sizeof(OVERLAPPED));
			WSASend(sock, &ioinfo->wsabuf, 1, NULL, 0, &ioinfo->overlapped, NULL);

			ioinfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			ioinfo->wsabuf.buf = ioinfo->buffer;
			ioinfo->wsabuf.len = BUF_SIZE;
			ioinfo->rwMode = RECV;
			memset(&ioinfo->overlapped, 0, sizeof(OVERLAPPED));
			WSARecv(sock, &ioinfo->wsabuf, 1, NULL, &flags, &ioinfo->overlapped, NULL);
		}
		else
		{
			puts("Message Send");
			free(ioinfo);
		}
	}
}