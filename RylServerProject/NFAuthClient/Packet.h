#pragma once

#include <windows.h>

enum PACKETLIST
{
	NONE = 0,
	CMD,
	AUTHOR,			// 각 어플리케이션의 인증을 시도한다.
	IPLIST,			// 접속가능한 IP리스트를 전달한다.
	BUYITEMINFO,	// 아이템 구매내역 정보

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

	SC_SHUTDOWN = 101,	// 종료시켜라 ( S->C ) NFNetClient가 포함된 어플리케이션을 종료한다.
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
	DWORD	Type;		// 타입 타입은 상위, 하위 워드로 이루어지며 하위는 타입, 상위는 존번호등이다.
	CHAR	Key[40];	// 인증코드
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


// AUTHServer에서 AuthClient로 결과를 전달한다. 
// 이결과에 따라 AuthClient는 행동한다 (셧다운등등)
typedef struct PKCMD
{
	DWORD	dwCmd;
	DWORD	dwValue;
}*LPPKCMD;
