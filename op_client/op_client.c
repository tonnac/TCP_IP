#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(const char * message);


int main()
{
	const u_short port = 12345;
	const wchar_t ipAddr[] = L"127.0.0.1";

	WSADATA wsaData;
	SOCKET hSocket;
	char opmsg[BUF_SIZE];
	int result, opndCnt, i;

	SOCKADDR_IN servAdr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(AF_INET, SOCK_STREAM, 0);
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

	fputs("Operand count: ", stdout);
	scanf_s("%d", &opndCnt);
	opmsg[0] = (char)opndCnt;

	for (i = 0; i < opndCnt; ++i)
	{
		printf("Operand %d: ", i + 1);
		scanf_s("%d", (int*)&opmsg[i*OPSZ + 1]);
	}
	fgetc(stdin);
	fputs("Operator: ", stdout);
	scanf_s("%c", &opmsg[opndCnt * OPSZ + 1]);
	send(hSocket, opmsg, opndCnt*OPSZ + 2, 0);
	recv(hSocket, &result, RLT_SIZE, 0);

	printf("Operation result: %d\n", result);

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