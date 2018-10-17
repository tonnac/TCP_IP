#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <map>
#include <vector>
#include "Protocol.h"

#pragma comment(lib, "ws2_32")

struct User
{
	User()
	{
		memset(&sock, 0, sizeof(SOCKET));
		memset(&addr, 0, sizeof(SOCKADDR_IN));
	}
	SOCKET sock;
	SOCKADDR_IN addr;
};

std::vector<User> g_UserList;
using User_Iter = std::vector<User>::iterator;

int WSAStart()
{
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int WSAClean()
{
	return WSACleanup();
}

int PacketHeaderRecv(const SOCKET& sock, UPACKET& packet);
int PacketMsgRecv(const SOCKET& sock, UPACKET& packet);
int RecvPacket(const SOCKET& sock, UPACKET& packet);
int SendMsg(const SOCKET& sock);
int SendServer(const SOCKET& sock, UPACKET& packet);

int PacketHeaderRecv(const SOCKET& sock, UPACKET& packet)
{
	char buffer[BUF_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int TRecvByte = 0;
	int recvByte = 0;
	do
	{
		recvByte = recv(sock, &buffer[recvByte], sizeof(char) * PACKET_HEADER_SIZE - recvByte, 0);
		if (recvByte == 0 || recvByte == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return -2;
			}
			return recvByte;
		}
		TRecvByte += recvByte;
	} while (TRecvByte < PACKET_HEADER_SIZE);
	memcpy(&packet.ph, buffer, sizeof(PACKET_HEADER_SIZE));
	return TRecvByte;
}
int PacketMsgRecv(const SOCKET& sock, UPACKET& packet)
{
	int recvByte = 0;
	int TRecvByte = 0;
	switch (packet.ph.type)
	{
	case PACKET_CHAT_MSG:
	{
		do
		{
			recvByte = recv(sock, &packet.msg[recvByte], sizeof(char) * packet.ph.len - recvByte, 0);
			if (recvByte == 0 || recvByte == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					return -2;
				}
				return recvByte;
			}
			TRecvByte += recvByte;
		} while (TRecvByte < packet.ph.len);
	}break;
	}
	return TRecvByte;
}
int RecvPacket(const SOCKET& sock, UPACKET& packet)
{
	int HeaderRecvByte = 0;
	int MsgRecvByte = 0;
	HeaderRecvByte = PacketHeaderRecv(sock, packet);
	if (HeaderRecvByte == 0 || HeaderRecvByte == SOCKET_ERROR)
	{
		return HeaderRecvByte;
	}
	MsgRecvByte = PacketMsgRecv(sock, packet);
	if (MsgRecvByte == 0 || MsgRecvByte == SOCKET_ERROR)
	{
		return MsgRecvByte;
	}
	return MsgRecvByte;
}
int SendMsg(const SOCKET& sock)
{
	char buffer[BUF_SIZE];
	int SendByte = 0;
	int TSendByte = 0;
	memset(buffer, 0, sizeof(char) * BUF_SIZE);
	UPACKET packet;
	std::cin >> packet.msg;
	packet.ph.len = strlen(packet.msg);
	packet.ph.type = PACKET_CHAT_MSG;
	memcpy(buffer, &packet, sizeof(packet));
	do
	{
		SendByte = send(sock, buffer, strlen(buffer), 0);
		if (SendByte == 0 || SendByte == SOCKET_ERROR)
		{
			return SendByte;
		}
		TSendByte += SendByte;
	} while (TSendByte < packet.ph.len + PACKET_HEADER_SIZE);
	return TSendByte;
}
int SendServer(const SOCKET& sock, UPACKET& packet)
{
	char buffer[BUF_SIZE];
	memset(buffer, 0, sizeof(char) * BUF_SIZE);
	memcpy(buffer, &packet, PACKET_HEADER_SIZE + strlen(packet.msg));
	return send(sock, buffer, PACKET_HEADER_SIZE + strlen(packet.msg), 0);
}
int BroadCasting(UPACKET& packet)
{
	User_Iter iter = g_UserList.begin();
	char buffer[BUF_SIZE];
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, &packet, PACKET_HEADER_SIZE + strlen(packet.msg));
	while (iter != g_UserList.end())
	{
		int SendByte = 0;
		int TSendByte = 0;
		do
		{
			SendByte = send(iter->sock, buffer, PACKET_HEADER_SIZE + strlen(packet.msg), 0);
			if (SendByte == 0 || SendByte == SOCKET_ERROR)
			{
				char IPBuffer[BUF_SIZE];
				memset(IPBuffer, 0, sizeof(char) * BUF_SIZE);
				g_UserList.erase(iter);
				std::cout << InetNtopA(AF_INET, &iter->addr.sin_addr, IPBuffer, sizeof(char)* BUF_SIZE) << ", " << ntohs(iter->addr.sin_port) << " 접속종료" << std::endl;
				break;
			}
			TSendByte += SendByte;
		} while (TSendByte < PACKET_HEADER_SIZE + strlen(packet.msg));
		++iter;
	}
	return 1;
}