#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 1024
#define OPSZ 4

void ErrorHandling(const char * message);
int calculate(int opnum, int opnds[], char op);

int main()
{
	const u_short port = 12345;
	const char ipAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	char opinfo[BUF_SIZE];
	int result, opndCnt, i;
	int recvCnt, recvLen;
	

	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSize;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() Error!");

	ZeroMemory(&servAdr, sizeof(SOCKADDR_IN));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPtonA(AF_INET, ipAddr, &servAdr.sin_addr);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if (listen(hServSock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	clntAdrSize = sizeof(clntAdr);

	for (i = 0; i < 5; ++i)
	{
		opndCnt = 0;
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
		if (hClntSock == SOCKET_ERROR)
			ErrorHandling("accept() error");
		else
		{
			char clntIP[BUF_SIZE];
			InetNtopA(AF_INET, &clntAdr.sin_addr, clntIP, sizeof(clntIP));
			printf("Connected client port : %d, IP : %s\n", ntohs(clntAdr.sin_port), clntIP);
		}
		recv(hClntSock, &opndCnt, 1, 0);

		recvLen = 0;
		while ((opndCnt*OPSZ + 1) > recvLen)
		{
			recvCnt = recv(hClntSock, &opinfo[recvLen], BUF_SIZE - 1, 0);
			recvLen += recvCnt;
		}
		result = calculate(opndCnt, (int*)opinfo, opinfo[recvLen - 1]);

		send(hClntSock, (char*)&result, sizeof(result), 0);
		closesocket(hClntSock);
	}

	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* message)
{
	char* pMsg = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	printf("\n%s", pMsg);
	LocalFree(pMsg);
	exit(1);
}
int calculate(int opnum, int opnds[], char op)
{
	int result = opnds[0], i;

	switch (op)
	{
	case '+':
		for (i = 1; i < opnum; ++i) result += opnds[i];
		break;
	case '-':
		for (i = 1; i < opnum; ++i) result -= opnds[i];
		break;
	case '*':
		for (i = 1; i < opnum; ++i) result *= opnds[i];
		break;
	}
	return result;
}