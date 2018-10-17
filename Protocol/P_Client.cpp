#include "Util.h"

int main(int agrc, char* argv[])
{
	const u_short port = atoi(argv[1]);
	const char* IPAddr = argv[2];

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
	addr.sin_port = htons(port);
	InetPtonA(AF_INET, IPAddr, &addr.sin_addr.s_addr);

	int iRet = connect(sock, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	while (true)
	{
		UPACKET packet;
		int SendByte = SendMsg(sock);
		if (SendByte == 0 || SendByte == SOCKET_ERROR)
		{
			break;
		}
		int RecvByte = RecvPacket(sock, packet);
		if (RecvByte == 0 || RecvByte == SOCKET_ERROR)
		{
			break;
		}
		std::cout << "수신 문자열: " << packet.msg;
	}

	closesocket(sock);
	if (WSAClean() != 0)
	{
		return -1;
	}
	return 0;
}