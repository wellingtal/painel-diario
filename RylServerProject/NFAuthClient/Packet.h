#pragma once

#include <windows.h>

enum PACKETLIST
{
	NONE = 0,
	CMD,
	AUTHOR,			// �� ���ø����̼��� ������ �õ��Ѵ�.
	IPLIST,			// ���Ӱ����� IP����Ʈ�� �����Ѵ�.
	BUYITEMINFO,	// ������ ���ų��� ����

	IRC,
	MaxPacket,
};

enum Result
{
	R_ERROR = 0,
	R_OK,
};

enum AUTHTYPE
{
	AT_NONE = 0,
	AT_LOGIN = 1,
	AT_UID,
	AT_DBAGENT,
	AT_AUTH,
	AT_CHAT,
	AT_ZONE,
};

enum CMDTYPE
{
	CMD_NONE = 0,
	CS_IPLIST = 1,
	SC_IPLISTEND,

	SC_SHUTDOWN = 101,	// ������Ѷ� ( S->C ) NFNetClient�� ���Ե� ���ø����̼��� �����Ѵ�.
	SC_CRASH = 201,
};

typedef struct NFITEMINFO
{
	unsigned long		CreateID;
	unsigned long		UID;
	unsigned long		CID;
	unsigned short		ItemPrototypeID;
	unsigned short		Amount;	
}*LPNFITEMINFO;

typedef struct PKIRC
{					
	CHAR	Key[64];
	CHAR	Message[512];
}*LPPKIRC;

typedef struct PKAUTHOR
{			
	DWORD	Type;		// Ÿ�� Ÿ���� ����, ���� ����� �̷������ ������ Ÿ��, ������ ����ȣ���̴�.
	CHAR	Key[40];	// �����ڵ�
}*LPPKAUTHOR;

typedef struct IPINFO
{
	DWORD	start;
	DWORD	end;
}*LPIPINFO;

typedef struct PKIPLIST
{
	enum { ALLOWIP, BLOCKIP };
	CHAR	Type;
	CHAR	Page;
	SHORT	Count;
}*LPPKIPLIST;

typedef struct PKBUYITEMINFO
{
	SHORT	Count;
	SHORT	Len;	
}*LPPKBUYITEMINFO;


// AUTHServer���� AuthClient�� ����� �����Ѵ�. 
// �̰���� ���� AuthClient�� �ൿ�Ѵ� (�˴ٿ���)
typedef struct PKCMD
{
	DWORD	dwCmd;
	DWORD	dwValue;
}*LPPKCMD;
