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
	SOCKET hSocket;
	FILE * fp;
	char buf[BUF_SIZE];
	int readCnt;

	SOCKADDR_IN servAdr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	fopen_s(&fp, "receive.dat", "wb");
	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	ZeroMemory(&servAdr, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(port);
	InetPton(AF_INET, ipAddr, &servAdr.sin_addr);

	connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr));

	while ((readCnt = recv(hSocket, buf, BUF_SIZE, 0)) != 0)
		fwrite((LPVOID)buf, 1, readCnt, fp);

	puts("Received file data");
	send(hSocket, "Thank you", 10, 0);
	fclose(fp);
	closesocket(hSocket);
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