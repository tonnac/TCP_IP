#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstring>

#pragma comment(lib,"ws2_32")

int main(int argc, char* argv[])
{
	const u_short port = atoi(argv[1]);
	const char* IPAddr = argv[2];

	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
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

	int iRet = connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (iRet == SOCKET_ERROR)
	{
		return -1;
	}

	while (1)
	{
		char buffer[256] = { 0, };
		std::cout << "전송 문자입력(Q는 종료): "; std::cin >> buffer;
		if (_stricmp(buffer, "q") == 0)
		{
			break;
		}
		buffer[strlen(buffer)] = '\n';
		int iSendByte = send(sock, buffer, strlen(buffer), 0);
		std::cout << "전송 문자열: " << buffer;
		ZeroMemory(buffer, sizeof(buffer));
		
		int iRecv = recv(sock, buffer, iSendByte, 0);
		int iRecvByte = 0;
		do
		{
			iRecvByte += iRecv;
		} while (iRecvByte < iSendByte);

		std::cout << "수신 문자열: " << buffer;
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}