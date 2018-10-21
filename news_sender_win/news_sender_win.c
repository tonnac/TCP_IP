#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define TTL 64
#define BUF_SIZE 30


int main()
{
	const u_short port = 10000;
	const char IPAddr[] = "127.0.0.1";

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	SOCKET hSendSock;
	SOCKADDR_IN mulAdr;
	int timeLive = TTL;
	FILE * fp;
	char buf[BUF_SIZE];

	hSendSock = socket(AF_INET, SOCK_DGRAM, 0);
	ZeroMemory(&mulAdr, sizeof(mulAdr));
	mulAdr.sin_family = AF_INET;
	mulAdr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &mulAdr.sin_addr);

	setsockopt(hSendSock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&timeLive, sizeof(timeLive));
	fopen_s(&fp, "news.txt", "r");

	while (!feof(fp))
	{
		fgets(buf, BUF_SIZE, fp);
		sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR*)&mulAdr, sizeof(mulAdr));
		Sleep(2000);
	}

	closesocket(hSendSock);
	WSACleanup();
	return 0;
}