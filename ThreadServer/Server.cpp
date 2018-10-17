#include "Server.h"

std::map<SOCKET, User> g_UserList;

Server::Server()
{
	ZeroMemory(&m_ListenSock, sizeof(SOCKET));
	ZeroMemory(&m_SockAddr, sizeof(SOCKADDR_IN));
}

bool Server::ServerInit()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}
	m_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSock == SOCKET_ERROR)
	{
		return false;
	}

	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(Port);
	InetPtonA(AF_INET, IPAddr, &m_SockAddr.sin_addr.s_addr);

	int iRet = bind(m_ListenSock, (sockaddr*)&m_SockAddr, sizeof(SOCKADDR_IN));
	if (iRet != 0)
	{
		return false;
	}

	iRet = listen(m_ListenSock, SOMAXCONN);
	if (iRet != 0)
	{
		return false;
	}
	return true;
}
bool Server::Accept()
{
	SOCKET clientsock;
	SOCKADDR_IN clientaddr;
	memset(&clientaddr, 0, sizeof(SOCKADDR_IN));
	int clientaddlen = sizeof(SOCKADDR_IN);

	clientsock = accept(m_ListenSock, (sockaddr*)&clientaddr, &clientaddlen);
	if (clientsock == INVALID_SOCKET)
	{
		return false;
	}
	char clientip[256] = { 0, };
	InetNtopA(AF_INET, &clientaddr.sin_addr, clientip, sizeof(clientip));
	std::cout << clientip << ", " << ntohs(clientaddr.sin_port) << "접속" << std::endl;
	User user;
	user.sock = clientsock;
	user.addr = clientaddr;
	g_UserList.insert(std::make_pair(user.sock, user));
	DWORD ThreadID = 0;
	CreateThread(NULL, 0, ThreadFunc, &g_UserList[user.sock], 0, &ThreadID);
	return true;
}
bool Server::RecvMsg(const SOCKET& sock, UPACKET& packet)
{
	int RecvByte = 0;
	int TRecvByte = 0;
	char buffer[BUF_SIZE] = { 0, };
	do
	{
		RecvByte = recv(sock, &buffer[TRecvByte], PACKET_HEADER_SIZE - TRecvByte, 0);
		TRecvByte += RecvByte;
	} while (TRecvByte < PACKET_HEADER_SIZE);
	memcpy(&packet.ph, buffer, sizeof(PACKET_HEADER_SIZE));
	RecvByte = 0;
	TRecvByte = 0;
	ZeroMemory(buffer, sizeof(char) * BUF_SIZE);
	switch (packet.ph.type)
	{
		case PACKET_CHAT_MSG:
		{
			do
			{
				RecvByte = recv(sock, &buffer[TRecvByte], packet.ph.len - TRecvByte, 0);
				TRecvByte += RecvByte;
			} while (TRecvByte < packet.ph.len);
			memcpy(packet.msg, buffer, packet.ph.len);
		}break;
	}
	return true;
}
bool Server::SendMsg(const SOCKET& sock, UPACKET& packet)
{
	int SendByte = 0;
	int TSendByte = 0;
	do
	{
		SendByte = send(sock, &packet.msg[TSendByte], strlen(packet.msg) + PACKET_HEADER_SIZE - TSendByte, 0);
		TSendByte += SendByte;
	} while (TSendByte < strlen(packet.msg) + PACKET_HEADER_SIZE);
	return true;
}
DWORD WINAPI ThreadFunc(LPVOID wParam)
{
	User* user = (User*)wParam;
	while (1)
	{
		UPACKET packet;
		Server::RecvMsg(user->sock, packet);
		std::cout << "수신 문자열: " << packet.msg;
		Server::SendMsg(user->sock, packet);
	}
}