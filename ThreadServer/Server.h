#pragma once
#include "Util.h"

DWORD WINAPI		ThreadFunc(LPVOID wParam);
using iter1 = std::vector<User>::iterator;
extern std::map<SOCKET, User> g_UserList;

class Server
{
public:
	Server();
public:
	bool				ServerInit();
	bool				Accept();
public:
	static bool			RecvMsg(const SOCKET& sock, UPACKET& packet);
	static bool			SendMsg(const SOCKET& sock, UPACKET& packet);
private:
	const char* IPAddr = "127.0.0.1";
	const u_short Port = 15000;
	SOCKET				m_ListenSock;
	SOCKADDR_IN			m_SockAddr;
};