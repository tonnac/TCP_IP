#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define BUF_SIZE 30
void ErrorHandling(char * msg);

int main()
{
	const u_short port = 12346;
	const wchar_t ipAddr[] = L"127.0.0.1";

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	FILE * fp;
	char buf[BUF_SIZE];
	int readCnt;

	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	fopen_s(&fp, "file_server_win.c", "rb");
	hServSock = socket(AF_INET, SOCK_STREAM, 0);

	ZeroMemory(&servAdr, sizeof(SOCKADDR_IN));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPton(AF_INET, ipAddr, &servAdr.sin_addr);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() Error");
	listen(hServSock, SOMAXCONN);

	clntAdrSz = sizeof(clntAdr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

	while (1)
	{
		readCnt = fread((LPVOID)buf, 1, BUF_SIZE, fp);
		if (readCnt < BUF_SIZE)
		{
			send(hClntSock, (char*)&buf, readCnt, 0);
			break;
		}
		send(hClntSock, (char*)&buf, BUF_SIZE, 0);
	}

	shutdown(hClntSock, SD_SEND);
	recv(hClntSock, (char*)buf, BUF_SIZE, 0);
	printf("Message from client: %s\n", buf);

	fclose(fp);
	closesocket(hClntSock), closesocket(hServSock);
	WSACleanup();

	return 0;
}

void ErrorHandling(char * msg)
{
	char* pMsg = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	printf("\n%s", pMsg);
	LocalFree(pMsg);
	exit(1);
}