#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <map>
#include "Protocol.h"

#pragma comment(lib, "ws2_32")


HANDLE g_hMutex = nullptr;

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
void ErrorMsg();
DWORD WINAPI ConnectThread(LPVOID Param);
DWORD WINAPI SendThread(LPVOID Param);
DWORD WINAPI RecvThread(LPVOID Param);

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
			return SOCKET_ERROR;
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
void ErrorMsg()
{
	char * pMsg = nullptr;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&pMsg, 0, NULL);
	std::cout << pMsg << std::endl;
	LocalFree(pMsg);
}
DWORD WINAPI ConnectThread(LPVOID Param)
{
	WaitForSingleObject(g_hMutex, INFINITE);
	SOCKET sock = (SOCKET)Param;
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(15000);
	InetPtonA(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

	int iRet;
	iRet = connect(sock, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	if (iRet == SOCKET_ERROR)
	{
		ErrorMsg();
		return -1;
	}
	ReleaseMutex(g_hMutex);
	return 1;
}
DWORD WINAPI SendThread(LPVOID Param)
{
	SOCKET sock = (SOCKET)Param;
	while (true)
	{
		if (SendMsg(sock) == SOCKET_ERROR)
		{
			break;
		}
		Sleep(1);
	}
	return 1;
}
DWORD WINAPI RecvThread(LPVOID Param)
{
	SOCKET sock = (SOCKET)Param;
	while (1)
	{
		UPACKET packet;
		if (RecvPacket(sock, packet) <= 0)
		{
			break;
		}
		std::cout << "수신 문자열: " << packet.msg;
	}
	return 1;
}