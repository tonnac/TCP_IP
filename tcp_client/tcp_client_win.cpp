#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>
#include <WS2tcpip.h>
void ErrorHandling(const char * message);

#pragma comment(lib, "ws2_32")

int main(void)
{
	const u_short port = 12345;
	const char ipAddr[] = "127.0.0.1";

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[30] = { 0, };
	int strLen = 0;
	int idx = 0, readLen = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	ZeroMemory(&servAddr, sizeof(SOCKADDR_IN));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	InetPtonA(AF_INET, ipAddr, &servAddr.sin_addr);

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");

	while (readLen = recv(hSocket, &message[idx++], 1, 0))
	{
		if (readLen == -1)
			ErrorHandling("read() error!");

		strLen += readLen;
	}

	printf("Message from server: %s \n", message);
	printf("Function read call count: %d \n", strLen);

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