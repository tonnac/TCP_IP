#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

void ErrorHandling(const char * message);

int main(void)
{
	WSADATA wsaData;
	unsigned short host_port = 0x1234;
	unsigned short net_port;
	unsigned long host_addr = 0x12345678;
	unsigned long net_addr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	net_port = htons(host_port);
	net_addr = htonl(host_addr);

	printf("Host oredered port: %#x \n", host_port);
	printf("Network ordered port: %#x \n", net_port);
	printf("Host orerderd address: %#lx \n", host_addr);
	printf("Network ordered address: %#lx \n", net_addr);

	WSACleanup();
	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}