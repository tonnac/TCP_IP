#include "Util.h"
#include <conio.h>

int main(void)
{
//	const u_short port = atoi(argv[1]);
//	const char* IPAddr = argv[2];

	if (WSAStart() != 0)
	{
		return -1;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		return -1;
	}

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(15000);
	InetPtonA(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

	u_long on = TRUE;
	ioctlsocket(sock, FIONBIO, &on);
	int iRet;
	do
	{
		iRet = connect(sock, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
		if (iRet == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				break;
			}
		}
	} while (iRet != 0 || iRet == SOCKET_ERROR);

	while (true)
	{
		if (_kbhit() == 1)
		{
			if (SendMsg(sock) == SOCKET_ERROR)
			{
				break;
			}
		}
		UPACKET packet;
		if (RecvPacket(sock, packet) > 0)
			std::cout << "수신 문자열: " << packet.msg;
	}

	closesocket(sock);
	if (WSAClean() != 0)
	{
		return -1;
	}
	return 0;
}