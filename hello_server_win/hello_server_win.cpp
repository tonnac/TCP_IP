#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>
#include <WS2tcpip.h>
void ErrorHandling(const char * message);

#pragma comment(lib, "ws2_32")

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClentSock;
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	const char message[] = "Hello World!";

	if (argc != 2)
	{
		printf("Usage : %s <port>\n,", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	ZeroMemory(&servAddr, sizeof(SOCKADDR_IN));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(atoi(argv[1]));
	const char ipAddr[] = "127.0.0.1";
	InetPtonA(AF_INET, ipAddr, &servAddr.sin_addr);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if (listen(hServSock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling("Listen() error");

	szClntAddr = sizeof(clntAddr);
	hClentSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
	if (hClentSock == INVALID_SOCKET)
		ErrorHandling("accept() error");

	send(hClentSock, message, sizeof(message), 0);
	closesocket(hClentSock);
	closesocket(hServSock);
	WSACleanup();

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}