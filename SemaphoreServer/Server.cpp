#include "Server.h"
#include <cstring>

std::map<SOCKET, User> g_UserList;
HANDLE g_Semaphore = nullptr;

Server::Server()
{
	ZeroMemory(&m_ListenSock, sizeof(SOCKET));
	ZeroMemory(&m_SockAddr, sizeof(SOCKADDR_IN));
}

bool Server::ServerInit()
{
	g_Semaphore = CreateSemaphore(nullptr, 1, 1, L"Semephore");
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
	std::cout << clientip << ", " << ntohs(clientaddr.sin_port) << " 접속" << std::endl;
	User user;
	user.sock = clientsock;
	user.addr = clientaddr;
	AddUser(user);
	DWORD ThreadID = 0;
	CreateThread(NULL, 0, ThreadFunc, &g_UserList[user.sock], 0, &ThreadID);
	return true;
}
bool Server::ServerRelease()
{
	CloseHandle(g_Semaphore);
	closesocket(m_ListenSock);
	WSACleanup();
	return true;
}
bool Server::RecvMsg(const User& user, UPACKET& packet)
{
	int RecvByte = 0;
	int TRecvByte = 0;
	char buffer[BUF_SIZE] = { 0, };
	do
	{
		RecvByte = recv(user.sock, &buffer[TRecvByte], PACKET_HEADER_SIZE - TRecvByte, 0);
		if (RecvByte == 0 || RecvByte == SOCKET_ERROR)
		{
			Server::Disconnect(user);
			return false;
		}
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
				RecvByte = recv(user.sock, &buffer[TRecvByte], packet.ph.len - TRecvByte, 0);
				if (RecvByte == 0 || RecvByte == SOCKET_ERROR)
				{
					Server::Disconnect(user);
					return false;
				}
				TRecvByte += RecvByte;
			} while (TRecvByte < packet.ph.len);
			memcpy(packet.msg, buffer, packet.ph.len);
		}break;
	}
	return true;
}
bool Server::SendMsg(const User& user, UPACKET& packet)
{
	int SendByte = 0;
	int TSendByte = 0;
	char buffer[BUF_SIZE] = { 0, };
	packet.ph.len = strlen(packet.msg);
	memcpy(buffer, &packet, PACKET_HEADER_SIZE + strlen(packet.msg));
	do
	{
		SendByte = send(user.sock, &buffer[TSendByte], strlen(packet.msg) + PACKET_HEADER_SIZE - TSendByte, 0);
		if (SendByte == 0 || SendByte == SOCKET_ERROR)
		{
			Server::Disconnect(user);
			return false;
		}
		TSendByte += SendByte;
	} while (TSendByte < strlen(packet.msg) + PACKET_HEADER_SIZE);
	return true;
}
void Server::AddUser(const User& user)
{
	WaitForSingleObject(g_Semaphore, INFINITE);
	g_UserList[user.sock] = user;
	ReleaseSemaphore(g_Semaphore, 1, NULL);
}
void Server::DelUser(const User& user)
{
	WaitForSingleObject(g_Semaphore, INFINITE);
	iter1 iter = g_UserList.find(user.sock);
	if (iter != g_UserList.end())
	{
		g_UserList.erase(iter);
	}
	ReleaseSemaphore(g_Semaphore, 1, NULL);
}
void Server::Disconnect(const User& user)
{
	char IPAdd[BUF_SIZE] = { 0, };
	InetNtopA(AF_INET, &user.addr.sin_addr, IPAdd, sizeof(IPAdd));
	std::cout << IPAdd << ", " << ntohs(user.addr.sin_port) << " 접속 종료" << std::endl;
}
void Server::BroadCasting(UPACKET& packet)
{
	WaitForSingleObject(g_Semaphore, INFINITE);
	iter1 iter = g_UserList.begin();
	while (iter != g_UserList.end())
	{
		if (Server::SendMsg(iter->second, packet) == false)
		{
			iter = g_UserList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	ReleaseSemaphore(g_Semaphore, 1, NULL);
}
DWORD WINAPI ThreadFunc(LPVOID wParam)
{
	User* user = (User*)wParam;
	while (1)
	{
		UPACKET packet;
		if (Server::RecvMsg(*user, packet) == false)
		{
			Server::DelUser(*user);
			break;
		}
		std::cout << "수신 문자열: " << packet.msg;
		char IPAdd[BUF_SIZE] = { 0, };
		InetNtopA(AF_INET, &user->addr.sin_addr, IPAdd, sizeof(IPAdd));
		char buffer1[BUF_SIZE] = { 0, };
		char port[256] = { 0, };
		_itoa_s(user->addr.sin_port, port, 10);
		CopyMemory(buffer1, IPAdd, strlen(IPAdd));
		CopyMemory(&buffer1[strlen(buffer1)], ", ", 2);
		CopyMemory(&buffer1[strlen(buffer1)], port, strlen(port));
		CopyMemory(&buffer1[strlen(buffer1)], "의 말: ", strlen("의 말: "));
		CopyMemory(&buffer1[strlen(buffer1)], packet.msg, strlen(packet.msg));
		CopyMemory(packet.msg, buffer1, strlen(buffer1));
		Server::BroadCasting(packet);
	}
	return 1;
}