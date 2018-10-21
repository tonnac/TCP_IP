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
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hRecvSock;
	SOCKADDR_IN Adr;
	struct ip_mreq joinAdr;
	char buf[BUF_SIZE];
	int strLen;

	hRecvSock = socket(AF_INET, SOCK_DGRAM, 0);
	ZeroMemory(&Adr, sizeof(Adr));
	Adr.sin_family = AF_INET;
	Adr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &Adr.sin_addr);

	bind(hRecvSock, (SOCKADDR*)&Adr, sizeof(Adr));

	InetPtonA(AF_INET, IPAddr, &joinAdr.imr_multiaddr);
	InetPtonA(AF_INET, IPAddr, &joinAdr.imr_interface);

	setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&joinAdr, sizeof(joinAdr));

	while (1)
	{
		strLen = recvfrom(hRecvSock, buf, BUF_SIZE - 1, 0, NULL, 0);
		if (strLen < 0)
			break;
		buf[strLen] = 0;
		fputs(buf, stdout);
	}

	closesocket(hRecvSock);
	WSACleanup();
	return 0;
}