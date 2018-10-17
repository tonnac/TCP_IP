#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

void ErrorHandling(const char * message);


int main(void)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	/* inet_addr */
	{
		char * addr = "127.212.124.78";
		DWORD conv_addr = inet_addr(addr);
		if (conv_addr == INADDR_NONE)
			printf("Error occured!\n");
		else
			printf("Network ordered integer addr: %#lx \n", conv_addr);
	}

	/* inet_ntoa */
	{
		struct sockaddr_in addr;
		char * strPtr;
		char strArr[20];

		addr.sin_addr.s_addr = htonl(0x1020304);
		strPtr = inet_ntoa(addr.sin_addr);
		strcpy_s(strArr, 20, strPtr);
		printf("Dotted-Decimal notation3 %s \n", strArr);
	}

	WSACleanup();

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}