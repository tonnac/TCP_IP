#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

void ErrorHandling()
{
	char* pMsg = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	printf("\n%s", pMsg);
	LocalFree(pMsg);
	exit(1);
}

void ShowSocketBufSize(SOCKET sock);

int main()
{
	WSADATA wsaData;
	SOCKET hSock;
	int sndBuf, rcvBuf, state;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSock = socket(PF_INET, SOCK_STREAM, 0);
	ShowSocketBufSize(hSock);

	sndBuf = 1024 * 3, rcvBuf = 1024 * 3;
	state = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, sizeof(sndBuf));
	state = setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, sizeof(rcvBuf));

	ShowSocketBufSize(hSock);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

void ShowSocketBufSize(SOCKET sock)
{
	int sndBuf, rcvBuf, state, len;

	len = sizeof(sndBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, &len);

	len = sizeof(rcvBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, &len);

	printf("Input buffer size: %d\n", rcvBuf);
	printf("Output buffer size: %d\n", sndBuf);
}