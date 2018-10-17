#include "Util.h"

int main(void)
{
	const u_short port = 15000;
	const char* IPAddr = "219.254.48.7";

	if (WSAStart() != 0)
	{
		return -1;
	}

	SOCKET ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSock == INVALID_SOCKET)
	{
		return -1;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	InetPtonA(AF_INET, IPAddr, &addr.sin_addr.s_addr);

	int iRet = bind(ListenSock, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	iRet = listen(ListenSock, SOMAXCONN);
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	bool bConnect = true;
	SOCKET ClientSock;
	SOCKADDR_IN ClientAddr;
	memset(&ClientAddr, 0, sizeof(SOCKADDR_IN));
	int ClientAddrlen = sizeof(SOCKADDR_IN);

	while (bConnect == true)
	{
		ClientSock = accept(ListenSock, (sockaddr*)&ClientAddr, &ClientAddrlen);
		if (ClientSock == INVALID_SOCKET)
		{
			bConnect = false;
			break;
		}
		char Buffer[BUF_SIZE];
		memset(Buffer, 0, sizeof(BUF_SIZE));
		std::cout << InetNtopA(AF_INET, &ClientAddr.sin_addr, Buffer, sizeof(char)* BUF_SIZE) << ", " << ntohs(ClientAddr.sin_port) << " 접속" << std::endl;
		UPACKET packet;
		memcpy(packet.msg, "안녕\n", strlen("안녕\n"));
		packet.ph.len = strlen(packet.msg);
		packet.ph.type = PACKET_CHAT_MSG;
		while (bConnect == true)
		{
			int SendByte = SendServer(ClientSock, packet);
			if (SendByte == 0 || SendByte == SOCKET_ERROR)
			{
				std::cout << InetNtopA(AF_INET, &ClientAddr.sin_addr, Buffer, sizeof(char)* BUF_SIZE) << ", " << ntohs(ClientAddr.sin_port) << " 접속종료" << std::endl;
				closesocket(ClientSock);
				bConnect = false;
				break;
			}
			std::cout << "송신 문자열: " << packet.msg;
			memset(&packet, 0, sizeof(packet));
			int RecvByte = RecvPacket(ClientSock, packet);
			if (RecvByte == 0 || RecvByte == SOCKET_ERROR)
			{
				std::cout << InetNtopA(AF_INET, &ClientAddr.sin_addr, Buffer, sizeof(char)* BUF_SIZE) << ", " << ntohs(ClientAddr.sin_port) << " 접속종료" << std::endl;
				closesocket(ClientSock);
				bConnect = false;
				break;
			}
			std::cout << "수신 문자열: " << packet.msg;
		}
	}

	closesocket(ListenSock);
	if (WSAClean() != 0)
	{
		return -1;
	}
	return 0;
}