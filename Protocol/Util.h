#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <map>
#include "Protocol.h"

#pragma comment(lib, "ws2_32")

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
	std::cout << "보낼 문자열 입력(q는 종료): ";
	std::cin >> packet.msg;
	if (_stricmp(packet.msg, "q") == 0)
	{
		closesocket(sock);
		return SOCKET_ERROR;
	}
	packet.msg[strlen(packet.msg)] = '\n';
	packet.ph.len = strlen(packet.msg);
	packet.ph.type = PACKET_CHAT_MSG;
	memcpy(buffer, &packet, packet.ph.len + PACKET_HEADER_SIZE);
	do
	{
		SendByte = send(sock, buffer, packet.ph.len + PACKET_HEADER_SIZE, 0);
		if (SendByte == 0 || SendByte == SOCKET_ERROR)
		{
			return SendByte;
		}
		TSendByte += SendByte;
	} while (TSendByte < packet.ph.len + PACKET_HEADER_SIZE);
	std::cout << "송신 문자열: " << packet.msg;
	return TSendByte;
}
int SendServer(const SOCKET& sock, UPACKET& packet)
{
	char buffer[BUF_SIZE];
	memset(buffer, 0, sizeof(char) * BUF_SIZE);
	memcpy(buffer, &packet, PACKET_HEADER_SIZE + strlen(packet.msg));
	return send(sock, buffer, PACKET_HEADER_SIZE + strlen(packet.msg), 0);
}