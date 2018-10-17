#pragma once
#include "Util.h"

DWORD WINAPI		ThreadFunc(LPVOID wParam);
using iter1 = std::map<SOCKET, User>::iterator;
extern std::map<SOCKET, User> g_UserList;

class Server
{
public:
	Server();
public:
	bool				ServerInit();
	bool				Accept();
	bool				ServerRelease();
public:
	static bool			RecvMsg(const User& user, UPACKET& packet);
	static bool			SendMsg(const User& user, UPACKET& packet);
	static void			DelUser(const User& user);
	static void			Disconnect(const User& user);
	static void			BroadCasting(UPACKET& packet);
private:
	void				AddUser(const User& user);
private:
	const char* IPAddr = "127.0.0.1";
	const u_short Port = 15000;
	SOCKET				m_ListenSock;
	SOCKADDR_IN			m_SockAddr;
};