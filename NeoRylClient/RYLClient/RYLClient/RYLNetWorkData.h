//================================================================
//
//	Name : RYLNetWorkData.h 
//	Desc : ��Ʈ�� ���� ����Ÿ
//  Date : 2003. 11. 16
//
//================================================================
#ifndef __RYL_NETWORKDATA_H__
#define __RYL_NETWORKDATA_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector.h>
#include <list>

#include <Network/Packet/PacketStruct/CharMovePacket.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>
#include <Network/ClientSocket/ClientSocket.h>
#include <Network/ClientNetwork/SessionMgr.h>

#include <Utility/Setup/GlobalConstants.h>
#include <Item/ItemStructure.h>

#ifndef NO_GAMEGUARD
#include <GameGuardLib/NPGameLib.h>
#endif

#ifdef ON_HACKSHIELD
#include <HShield/HShield.h>
#include <HShield/HSUpChk.h>
#endif

#include "../EventHandler/AuthHandler.h"
#include "../EventHandler/GameHandler.h"
#include "../EventHandler/UDPHandler.h"

#ifdef JAPAN_CODE
	#include <atlbase.h>
	extern CComModule _Module;
	#include <atlcom.h>

	#include <comdef.h>
	#import "IDAuth.tlb" no_namespace
#endif

// ���� ����
class CItemInstance;
class CPacketProfileMgr;

enum NetworkState
{
	NS_NOWAITING = 0,
	NS_CONNECTGAMESERVER,
	NS_SERVERMOVE,
	NS_CHARLOGIN,
	NS_CHARSELECT,
	NS_USERLOGIN,
	NS_UPDATEADDRESS,
	NS_CREATECHAR,
	NS_DELETECHAR,
	// WORK_LIST 2.1 ���� ���� �߰�
	NS_SELECTACCOUNTNATION,
	NS_TAKEITEM,
	NS_TRADEITEM,
	NS_RESPAWN,
	NS_UPGRADECLASS,
	NS_USEITEM,
	NS_SKILLERASE,
	NS_TAKEGOLD,
	NS_CASTOBJECT,
	NS_CONNECTAUTHSERVER,
	NS_SKILLUNLOCK,
	NS_ZONEMOVE,
	NS_RESPAWNSELECT,
	NS_STALLREGISTERITEM,
	NS_ITEMUPGRADE,
	NS_AUTHACCOUNT,
	NS_AUTOROUTE,
	NS_CONNECTCHAT,
	NS_CHATLOGIN,
	NS_GETOFFARMS,
	NS_REQUESTQUEST,
	NS_USECASHITEM,
	NS_UNIFIEDCHARSEL,
};

#define		NF_NOWAITING 0x00000000
#define		NF_BINDPOS   0x00000001
#define		NF_RESPAWN   0x00000002
#define		NF_SUICIDE	 0X00000005

struct StreetStallNode
{
	unsigned long m_dwChrID;
	char m_strStallName[32];
};

#ifndef NO_GAMEGUARD
struct GGAuthCode
{
	enum Flag
	{
		GG_AUTH_CODE_1	= (1 << 0),
		GG_AUTH_CODE_2	= (1 << 1), 
	};

	unsigned long		m_dwAuthCode;
	GG_AUTH_DATA		m_AuthCode2;
	unsigned char		m_cFlag;

	GGAuthCode()
	:	m_cFlag(0)
	{ }

	void SetFlag(unsigned char cFlag)	{ m_cFlag |= cFlag;	}
	bool CheckFlag(Flag flag)
	{
		if (m_cFlag & flag)
		{
			m_cFlag &= ~flag;
			return true;
		}

		return false;
	}
};
#endif

class CRYLNetworkData
{
private:

	unsigned long			m_dwGameContentsFlag;					// ����� �������� �����ϴ� �÷���
	unsigned long			m_dwNetworkFlag;

public :
#ifdef JAPAN_CODE
	IReqGameIDPtr	m_ReqGameID;
#endif

    typedef std::list<StreetStallNode*> StallNodeList;

	static long				m_lGMReportCounter;
	static long				m_lShoutCounter;
	static long				m_lFameInfoCounter;
	static long				m_lPartyFindCounter;
	static long				m_lPartyFindCounter2;					// 1�п� �ѹ��� ��û
	static long				m_lPartyCmdCounter;
		
	static char				m_strIP[MAX_PATH];						// �Ķ���ͷ� �޴� ���� IP
	unsigned long			m_dwUserID;								// ���� UID
	unsigned long			m_dwSessionID;							// ���� ���� ID
	unsigned long			m_dwChannelID;							// ä��
	static unsigned long	m_dwServerID;							// ������ ����
	unsigned long			m_dwChatServerID;						// ä�� ������ ID
	char					m_strChatIP[MAX_PATH];					// ä�� ������ IP

	char					m_strLoginID[ 24 ];
	char					m_strLoginPass[ 36 ];

	unsigned short			m_aryChannel[ 5 ];						// ä��
	unsigned short			m_aryChannelNum;						// ä�ΰ���

	unsigned long			m_dwMyChrID;							// �� ĳ���� UID
	unsigned long			m_dwPresentZone;						// ���� �� ��ȣ
	unsigned long			m_dwNextZone;							// ���� �� ��ȣ �� �̵��� ���δ�.

	NetworkState			m_nsNetworkStateforCastObject;
	unsigned long			m_dwNetworkError;

	GameRYL::ServiceNation	m_eInternationalCode;					// ���� �ڵ�
	GameRYL::ServerType		m_eServerType;							// ���� Ÿ��

	unsigned char			m_cBillingType;							// D : ����     T : ����    N : ����
	unsigned char			m_cBillingUser;							// 0 : ����     1 : �ǽù�

    unsigned char           m_cLoginStatusFlag;						// �α��� ���� statusFlag
	unsigned char			m_cAgentServerType;						// ���� Ÿ�� (����/�ű�)
    
	StallNodeList           m_lstStallName;

	CPacketProfileMgr*		m_lpPacketProfileMgr;					// ��Ŷ �����ս� üũ�� Ŭ���� (_PACKET_PROFILE_�� ���ǵǾ� �־�� �۵��Ѵ�.)

    CNetworkPos             m_LastNetPos;    
	unsigned long           m_dwLastSentTime;
    unsigned char           m_cLastUAct;
    unsigned char           m_cLastLAct;

	bool					m_bChinaBillingTime;

#ifndef NO_GAMEGUARD
	GGAuthCode				m_GGAuthCode;
	CNPGameLib*             m_lpNPGameLib;
#endif

public :

	CRYLNetworkData(GameRYL::ServiceNation eInternationalCode, GameRYL::ServerType eServerType, const char* szGameGuardString);
	~CRYLNetworkData();
	
	VOID	LinkCharacterData();

	static VOID		ProcessCounter( FLOAT fUpdate );

	static BOOL		SendMovingPacket( unsigned long dwUpperAni, unsigned long dwLowerAni, float fDir, vector3 &vecMove );
	static BOOL		SendCharRespawn( unsigned long dwChrID );
	static BOOL		SendMoveUpdatePacket( float fDir, vector3 &vecPosition );
	static BOOL		SendCharAttack( vector3 &vecPos, float fDir, unsigned short wType, bool bCasting, 
									unsigned char cSkillLock, unsigned char cSkillLevel, unsigned char cAtCount, 
									unsigned short wDefenserNum, unsigned long dwDefenser[ 10 ], 
									bool bMissileAttack = false, vector3 &vecDstPos = vector3( 0.0f, 0.0f, 0.0f ) );
	static BOOL		SendSummonAttack( unsigned long dwTargetID );

	static BOOL		SendChatMessage( const char *strName, const char *strMessage, unsigned long dwCommand, unsigned short shLang = 0);
	static BOOL		SendCreateChar( unsigned long dwSlot, CHAR_CREATE &pCreateChar );
	static BOOL		SendDeleteChar( unsigned long dwSlot, unsigned long dwUID, char *strPassword );
	// WORK_LIST 2.1 ���� ���� �߰�
	static BOOL		SendSelectAccountNation( unsigned char cType, unsigned char cAccountNation );
	static BOOL		SendFieldObjectPickUp( unsigned __int64 nFieldID, Item::ItemPos &pItemIndex );
	static BOOL		SendTakeItem( CItemInstance *lpTakeItem, Item::ItemPos *lpTakeIndex, unsigned char cNum );
	static BOOL		SendTakeMaterial( unsigned long dwCampID, unsigned char cSubCmd, unsigned char cNum, Item::ItemPos TakeIndex );
	static BOOL		SendTakeCastleJewel( unsigned long dwCastleID, unsigned char cSubCmd, unsigned short wItemID, unsigned char cIndex, unsigned char cNum, Item::ItemPos TakeIndex );
	static BOOL		SendSplitItem( CItemInstance *lpTakeItem, Item::ItemPos *lpTakeIndex, unsigned char cNum );
	static BOOL		SendQuickSlotMove( CItemInstance *lpTakeItem, unsigned char cPos, unsigned char cIndex, unsigned char cNum = 1 );
	static BOOL		SendFieldObjectPullDown( unsigned long dwNum, Item::ItemPos &pItemIndex );
	static BOOL		SendSwapItem( CItemInstance *lpSrcItem, Item::ItemPos *lpSrcIndex, CItemInstance *lpDstItem );
	static BOOL		SendTradeItem( unsigned char cCmd, unsigned long dwNPCID, unsigned short wTypeID, TakeType takeType, Item::ItemPos CouponPos, Item::CItem* lpItem );
	static BOOL		SendCharShape( unsigned long dwChrID = 0xFFFFFFFF );
	static BOOL		SendCharSwitchHand( void );
	static BOOL		SendCharSwitchRide( void );
    static BOOL		SendFameInfo( char* name,int check );
	static long		SendPartyFind( void );
	static long		SendPartyCmd( unsigned long dwReferenceID, unsigned short wCmd );
	static void		SendChant (CItemInstance *lpChantSkill );
	static BOOL		SendExchangeCmd( unsigned long dwSendID, unsigned short wCmd );
	static void		SetChant( CItemInstance *lpChantSkill );
	static void		SendClientLog( unsigned short wCmd, char *strString, char *strString2 = NULL );
	static BOOL		SendStallEnter( unsigned long dwStallChrID );
	static BOOL		SendStallLeave( void );
	static BOOL		SendSummonCmd( unsigned char cCmd, unsigned long dwTargetID = 0 );
	static BOOL		ChristmasParticle( DWORD dwAttChrID, WORD wSkillID, CHAR SkillLockCount );

	const char*		GetOldServerName( int iServerIdx );
	const char*		GetZoneName( char cZone );

	void	ChecknProtect( HWND hWnd );
	void	ClosenProtect();
	BOOL	StartnProtect(void);
	BOOL	Initnpfindsh();

	void	SendLoginID(char *strLoginID);
	BOOL	InitNPGameLib( HWND hWnd );

	// �ٽ��� ���� �Լ�
	static BOOL	UpdateHShield();
	BOOL	InitHShield(HWND hWnd);
	BOOL	UnInstHShield();
	BOOL	StartHShield(HWND hWnd);
	BOOL	StopHShield();

	bool UseContents(unsigned long dwContents)	{ return ((m_dwGameContentsFlag & dwContents) == dwContents) ? true : false;	}

	unsigned long	GetNetworkFlag()								{ return m_dwNetworkFlag;			}
	void			SetNetworkFlag( unsigned long dwNetworkFlag )	{ m_dwNetworkFlag = dwNetworkFlag; }

	// Signton
public :

	static CRYLNetworkData*	s_pInstance;
	static CRYLNetworkData*	Instance()			{ return s_pInstance;	} 
};

extern ClientNet::CSessionMgr*	g_pSessionMgr;				// ���� �Ŵ���.
extern ClientSocket*			g_pClientSocket;			// Ŭ���̾�Ʈ ���� Ŭ���� ������

#define g_AuthSession &g_pClientSocket->GetHandler(ClientSocket::AuthEventHandler)
#define g_GameSession &g_pClientSocket->GetHandler(ClientSocket::GameEventHandler)


#endif //__RYL_NETWORKDATA_H__