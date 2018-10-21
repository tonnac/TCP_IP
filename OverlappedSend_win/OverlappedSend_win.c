#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

int main()
{
	const u_short port = 12345;
	const char IPAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;

	WSABUF dataBuf;
	char msg[] = "Network is Computer!";
	int sendBytes = 0;

	WSAEVENT evObj;
	WSAOVERLAPPED overlapped;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&sendAdr, sizeof(sendAdr));
	sendAdr.sin_family = AF_INET;
	sendAdr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &sendAdr.sin_addr);

	connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr));

	evObj = WSACreateEvent();
	ZeroMemory(&overlapped, sizeof(overlapped));
	overlapped.hEvent = evObj;
	dataBuf.buf = msg;
	dataBuf.len = strlen(msg) + 1;

	if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("Background data send");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
		}
		else
		{
			puts("Error");
		}
	}

	printf("Send data size : %d \n", sendBytes);
	WSACloseEvent(evObj);
	closesocket(hSocket);
	WSACleanup();

	return 0;
}