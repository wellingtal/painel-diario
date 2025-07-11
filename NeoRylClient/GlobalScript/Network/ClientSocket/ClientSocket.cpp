///////////////////////////////////////////////////////////////////////////////////////////////
//
// ClientSocket Class
//
///////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ClientSocket.h"

#include <mmsystem.h>
#include <Network/ClientNetwork/SessionMgr.h>
#include <Network/ClientNetwork/TCPSession.h>
#include <Network/ClientNetwork/UDPSession.h>
#include <Network/ClientNetwork/NullHandler.h>
#include <Network/ClientNetwork/ClientEventHandler.h>

#include <ctime>
#include "GMMemory.h"

// forward decl
bool GetHostIP(IN_ADDR &Addr, bool AllowVirtual);
bool IsWrongIP(IN_ADDR &Addr);
bool GetNATAddress(SOCKET Socket_In, SOCKADDR_IN *Address_Out, bool AllowVirtual);

ClientSocket* ClientSocket::s_pInstance = 0;

ClientSocket* ClientSocket::CreateInstance()
{ 
	if( s_pInstance )
		delete s_pInstance;

	s_pInstance = new ClientSocket;
	return s_pInstance; 
};

ClientSocket::ClientSocket()
:   m_dwStatusFlag(0)
{
    memset(m_Address, 0, sizeof(SOCKADDR_IN) * (MAX_ADDRESS + 1));
    memset(m_lpEventHandler, 0, sizeof(ClientNet::CClientEventHandler*) * MAX_HANDLER + 1);

    m_bStatusFlag.reset();

	m_lpNullEventHandler = &ClientNet::CNullClientEventHandler::GetInstance();
#include "GMUndefNew.h"
    m_lpEventHandlerMgr = new (std::nothrow) ClientNet::CClientEventHandlerMgr;
#include "GMRedefNew.h"

	InitNSCheckList();
}


ClientSocket::~ClientSocket(void)
{
    delete m_lpEventHandlerMgr;
}


bool ClientSocket::Open(const char* address, unsigned short usport,
                        ClientNet::CClientEventHandler* lpEventHandler)
{
    return (0 != lpEventHandler && 0 != m_lpEventHandlerMgr && 
        m_lpEventHandlerMgr->Connect(INET_Addr(address, usport), lpEventHandler));
}

void ClientSocket::SetHandler(ClientNet::CClientEventHandler* lpHandler, HandlerType eHandlerType)
{
    m_lpEventHandler[eHandlerType] = lpHandler;
}

void ClientSocket::ResetHandler(ClientNet::CClientEventHandler* lpHandler, HandlerType eHandlerType)
{
    if (lpHandler == m_lpEventHandler[eHandlerType])
    {
        m_lpEventHandler[eHandlerType] = 0;
    }
}

bool ClientSocket::IsConnected(HandlerType eHandlerType) 
{ 
    return (0 != m_lpEventHandler[eHandlerType]);
}

void ClientSocket::Disconnect(HandlerType eHandlerType)
{
    if (0 != m_lpEventHandlerMgr)
    {
        if (eHandlerType == MAX_HANDLER)
        {
            for(int idx = 0; idx < MAX_HANDLER; ++idx)
            {
                if(0 != m_lpEventHandler[idx])
                {
                    m_lpEventHandlerMgr->Close(m_lpEventHandler[idx]);
                }
            }
        }
        else if(0 != m_lpEventHandler[eHandlerType])
        {
            m_lpEventHandlerMgr->Close(m_lpEventHandler[eHandlerType]);
        }
    }
}

void ClientSocket::SetAddress(AddressType eAddrType, const char* szAddress, unsigned port)
{
    m_Address[eAddrType] = INET_Addr(szAddress, port).get_addr_in();
}

void ClientSocket::HandleEvents(DWORD dwProcessPerCall)
{
    if(0 != m_lpEventHandlerMgr)
    {
        m_lpEventHandlerMgr->HandleEvents(dwProcessPerCall);
    }
}

bool GetHostIP(IN_ADDR &Addr, bool AllowVirtual)
{
    char HostName[256];
    if(!gethostname(HostName, 255))
    {
        PHOSTENT pHost = gethostbyname(HostName);
        if(NULL == pHost)
        {
            return false;
        }

		int Count = 0;
        for(;pHost->h_addr_list[Count]; ++Count)
        {
            memcpy(&(Addr.S_un.S_addr), pHost->h_addr_list[Count], sizeof(IN_ADDR));

            if(!AllowVirtual)
            {
                // ���� ������ Ȯ��
                // 10.0.0.0		~ 10.255.255.255
                // 172.16.0.0	~ 172.31.255.255
                // 192.168.0.0	~ 192.168.255.255
                if((unsigned char)10 == Addr.S_un.S_un_b.s_b1)
                {
                    continue;
                }
                else if((unsigned char)172 == Addr.S_un.S_un_b.s_b1)
                {
                    if(Addr.S_un.S_un_b.s_b2 >= (unsigned char)16 && Addr.S_un.S_un_b.s_b2 <= (unsigned char)31)
                    {
                        continue;
                    }
                }
                else if((unsigned char)192 == Addr.S_un.S_un_b.s_b1)
                {
                    if((unsigned char)168 == Addr.S_un.S_un_b.s_b2)
                    {
                        continue;
                    }
                }
            }

            // �߸��� ������
            // 169.X.X.X �ڵ� ������ �Ҵ� ( SyGate )
            // 0.X.X.X
            if((unsigned char)169 == Addr.S_un.S_un_b.s_b1)
            {
                continue;
            }
            else if((unsigned char)0 == Addr.S_un.S_un_b.s_b1)
            {
                continue;
            }

            return true;
        }

        if(0 != Count)
            return true;
    }

    return false;
}


bool IsWrongIP(IN_ADDR &Addr)
{
    if((unsigned char)169 == Addr.S_un.S_un_b.s_b1)
    {
        if((unsigned char)254 == Addr.S_un.S_un_b.s_b2)
        {
            return true;
        }
    }

    if((unsigned char)0 == Addr.S_un.S_un_b.s_b1)
        return true;

    return false;
}



bool GetNATAddress(SOCKET Socket_In, SOCKADDR_IN *Address_Out, bool AllowVirtual)
{
	int AddressSize = sizeof(SOCKADDR_IN);
	if(getsockname(Socket_In, (struct sockaddr *)Address_Out, &AddressSize) != SOCKET_ERROR)
	{
		char HostName[256];
 		if(!gethostname(HostName, 255))
		{
			PHOSTENT pHost = gethostbyname(HostName);
			if(NULL == pHost)
			{
				return false;
			}

			IN_ADDR& Addr = Address_Out->sin_addr;
			int Count = 0;
			for(;pHost->h_addr_list[Count]; ++Count)
			{
				memcpy(&(Addr.S_un.S_addr), pHost->h_addr_list[Count], sizeof(IN_ADDR));

				if(!AllowVirtual)
				{
					// ���� ������ Ȯ��
					// 10.0.0.0		~ 10.255.255.255
					// 172.16.0.0	~ 172.31.255.255
					// 192.168.0.0	~ 192.168.255.255
					if((unsigned char)10 == Addr.S_un.S_un_b.s_b1)
					{
						continue;
					}
					else if((unsigned char)172 == Addr.S_un.S_un_b.s_b1)
					{
						if(Addr.S_un.S_un_b.s_b2 >= (unsigned char)16 && Addr.S_un.S_un_b.s_b2 <= (unsigned char)31)
						{
							continue;
						}
					}
					else if((unsigned char)192 == Addr.S_un.S_un_b.s_b1)
					{
						if((unsigned char)168 == Addr.S_un.S_un_b.s_b2)
						{
							continue;
						}
					}
				}

				// �߸��� ������
				// 169.X.X.X �ڵ� ������ �Ҵ� ( SyGate )
				// 0.X.X.X
				if((unsigned char)169 == Addr.S_un.S_un_b.s_b1)
				{
					continue;
				}
				else if((unsigned char)0 == Addr.S_un.S_un_b.s_b1)
				{
					continue;
				}

				return true;
			}

			if(0 != Count)
				return true;
		}

		return false;
	}

	return false;
}


//by Hades Kang

void
ClientSocket::SetStatusFlag(unsigned long dwStatusFlag)
{
	m_dwStatusFlag = dwStatusFlag;
}
	
unsigned long 
ClientSocket::GetStatusFlag()
{
	return m_dwStatusFlag;
}



	
bool 
//�ش� �÷��װ� üũ�Ǿ����� ����
ClientSocket::IsNSFlagChecked( unsigned char ucCmdFlag )
{
	if( s_pInstance )
		return s_pInstance->m_bStatusFlag[ ucCmdFlag ];
	return false;
}


bool 
//�ش� �÷��� ü���� üũ�Ǿ����� ����
ClientSocket::IsNSFlagChainChecked( unsigned char ucCmdFlag )
{
	if( s_pInstance )
	{
		std::vector<unsigned char>*	pCheckList;
		std::vector<unsigned char>::iterator iter;
		pCheckList = &(s_pInstance->m_SendCheckList[ucCmdFlag]);
		for( iter = pCheckList->begin() ; iter != pCheckList->end(); iter++ )
		{
			if( s_pInstance->m_bStatusFlag[ (*iter) ] )						//�ش� �÷��װ� üũ �Ǿ� �ִٸ�...
				return true;
		}
	}
	return false;
}


void 
//�ش� �÷��׸� Ȱ��ȭ ��Ŵ - Send �ÿ� ����
ClientSocket::SetNSFlagOn( unsigned char ucCmdFlag )
{
	if( ucCmdFlag == CmdCharMove ||							//üũ�� �ʿ� ���� �κе�.
		ucCmdFlag == CmdCharUpdateAddress ||
		ucCmdFlag == CmdSysPing ||
		ucCmdFlag == CmdCharAdminCmd )
		return ;


	if( s_pInstance )
		s_pInstance->m_bStatusFlag[ ucCmdFlag ] = true;
}

void 
//�ش� �÷��׸� ��Ȱ��ȭ ��Ŵ - Receive �ÿ� ����
ClientSocket::SetNSFlagOff( unsigned char ucCmdFlag )
{
	if( s_pInstance )
	{
		std::vector<unsigned char>*	pCheckList;
		std::vector<unsigned char>::iterator iter;
		pCheckList = &(s_pInstance->m_ReceiveCheckList[ucCmdFlag]);
		for( iter = pCheckList->begin() ; iter != pCheckList->end(); iter++ )
		{
			s_pInstance->m_bStatusFlag[ (*iter) ] = false;
		}	
	}
}


void	
// Network Status ���� Check List �ʱ�ȭ ( Send, Receive )
ClientSocket::InitNSCheckList()
{
	InitSendCheckList();
	InitReceiveCheckList();
}

void	
ClientSocket::InitSendCheckList()
{
/*
	m_SendCheckList[ CmdAuthAccount				].push_back( CmdAuthAccount				) ;			// ���� ����
															
	m_SendCheckList[ CmdServerGroup				].push_back( CmdServerGroup				) ;			// ������ ����
															
	m_SendCheckList[ CmdUserLogin				].push_back( CmdUserLogin				) ;			// ���� �α���
	m_SendCheckList[ CmdUserKill				].push_back( CmdUserKill				) ;			// ���� ����
															
	m_SendCheckList[ CmdCharSelect				].push_back( CmdCharSelect				) ;			// ĳ���� ����
	m_SendCheckList[ CmdCharCreate				].push_back( CmdCharCreate				) ;			// ĳ���� ����
	m_SendCheckList[ CmdCharDelete				].push_back( CmdCharDelete				) ;			// ĳ���� ����
															
	m_SendCheckList[ CmdCharLogin				].push_back( CmdCharLogin				) ;			// ĳ���� �α���
	m_SendCheckList[ CmdCharSuicide				].push_back( CmdCharSuicide				) ;			// ĳ���� �ڻ�
	m_SendCheckList[ CmdCharRespawn				].push_back( CmdCharRespawn				) ;			// ĳ���� ��Ȱ
	//m_SendCheckList[ CmdCharMove				].push_back( CmdCharMove				) ;			// ĳ���� �̵� (P2P)
	m_SendCheckList[ CmdCharMoveUpdate			].push_back( CmdCharMoveUpdate			) ;			// ĳ���� �̵� ������Ʈ
	m_SendCheckList[ CmdCharChat				].push_back( CmdCharChat				) ;			// ĳ���� ä��
	m_SendCheckList[ CmdCharAttack				].push_back( CmdCharAttack				) ;			// ĳ���� ����
	m_SendCheckList[ CmdCharCastObject			].push_back( CmdCharCastObject			) ;			// ĳ���� ������Ʈ ������
	m_SendCheckList[ CmdCharUseSkill			].push_back( CmdCharUseSkill			) ;			// ĳ���� ��ų ���
*/
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharTakeItem			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)

	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharSwapItem			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)

	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharRepairItem			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)

	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharUseItem				].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)


	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharTradeItem			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)

	m_SendCheckList[ CmdCharStartQuest			].push_back( CmdCharStartQuest			) ;			// ����Ʈ ����

	m_SendCheckList[ CmdCharEquipShopInfo		].push_back( CmdCharEquipShopInfo		) ;			// NPC ��� ���� ����

	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)
	
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)

	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�

	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharPullDown			) ;				
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharPickUp				) ;				
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
	
	
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharPickUp				) ;				
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharTradeItem			) ;			// ĳ���� ������ �ŷ�
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharTakeItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharSwapItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharRepairItem			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharUseItem				) ;			// ĳ���� ������ ���
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�

/*
	m_SendCheckList[ CmdCharSkillLock			].push_back( CmdCharSkillLock			) ;			// ĳ���� ��ų ��
	m_SendCheckList[ CmdCharSkillCreate			].push_back( CmdCharSkillCreate			) ;			// ĳ���� ��ų ����
	m_SendCheckList[ CmdCharSkillErase			].push_back( CmdCharSkillErase			) ;			// ĳ���� ��ų �����
	m_SendCheckList[ CmdCharClassUpgrade		].push_back( CmdCharClassUpgrade		) ;			// ĳ���� Ŭ���� ���׷��̵�
	m_SendCheckList[ CmdCharShapeInfo			].push_back( CmdCharShapeInfo			) ;			// ĳ���� ���� (P2P)
	m_SendCheckList[ CmdCharIncreasePoint		].push_back( CmdCharIncreasePoint		) ;			// ĳ���� �ɷ� ����Ʈ ����
	m_SendCheckList[ CmdCharBindPosition		].push_back( CmdCharBindPosition		) ;			// ĳ���� ���ε� ������
	m_SendCheckList[ CmdCharRequireInfo			].push_back( CmdCharRequireInfo			) ;			// ĳ���� �ش� ���� ��û
//	m_SendCheckList[ CmdCharUpdateAddress		].push_back( CmdCharUpdateAddress		) ;			// ĳ���� UDP �ּ� ������Ʈ
	m_SendCheckList[ CmdCharPartyCmd			].push_back( CmdCharPartyCmd			) ;			// ĳ���� ��Ƽ ���
	m_SendCheckList[ CmdCharPartyMemInfo		].push_back( CmdCharPartyMemInfo		) ;			// ĳ���� ��Ƽ �ɹ� ���� ���� (P2P)
	m_SendCheckList[ CmdCharExchangeCmd			].push_back( CmdCharExchangeCmd			) ;			// ĳ���� ������ ��ȯ ���
	m_SendCheckList[ CmdSysConnectAgent			].push_back( CmdSysConnectAgent			) ;			// �ý��� �߰� ���� (UDP �߰�)		# ��� ����
	m_SendCheckList[ CmdSysPacketTransmit		].push_back( CmdSysPacketTransmit		) ;			// �ý��� ��Ŷ ���� (UDP �߰�)		# ��� ����
	m_SendCheckList[ CmdCharLogout				].push_back( CmdCharLogout				) ;			// ĳ���� �α� �ƿ�
	m_SendCheckList[ CmdDBGetData				].push_back( CmdDBGetData				) ;			// DB ������ ���
	m_SendCheckList[ CmdDBUpdateData			].push_back( CmdDBUpdateData			) ;			// ������Ʈ DB ������
	m_SendCheckList[ CmdAgentParty				].push_back( CmdAgentParty				) ;			// DB ������Ʈ ��Ƽ
	m_SendCheckList[ CmdSysServerLogin			].push_back( CmdSysServerLogin			) ;			// �ý��� ���� �α���
															
	m_SendCheckList[ CmdServerZone				].push_back( CmdServerZone				) ;			// ������ ����
															
	m_SendCheckList[ CmdGameCellInfo			].push_back( CmdGameCellInfo			) ;			// ���� �� ���� (������Ʈ ����)	
	m_SendCheckList[ CmdCharInfo				].push_back( CmdCharInfo				) ;			// ĳ���� ����						# ��� ����
	m_SendCheckList[ CmdCharAddressInfo			].push_back( CmdCharAddressInfo			) ;			// ĳ���� UDP �ּ� ����
	m_SendCheckList[ CmdCharCellLogin			].push_back( CmdCharCellLogin			) ;			// ĳ���� �� �α���
	m_SendCheckList[ CmdCharCellLogout			].push_back( CmdCharCellLogout			) ;			// ĳ���� �� �α׾ƿ�
	m_SendCheckList[ CmdMonMove					].push_back( CmdMonMove					) ;			// ���� �̵�
	m_SendCheckList[ CmdCharAttackInfo			].push_back( CmdCharAttackInfo			) ;			// ĳ���� ���� ���� (P2P)
	m_SendCheckList[ CmdCharAttacked			].push_back( CmdCharAttacked			) ;			// ĳ���� ���� ����
	m_SendCheckList[ CmdCharAward				].push_back( CmdCharAward				) ;			// ĳ���� ����� 
	m_SendCheckList[ CmdCharItemInfo			].push_back( CmdCharItemInfo			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharPickUp				].push_back( CmdCharPickUp				) ;			// ĳ���� ����
	m_SendCheckList[ CmdCharPullDown			].push_back( CmdCharPullDown			) ;			// ĳ���� ������
	m_SendCheckList[ CmdCharPickUpInfo			].push_back( CmdCharPickUpInfo			) ;			// ĳ���� ���� ����
	m_SendCheckList[ CmdCharPullDownInfo		].push_back( CmdCharPullDownInfo		) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharCastObjectInfo		].push_back( CmdCharCastObjectInfo		) ;			// ĳ���� ������ ������Ʈ ����
	m_SendCheckList[ CmdCharInstallSocket		].push_back( CmdCharInstallSocket		) ;			// ĳ���� ������ ���� ��ġ
	m_SendCheckList[ CmdCharLevelUp				].push_back( CmdCharLevelUp				) ;			// ĳ���� ���� ��
	m_SendCheckList[ CmdCharPartyInfo			].push_back( CmdCharPartyInfo			) ;			// ĳ���� ��Ƽ ����
	m_SendCheckList[ CmdCharUpgradeItem			].push_back( CmdCharUpgradeItem			) ;			// ĳ���� ������ ���׷��̵�
	m_SendCheckList[ CmdCharHPRegen				].push_back( CmdCharHPRegen				) ;			// ĳ���� HP ����
	m_SendCheckList[ CmdCharLevelUpInfo			].push_back( CmdCharLevelUpInfo			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharSplitItem			].push_back( CmdCharSplitItem			) ;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)
	m_SendCheckList[ CmdUpdateUIDTable			].push_back( CmdUpdateUIDTable			) ;			// UID ���̺� ������Ʈ
	m_SendCheckList[ CmdCharQuickSlotMove		].push_back( CmdCharQuickSlotMove		) ;			// ĳ���� ������ �̵�
	m_SendCheckList[ CmdCharSwitchEQ			].push_back( CmdCharSwitchHand			) ;			// �� �ٲٱ�
	m_SendCheckList[ CmdSysMngerRegistry		].push_back( CmdSysMngerRegistry		) ;			// ���� ������ ���
	m_SendCheckList[ CmdSysMngerRequest			].push_back( CmdSysMngerRequest			) ;			// ���� �������� ��û
	m_SendCheckList[ CmdSysMngerResponse		].push_back( CmdSysMngerResponse		) ;			// ���� �����ڷ� ����
	m_SendCheckList[ CmdCharTakeItems			].push_back( CmdCharTakeItems			) ;			// ĳ���� ������ ���� (����)
	m_SendCheckList[ CmdCharTakeGold			].push_back( CmdCharTakeGold			) ;			// ĳ���� �� ����
	m_SendCheckList[ CmdCharExchangeItem		].push_back( CmdCharExchangeItem		) ;			// ĳ���� ������ ��ȯ
	m_SendCheckList[ CmdCellBroadCasting		].push_back( CmdCellBroadCasting		) ;			// �� ��ε� ĳ����
	m_SendCheckList[ CmdSysPatchAddress			].push_back( CmdSysPatchAddress			) ;			// ��ġ �ּ� 
	m_SendCheckList[ CmdCharPartyCmdInfo		].push_back( CmdCharPartyCmdInfo		) ;			// ��Ƽ ��� ����
	m_SendCheckList[ CmdServerLog				].push_back( CmdServerLog				) ;			// �α� ���� (???)
	m_SendCheckList[ CmdCharWhisper				].push_back( CmdCharWhisper				) ;			// ĳ���� �ӼӸ�
	m_SendCheckList[ CmdSysServerVerUpdate		].push_back( CmdSysServerVerUpdate		) ;			// ���� ���� ������Ʈ
	m_SendCheckList[ CmdSysMng					].push_back( CmdSysMng					) ;			// ���� ���� ����, Ŭ���̾�Ʈ�� ����ϴ� ��Ŷ (�ӽ�) (???)
	m_SendCheckList[ CmdSysChannelUpdate		].push_back( CmdSysChannelUpdate		) ;			// ���� ä�� ������Ʈ
	m_SendCheckList[ CmdCharPartyFind			].push_back( CmdCharPartyFind			) ;			// ��Ƽ ã��
	m_SendCheckList[ CmdCharPartyMemData		].push_back( CmdCharPartyMemData		) ;			// ��Ƽ ��� ������
	m_SendCheckList[ CmdCharControlOption		].push_back( CmdCharControlOption		) ;			// ĳ���� �ɼ� ����
	m_SendCheckList[ CmdCharDuelCmd				].push_back( CmdCharDuelCmd				) ;			// ĳ���� ��� ���
	m_SendCheckList[ CmdCharFameInfo			].push_back( CmdCharFameInfo			) ;			// ĳ���� �� ����
															
	m_SendCheckList[ CmdLoginServerList			].push_back( CmdLoginServerList			) ;			// ���� ����Ʈ #!! ��ȣ ���� �Ұ� !!#
															
	m_SendCheckList[ CmdCharSpellInfo			].push_back( CmdCharSpellInfo			) ;			// ĳ���� æƮ&��æƮ ����
	m_SendCheckList[ CmdCharSkillUnLock			].push_back( CmdCharSkillUnLock			) ;			// ĳ���� ��ų �� ����
//	m_SendCheckList[ CmdSysPing					].push_back( CmdSysPing					) ;			// ���� �� ��Ŷ
	m_SendCheckList[ CmdCharMoveZone			].push_back( CmdCharMoveZone			) ;			// �� �̵�
															
	m_SendCheckList[ CmdAgentZone				].push_back( CmdAgentZone				) ;			// �� ����
															
	m_SendCheckList[ CmdDeposit					].push_back( CmdDeposit					) ; 		// â�� ó�� ��Ŷ (Client <--> GameServer)
	m_SendCheckList[ CmdDepositUpdate			].push_back( CmdDepositUpdate			) ; 		// â�� ������Ʈ ��Ŷ (GameServer <--> DBAgent)
															
	m_SendCheckList[ CmdCharStallOpen			].push_back( CmdCharStallOpen			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharStallRegisterItem	].push_back( CmdCharStallRegisterItem	) ;			// ĳ���� ������ ������ ���
	m_SendCheckList[ CmdCharStallEnter			].push_back( CmdCharStallEnter			) ;			// ĳ���� ������ ����
	m_SendCheckList[ CmdCharStallItemInfo		].push_back( CmdCharStallItemInfo		) ;			// ĳ���� ������ ������ ����
															
	//m_SendCheckList[ CmdCharAdminCmd			].push_back( CmdCharAdminCmd			) ;			// ĳ���� ����
															
	m_SendCheckList[ CmdCharTeamBattleInfo		].push_back( CmdCharTeamBattleInfo		) ;			// ����Ʋ ����
															
	m_SendCheckList[ CmdFriendAddRequest		].push_back( CmdFriendAddRequest		) ; 		// ģ�� �߰�
	m_SendCheckList[ CmdFriendRemoveRequest		].push_back( CmdFriendRemoveRequest		) ; 		// ģ�� ����
	m_SendCheckList[ CmdFriendEtcRequest		].push_back( CmdFriendEtcRequest		) ; 		// ģ�� ��Ÿ
	m_SendCheckList[ CmdFriendAck				].push_back( CmdFriendAck				) ; 		// ģ�� Ack
	m_SendCheckList[ CmdFriendDB				].push_back( CmdFriendDB				) ; 		// ģ�� ������ (�� ģ�� ����Ʈ ����)
															
	m_SendCheckList[ CmdEliteBonus				].push_back( CmdEliteBonus				) ; 		// ����Ʈ ���ʽ�

	m_SendCheckList[ CmdCharOperateTrigger		].push_back( CmdCharOperateTrigger		) ;			// Ʈ���� �ߵ�
	m_SendCheckList[ CmdQuestDB					].push_back( CmdQuestDB					) ;			// ����Ʈ ���� (DB�� ���̺�/�ε�, ĳ���� �α��)
	m_SendCheckList[ CmdCharEndQuest			].push_back( CmdCharEndQuest			) ;			// ����Ʈ ����
															
	m_SendCheckList[ CmdCharDisappearItem		].push_back( CmdCharDisappearItem		) ;			// ĳ���� ������ �������
															
	m_SendCheckList[ CmdCharAuthorizePanel		].push_back( CmdCharAuthorizePanel		) ;			// ĳ���� ���� �ǳ�
	m_SendCheckList[ CmdCharPeaceMode			].push_back( CmdCharPeaceMode			) ;			// ĳ���� ���� ���
	m_SendCheckList[ CmdConfigInfoDB			].push_back( CmdConfigInfoDB			) ;			// ���� ����
	m_SendCheckList[ CmdCharAutoRouting			].push_back( CmdCharAutoRouting			) ;			// ĳ���� ���� ����
	m_SendCheckList[ CmdRankingInfo				].push_back( CmdRankingInfo				) ;			// ��ŷ ����
	m_SendCheckList[ CmdCharStateRedistribution	].push_back( CmdCharStateRedistribution	) ;			// ���� ��й� (Ŭ���� ó�� ���·�)
	m_SendCheckList[ CmdBillingTimeoutNotify	].push_back( CmdBillingTimeoutNotify	) ; 		// ���� Ÿ�Ӿƿ� Notify����
															
	m_SendCheckList[ CmdAdminToolGetData		].push_back( CmdAdminToolGetData		) ; 		// ������� DBAgent�� ������ ��� ����.
	m_SendCheckList[ CmdAdminToolSetData		].push_back( CmdAdminToolSetData		) ;			// ������� DBAgent�� ������ ����.
															
	m_SendCheckList[ CmdEventDropItem			].push_back( CmdEventDropItem			) ;			// ������ ������ �̺�Ʈ
	m_SendCheckList[ CmdCharCancelQuest			].push_back( CmdCharCancelQuest			) ;			// ����Ʈ ���
	m_SendCheckList[ CmdBillingTimeCheckNotify	].push_back( CmdBillingTimeCheckNotify	) ;			// ���� Ÿ�� üũ
	m_SendCheckList[ CmdCharLotteryResult		].push_back( CmdCharLotteryResult		) ;			// ���� ���
	m_SendCheckList[ CmdCharSummonCmd			].push_back( CmdCharSummonCmd			) ;			// ��ȯ�� ���
															
	m_SendCheckList[ CmdChatClientLogin			].push_back( CmdChatClientLogin			) ; 		// Ŭ���̾�Ʈ���� ä�ü����� ���� �� ���� ��
	m_SendCheckList[ CmdChatLogin				].push_back( CmdChatLogin				) ; 		// �߰輭���� ä�ü����� Ŭ���̾�Ʈ �α��� ��.
	m_SendCheckList[ CmdChatLogout				].push_back( CmdChatLogout				) ; 		// �߰輭���� ä�ü����� Ŭ���̾�Ʈ �α׾ƿ� ��.
	m_SendCheckList[ CmdChatInfoChanged			].push_back( CmdChatInfoChanged			) ; 		// �߰輭���� ä�ü����� Ŭ���̾�Ʈ ���� ���� ��.
															
	m_SendCheckList[ CmdCharSummon				].push_back( CmdCharSummon				) ;			// ��ȯ
															
	m_SendCheckList[ CmdJapanAuthAccount		].push_back( CmdJapanAuthAccount		) ; 		// �Ϻ��� ���� ���� (ID, Password���, ������ �ڵ尡 �´�.)
															
	m_SendCheckList[ CmdCharBattleGroundRespawn	].push_back( CmdCharBattleGroundRespawn	) ;			// ������ (��Ʋ�׶��� �����)
	m_SendCheckList[ CmdCharRespawnWaitQueue	].push_back( CmdCharRespawnWaitQueue	) ;			// ������ ť ����� ���� (��Ʋ�׶����)
	m_SendCheckList[ CmdStatueInfo				].push_back( CmdStatueInfo				) ;			// ���� ����
	m_SendCheckList[ CmdCameraScript			].push_back( CmdCameraScript			) ;			// ī�޶� ��ũ��Ʈ
	m_SendCheckList[ CmdCharEquipDurability		].push_back( CmdCharEquipDurability		) ;			// ĳ���� ��� ������
															
	m_SendCheckList[ CmdCreateGuild				].push_back( CmdCreateGuild				) ;			// ��� ����
	m_SendCheckList[ CmdGuildCmd				].push_back( CmdGuildCmd				) ;			// ��� ��� ���� ���
	m_SendCheckList[ CmdGuildMark				].push_back( CmdGuildMark				) ;			// ��� ��ũ ����
	m_SendCheckList[ CmdGuildLevel				].push_back( CmdGuildLevel				) ;			// ��� ���� ����
	m_SendCheckList[ CmdGuildRelation			].push_back( CmdGuildRelation			) ;			// ��� ���� ����
	m_SendCheckList[ CmdGuildList				].push_back( CmdGuildList				) ;			// ��� ����Ʈ
	m_SendCheckList[ CmdGuildDB					].push_back( CmdGuildDB					) ;			// ��� ������
	m_SendCheckList[ CmdGuildRight				].push_back( CmdGuildRight				) ;			// ��� ���� ����
	m_SendCheckList[ CmdGuildMemberList			].push_back( CmdGuildMemberList			) ;			// ��� ��� ����Ʈ
	m_SendCheckList[ CmdMyGuildInfo				].push_back( CmdMyGuildInfo				) ;			// �ڱ� ��� ����
	m_SendCheckList[ CmdGuildSafe				].push_back( CmdGuildSafe				) ;			// ��� �ݰ�
	m_SendCheckList[ CmdGuildMemberInfoUpdate	].push_back( CmdGuildMemberInfoUpdate	) ;			// ��� ��� ���� ������Ʈ
															
	m_SendCheckList[ CmdCharStatusRetrain		].push_back( CmdCharStatusRetrain		) ;			// �������ͽ� ���Ʒ� (������ �ٽ� ��������)
	m_SendCheckList[ CmdSysServerLogout			].push_back( CmdSysServerLogout			) ;			// ���� �α׾ƿ�

//	m_SendCheckList[ CmdCharPartyAddress		].push_back( CmdCharPartyAddress		) ;			// ��Ƽ�� UDP �ּ� ����
//	m_SendCheckList[ CmdCharPartyMemAddress		].push_back( CmdCharPartyMemAddress		) ;			// ��Ƽ�� UDP �ּ� ���� (P2P)

	m_SendCheckList[ CmdBGServerMapList			].push_back( CmdBGServerMapList			) ;			// ��Ʋ �׶��� ���� ��(��) ���� ����Ʈ ��û
	m_SendCheckList[ CmdBGServerResultList		].push_back( CmdBGServerResultList		) ;			// ��Ʋ �׶��� ���� ��(��) ��� ����Ʈ ��û
	m_SendCheckList[ CmdBGServerMoveZone		].push_back( CmdBGServerMoveZone		) ;			// ��Ʋ �׶��� ���� ��(��) �̵� (�� �̵�)
	m_SendCheckList[ CmdBGServerMileageChange	].push_back( CmdBGServerMileageChange	) ;			// ��Ʋ �׶��� ���� ȯ���� ���
	m_SendCheckList[ CmdBGServerCharSlot		].push_back( CmdBGServerCharSlot		) ;			// ��Ʋ �׶��� ���� ���� ĳ���� ���� ��û
															
	m_SendCheckList[ CmdHanBTNWarning			].push_back( CmdHanBTNWarning			) ; 		// �Ѱ��� ���պ��� ���� ������ ��� �޽���
	m_SendCheckList[ CmdHanBTNUserKill			].push_back( CmdHanBTNUserKill			) ; 		// �Ѱ��� ���պ��� ���� ���� �޽���
															
	m_SendCheckList[ CmdCharRepairAllItem		].push_back( CmdCharRepairAllItem		) ;			// ��� ������ ��� ����
	m_SendCheckList[ CmdCSAuth					].push_back( CmdCSAuth					) ;			// ���� �ڵ� (���� ����)
	m_SendCheckList[ CmdCharItemChemical		].push_back( CmdCharItemChemical		) ;			// ������ �ռ�
	m_SendCheckList[ CmdItemQtyCheck			].push_back( CmdItemQtyCheck			) ;			// ������ ���� ����
	m_SendCheckList[ CmdGuildInclination		].push_back( CmdGuildInclination		) ;			// ��� ����
	m_SendCheckList[ CmdGuildMemberFameUpdate	].push_back( CmdGuildMemberFameUpdate	) ;			// ��� ��� �� ������Ʈ
															
	m_SendCheckList[ CmdCastleInfo				].push_back( CmdCastleInfo				) ;			// �� ���� ������			(DBAgentServer -> GameServer)
	m_SendCheckList[ CmdCampInfo				].push_back( CmdCampInfo				) ;			// ��� ��� ���� ������	(DBAgentServer -> GameServer)
	m_SendCheckList[ CmdCreateCastle			].push_back( CmdCreateCastle			) ;			// �� ����					(GameServer -> Client)
	m_SendCheckList[ CmdCreateCamp				].push_back( CmdCreateCamp				) ;			// ��� ��� ����			(Client <- GameServer -> DBAgentServer)
	m_SendCheckList[ CmdCreateSiegeArms			].push_back( CmdCreateSiegeArms			) ;			// ���� ���� ����			(Client <- GameServer -> DBAgentServer)
	m_SendCheckList[ CmdCastleCmd				].push_back( CmdCastleCmd				) ;			// �� ���� ��� ��Ŷ		(Client <-> GameServer <-> DBAgentServer)
	m_SendCheckList[ CmdCampCmd					].push_back( CmdCampCmd					) ;			// ��� ��� ���� ��� ��Ŷ	(Client <-> GameServer <-> DBAgentServer)
	m_SendCheckList[ CmdSiegeArmsCmd			].push_back( CmdSiegeArmsCmd			) ;			// ���� ���� ���� ��� ��Ŷ	(Client <-> GameServer <-> DBAgentServer)
	m_SendCheckList[ CmdCastleRight				].push_back( CmdCastleRight				) ;			// �� ���� ���� ��Ŷ		(Client <-> GameServer <-> DBAgentServer)
	m_SendCheckList[ CmdCampRight				].push_back( CmdCampRight				) ;			// ��� ��� ���� ���� ��Ŷ
	m_SendCheckList[ CmdSiegeBroadCast			].push_back( CmdSiegeBroadCast			) ;			// ���� ������Ʈ ��ε� ĳ��Ʈ
	m_SendCheckList[ CmdSiegeTimeInfo			].push_back( CmdSiegeTimeInfo			) ;			// ���� �ð� ����
	m_SendCheckList[ CmdStealthInfo				].push_back( CmdStealthInfo				) ;			// ���ڽ� ��밡�� ����
															
	m_SendCheckList[ CmdCellBroadCast2nd		].push_back( CmdCellBroadCast2nd		) ;			// �� ��ε�ĳ��Ʈ ������ ��Ŷ
															
	m_SendCheckList[ CmdCharRespawnInfo			].push_back( CmdCharRespawnInfo			) ;			// ������ ���� (������ ���� ����Ʈ ����)
	m_SendCheckList[ CmdCharRespawnAreaInfo		].push_back( CmdCharRespawnAreaInfo		) ;			// ������ ������ ������ ���� ����
															
	m_SendCheckList[ CmdCharEquipShopInfo		].push_back( CmdCharEquipShopInfo		) ;			// NPC ��� ���� ����
	m_SendCheckList[ CmdSiegeBroadCast2nd		].push_back( CmdSiegeBroadCast2nd		) ;			// ���� ������Ʈ ��ε� ĳ��Ʈ �ٸ� ����
	m_SendCheckList[ CmdCharItemOptionGraft		].push_back( CmdCharItemOptionGraft		) ;			// ������ �ɼ� �̽�
	m_SendCheckList[ CmdCharItemCompensation	].push_back( CmdCharItemCompensation	) ;			// ������ ���� �Ǹ�
*/															
}

void	
ClientSocket::InitReceiveCheckList()
{
	for (int nIndex = 0; nIndex < CmdFinalPacketNum; ++nIndex)
	{
		m_ReceiveCheckList[nIndex].push_back(nIndex);
	}
}

int	
ClientSocket::CheckFlag( int* arrFlag )			//������
{
	int res = 0;
	for( int i = 0; i < CmdFinalPacketNum ; i++ )
	{
		if( s_pInstance->m_bStatusFlag[i] )
			arrFlag[res++] = i;
	}
	return res;
}