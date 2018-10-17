#pragma once
#include <windows.h>

#define BUF_SIZE 256

#pragma pack(push,1)
struct PACKET_HEADER
{
	WORD len;
	WORD type;
};

struct UPACKET
{
	UPACKET()
	{
		memset(&ph, 0, sizeof(ph));
		memset(msg, 0, sizeof(msg));
	}
	PACKET_HEADER ph;
	char msg[BUF_SIZE];
};
using P_UPACKET = UPACKET*;

struct CHARACTER_INFO
{
	BYTE szName[13];
	INT iIndex;
	INT iData[20];
};
#pragma pack(pop)

#define PACKET_HEADER_SIZE sizeof(PACKET_HEADER)

#define PACKET_CHAT_MSG 1000
#define PACKET_CREATE_CHARACTER 2000