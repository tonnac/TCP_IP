#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define BUF_SIZE 1024

int main()
{
	const u_short port = 12345;
	const char IPAddr[] = "219.254.48.7";

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAdr;
	char message[BUF_SIZE];
	int strLen, readLen;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSocket = socket(AF_INET, SOCK_STREAM, 0);

	ZeroMemory(&servAdr, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &servAdr.sin_addr);

	connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr));

	while (1)
	{
		fputs("Input Message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		if (!_stricmp(message, "q\n"))
		{
			break;
		}
		strLen = strlen(message);
		send(hSocket, message, strLen, 0);
		readLen = 0;
		while (1)
		{
			readLen += recv(hSocket, &message[readLen], BUF_SIZE - 1, 0);
			if (readLen >= strLen)
				break;
		}
		message[strLen] = 0;
		printf("Message from server: %s", message);
	}

	closesocket(hSocket);
	WSACleanup();

	return 0;
}