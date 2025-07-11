#include "stdafx.h"
#include "DummyGameHandler.h"
#include "CharacterData.h"

#include "MainFrm.h"

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
#include <Network/ClientSocket/Send/SendMove.h>
#include <Network/ClientSocket/Parse/ParseLoginout.h>
#include <Network/ClientSocket/Parse/ParseItem.h>
#include <Network/ClientSocket/Parse/ParseMove.h>
#include <Network/ClientSocket/Parse/ParseEtc.h>
#include "GMMemory.h"

bool ParseCharLogin(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseCharDepositUpdate(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseCharNameChange(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);


CDummyGameHandler::CDummyGameHandler(CCharacterData& characterData)
:   m_CharacterData(characterData)
{


}


CDummyGameHandler::~CDummyGameHandler()
{


}


int CDummyGameHandler::OnOpen(int iErrorCode)
{
    if(0 != iErrorCode || 
        !SendPacket::CharLogin(m_CharacterData.GetClientSocket(), m_CharacterData.GetUID(), 
        m_CharacterData.GetCID(), m_CharacterData.GetSessionID()))
    {
        // �����ڵ� ����
        CCharacterDataMgr& characterDataMgr = 
            static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

        characterDataMgr.RemoveByAccount(m_CharacterData.GetAccount());
    }
    else
    {
        m_CharacterData.GetClientSocket().SetHandler(this, ClientSocket::GameEventHandler);
    }

    return 0;
}

int CDummyGameHandler::OnClose()
{
    CCharacterDataMgr& characterDataMgr = 
        static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

    characterDataMgr.RemoveByAccount(m_CharacterData.GetAccount());
    m_CharacterData.GetClientSocket().ResetHandler(this, ClientSocket::GameEventHandler);
    return 0;
}

int CDummyGameHandler::OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr)
{
    bool rc = true;

    switch(lpPktBase->GetCmd())
    {
    case CmdCharLogin:          rc = ParseCharLogin(this, lpPktBase, m_CharacterData);           break;
    case CmdDepositUpdate:      rc = ParseCharDepositUpdate(this, lpPktBase, m_CharacterData);   break;
    case CmdCharNameChange:     rc = ParseCharNameChange(this, lpPktBase, m_CharacterData);      break;
    }

    if(!rc)
    {
        CCharacterDataMgr& characterDataMgr = 
            static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

        characterDataMgr.RemoveByAccount(m_CharacterData.GetAccount());
    }

    return 0;
}


bool ParseCharLogin(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    CHAR_INFOST     charInfoST;       
    CHAR_POS		charPos;
    QUICK			quickSlot;
    SKILL           skillSlot;
	SPELL			spell;
    

    unsigned long	dwServerID, dwServerTime;
    unsigned short	wEquipSize, wInvenSize, wExtraSize, wExchangeSize, wTempInven;
    char		   *lpEquipBuffer, *lpInvenBuffer, *lpExtraBuffer, *lpExchangeBuffer, *lpTempInvenBuffer;
    bool			bPeace;
    bool            bCheckRelation;
    unsigned char   cNameChangeCount;
	unsigned char	cAdminFlag;
	unsigned char	cGuildWarFlag;
	unsigned char	cRealmWarFlag;
	unsigned char	cRealmPoint;
	unsigned char	cTacticsFlag;
        
    ClientSocket&   clientSocket = characterData.GetClientSocket();

    unsigned long dwError = ParsePacket::HandleCharLogin(lpPktBase, clientSocket, 
        &bPeace, &bCheckRelation, &cNameChangeCount, &cAdminFlag,
		&cGuildWarFlag, &cRealmWarFlag, &cRealmPoint, &cTacticsFlag,
        &charInfoST, &skillSlot, &quickSlot, &spell,
		&charPos, &dwServerTime, &dwServerID, 
        &wEquipSize, &lpEquipBuffer, &wInvenSize, &lpInvenBuffer,
        &wExtraSize, &lpExtraBuffer, &wExchangeSize, &lpExchangeBuffer, 
        &wTempInven, &lpTempInvenBuffer);

    if (0 == dwError)
    {
        // ���� �����͸� ������ �����Ѵ�.
        // ������ �ʿ� ������ ��....

        // ĳ���� �̸��� �����Ѵ�        
        characterData.SetCharacterName(charInfoST.Name);

        CCharacterDataMgr& characterDataMgr = 
            static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

        characterDataMgr.AddByAccount(&characterData);
    } 
    else
    {        
        const char* szErrorString = 0;

        switch (dwError)
        {
        case 2:     szErrorString = "������ ��� ����";             break;
        case 4:     szErrorString = "�̹� ������";                  break;
        case 24:    szErrorString = "����Ʈ ī�� ����ڰ� �ƴ�";    break;
        }

        std::string szErrorMessage = "CmdCharLogin : ";
        szErrorMessage += szErrorString;

    }

    return true;
}

bool ParseCharDepositUpdate(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, 
                            CCharacterData& characterData)
{
    unsigned long	dwFlag;    
    char*			lpStoreBuffer;

    unsigned short	wStoreSize;
    unsigned char	cTab;
    bool			bComplete;

    ClientSocket&   clientSocket = characterData.GetClientSocket();

    unsigned long dwError = ParsePacket::HandleDepositUpdate(lpPktBase, 
        &dwFlag, &cTab, &bComplete, &wStoreSize, &lpStoreBuffer);

    if (0 == dwError)
    {
        // â�� �����ʹ� �޴��� ������;;;

        if (bComplete)
        {
            // �α��� �����̴�. �̶� ���� ó���� �Ѵ�.
            


        }
    }

    return true;
}

bool ParseCharNameChange(ClientNet::CClientEventHandler* lpHandler, 
                         PktBase* lpPktBase, CCharacterData& characterData)
{
    char* szChangedName = 0; 
    unsigned char cNameChangeCount = 0;

    unsigned long dwError = ParsePacket::HandleCharNameChange(lpPktBase, szChangedName, &cNameChangeCount);

    if(0 == dwError)
    {
        characterData.SetCharacterName(szChangedName);
    }
    else
    {
        switch(dwError)
        {
        case PktCharNameChange::ERR_SERVER:
        case PktCharNameChange::ERR_CANNOT_FIND_CHARACTER:
        case PktCharNameChange::ERR_NAMECHANGE_COUNT:
        case PktCharNameChange::ERR_INVALID_NAME:
        case PktCharNameChange::ERR_CANNOT_USE_NAME:
        case PktCharNameChange::ERR_ALREADY_USE_NAME:

            break;
        }
    }

    return true;
}
