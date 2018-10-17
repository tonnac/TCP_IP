#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 1024

void ErrorHandling(const char * message);


int main()
{
	const u_short port = 12345;
	const wchar_t ipAddr[] = L"127.0.0.1";

	WSADATA wsaData;
	SOCKET hSocket;
	char message[BUF_SIZE];
	int strLen;

	SOCKADDR_IN servAdr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() Error!");

	ZeroMemory(&servAdr, sizeof(SOCKADDR_IN));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPton(AF_INET, ipAddr, &servAdr.sin_addr);

	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("Connected.................");

	while (1)
	{
		fputs("Input Message(Q to Quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);

		if (_stricmp(message, "q\n") == 0)
			break;

		send(hSocket, message, strlen(message), 0);
		strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
		message[strLen] = 0;
		printf("Message from server: %s", message);
	}
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}