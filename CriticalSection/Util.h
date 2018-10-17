#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "Protocol.h"

#pragma comment(lib, "ws2_32")

struct User
{
	User()
	{
		memset(&sock, 0, sizeof(SOCKET));
		memset(&addr, 0, sizeof(SOCKADDR_IN));
	}
	bool operator == (const User& user)
	{
		if (this->sock == user.sock)
		{
			return true;
		}
		return false;
	}
	SOCKET sock;
	SOCKADDR_IN addr;
};