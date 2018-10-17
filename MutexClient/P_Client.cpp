#include "Util.h"
#include <conio.h>


int main(void)
{
//	const u_short port = atoi(argv[1]);
//	const char* IPAddr = argv[2];

	g_hMutex = CreateMutex(nullptr, FALSE, L"Client");
	//if (GetLastError() == ERROR_ALREADY_EXISTS)
	//{
	//	CloseHandle(g_hMutex);
	//	return -1;
	//}

	if (WSAStart() != 0)
	{
		return -1;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	DWORD ThreadID1;
	HANDLE hConnectThread = CreateThread(nullptr, 0, ConnectThread, (LPVOID)sock, 0, &ThreadID1);

	WaitForSingleObject(hConnectThread, INFINITE);

	DWORD ThreadID2;
	HANDLE hSendThread = CreateThread(nullptr, 0, SendThread, (LPVOID)sock, 0, &ThreadID2);
	DWORD ThreadID3;
	HANDLE hRecvThread = CreateThread(nullptr, 0, RecvThread, (LPVOID)sock, 0, &ThreadID3);

	WaitForSingleObject(hSendThread, INFINITE);
	WaitForSingleObject(hRecvThread, INFINITE);

	CloseHandle(hConnectThread);
	CloseHandle(hSendThread);
	CloseHandle(hRecvThread);

	closesocket(sock);
	if (WSAClean() != 0)
	{
		return -1;
	}
	CloseHandle(g_hMutex);
	return 0;
}