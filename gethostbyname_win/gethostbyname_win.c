#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")
#define BUF_SIZE 1024

void ErrorHandling();


int main()
{
	const u_short port = 12345;
	const wchar_t ipAddr[] = L"127.0.0.1";
	const char hostname[] = "google.com";

	WSADATA wsaData;
	int i;
	struct hostent *host;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling();
	host = gethostbyname(hostname);

	if (!host)
		ErrorHandling();

	printf("Official name: %s \n", host->h_name);
	for (i = 0; host->h_aliases[i]; ++i)
	{
		printf("Aliases %d: %s \n", i + 1, host->h_aliases[i]);
	}
	printf("Address type: %s \n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
	for (i = 0; host->h_addr_list[i]; ++i)
	{
		char addrlist[256];
		InetNtopA(AF_INET, (struct in_addr*)host->h_addr_list[i], addrlist, sizeof(addrlist));
		printf("IP addr %d: %s \n", i + 1, addrlist);
	}

	WSACleanup();
	return 0;
}

void ErrorHandling()
{
	char* pMsg = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	printf("\n%s", pMsg);
	LocalFree(pMsg);
	exit(1);
}