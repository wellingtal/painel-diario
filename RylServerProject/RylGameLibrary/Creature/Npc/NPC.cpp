
#include "stdafx.h"

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Math/Math.h>

#include <Quest/QuestStruct.h>
#include <Quest/QuestMgr.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Item/ItemStructure.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Monster/AwardTable.h>

#include <Community/guild/Guild.h>
#include <Community/guild/GuildMgr.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

#include "NPC.h"

#include <algorithm>

const char* szNPCScriptFileName = "./Script/Game/NPCScript.mcf";

using namespace Quest;


CNPC::GoodInfo::GoodInfo()
:	m_wItemID(0), m_cRace(0), m_cTabPage(0), 
    m_ePreGrade(Item::EquipType::F_GRADE), m_eCurrentGrade(Item::EquipType::F_GRADE)
	
{

}
 

CNPC::GoodInfo::GoodInfo(unsigned short wItemID, unsigned char cRace, unsigned char cTabPage)
:	m_wItemID(wItemID), m_cRace(cRace), m_cTabPage(cTabPage), 
	m_ePreGrade(Item::EquipType::F_GRADE), m_eCurrentGrade(Item::EquipType::F_GRADE)
{

}

class CFindGood : public std::unary_function<CNPC::GoodInfo, bool>
{
public:

	explicit CFindGood(unsigned short wItemID) : m_wItemID(wItemID) { }
    bool operator() (CNPC::GoodInfo& goodInfo) { return (m_wItemID == goodInfo.m_wItemID); }

private:
	const unsigned short	m_wItemID;
};


// NPC ��ũ��Ʈ�� �Լ� ���
namespace NPC
{
    enum NPCFlag
    {
        BELONG_TO_CASTLE            = (1 << 0), // �� ���� NPC�ΰ�?
        REPAIRABLE                  = (1 << 1), // ���� ������ NPC�ΰ�?
        BLACK_MARKETEER             = (1 << 2), // �Ͻ��� �����ΰ�?
        MUST_SELL_WITH_MILEAGE      = (1 << 3), // ���ϸ����� �Ǹ��ϴ� �����ΰ�?
        MUST_SELL_WITH_SKILLCOUPON  = (1 << 4), // ��ų �������� �Ǹ��ϴ� �����ΰ�?
        DISPLAY_EQUIP               = (1 << 5), // ��� �����ִ� �����ΰ�?
    };

    void ScriptErrorMessage(const char *msg);

	void NPCDisplay(int nUID, int nNationType, bool bTestServer, bool bRegularServer, bool bUnifiedServer);
    void SetNPC(int nZone, int nUID, int nTownID, bool bBelongToCastle, const char *strNpcSkin, const char *strNpcName);
    void SetPosition(int nUID, float fDirection, float fPosX, float fPosY, float fPosZ);
    void AddWords(int nUID, const char *strNpcScript);
    void AddDialog(int nUID, int nPage, int nFlag, const char *strNpcScript);
    void AddItem(int nUID, int nRaceID, int nTab, int nPage, int nKindItem);
    void AddZoneMove(int nUID, int nZoneNumber, float fPosX, float fPosY, float fPosZ);
    void AddQuest(int nNPCID, int nQuestID);
    void SetDropGrade(int nUID, int nDropType, int nGradeF, int nGradeD, int nGradeC, int nGradeB, int nGradeA);
    void SetDropBase(int nUID, int nDropType, int nGradeF, int nGradeD, int nGradeC, int nGradeB, int nGradeA);
    void AddQuestWords(int nUID, int nQuestID, const char* strHelpMessage);
    void AddPopup(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, int nQuestID, int nLimitLevel, int nLimitClass);
    void AddPopupGuild(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, int nQuestID, int nLimitLevel, int nLimitFame);
    void AddPopupFame(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, int nQuestID, int nLimitFame, int nLimitClass);
    void AddPopupAbility(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, int nAbilityID, int nAbilityLV, int nLimitClass);
	
    void SetNPCAttribute(int nUID, int nMinimapIconID, int nNation);
};

bool CNPC::LoadNPCInfo()
{
	CCreatureManager::GetInstance().DestoryNPCList();

	_SE_SetMessageFunction(NPC::ScriptErrorMessage);
	
	SCRIPT Script = _SE_Create(szNPCScriptFileName);
    if (NULL == Script) { return false; }

	_SE_RegisterFunction(Script, NPC::NPCDisplay, T_VOID, "NPCDisplay", T_INT, T_INT, T_BOOL, T_BOOL, T_BOOL, 0 ) ;
    _SE_RegisterFunction(Script, NPC::SetNPC, T_VOID, "SetNPC", T_INT, T_INT, T_INT, T_BOOL, T_STRING, T_STRING, 0);
	_SE_RegisterFunction(Script, NPC::SetPosition, T_VOID, "SetPosition", T_INT, T_FLOAT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, NPC::AddWords, T_VOID, "AddWords", T_INT, T_STRING, 0);
	_SE_RegisterFunction(Script, NPC::AddDialog, T_VOID, "AddDialog", T_INT, T_INT, T_INT, T_STRING, 0);
	_SE_RegisterFunction(Script, NPC::AddItem, T_VOID, "AddItem", T_INT, T_INT, T_INT, T_INT, T_INT, 0);	
	_SE_RegisterFunction(Script, NPC::AddZoneMove, T_VOID, "AddZoneMove", T_INT, T_INT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, NPC::AddQuest, T_VOID, "AddQuest", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::SetDropGrade, T_VOID, "SetDropGrade", T_INT, T_INT, T_INT, T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::SetDropBase, T_VOID, "SetDropBase", T_INT, T_INT, T_INT, T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::AddQuestWords, T_VOID, "AddQuestWords", T_INT, T_INT, T_STRING, 0);
	_SE_RegisterFunction(Script, NPC::AddPopup, T_VOID, "AddPopup", T_INT, T_INT, T_INT, T_STRING, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::AddPopupGuild, T_VOID, "AddPopupGuild", T_INT, T_INT, T_INT, T_STRING, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::AddPopupFame, T_VOID, "AddPopupFame", T_INT, T_INT, T_INT, T_STRING, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::AddPopupAbility, T_VOID, "AddPopupAbility", T_INT, T_INT, T_INT, T_STRING, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, NPC::SetNPCAttribute, T_VOID, "SetNPCAttribute", T_INT, T_INT, T_INT, 0);

	_SE_Execute(Script);
	_SE_Destroy(Script);

    CCreatureManager::GetInstance().ProcessAllNPC(std::mem_fun(&CNPC::SortGoods));
	CCreatureManager::GetInstance().ProcessAllNPC(std::mem_fun(&CNPC::SortQuests));

	return true;
}


void NPC::ScriptErrorMessage(const char *msg) 
{
	MessageBox(NULL, msg, "Script Error", MB_OK);
}

void NPC::NPCDisplay(int nUID, int nNationType, bool bTestServer, bool bRegularServer, bool bUnifiedServer)
{
}

void NPC::SetNPC(int nZone, int nUID, int nTownID, bool bBelongToCastle, const char *strNpcSkin, const char *strNpcName)
{
	CNPC* lpNPC = new CNPC(nUID, nZone, nTownID, bBelongToCastle);
	if (!CCreatureManager::GetInstance().AddCreature(lpNPC))
    {
		ERRLOG1(g_Log, "NID:0x%08x NPC��Ͽ� �����߽��ϴ�.", nUID);
        delete lpNPC;
    }
}

void NPC::SetPosition(int nUID, float fDirection, float fPosX, float fPosY, float fPosZ)
{
    // NPC��ġ�� �����Ѵ�.
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (0 != lpNPC) 
    {
        lpNPC->SetNPCPosition(Position(fPosX, fPosY, fPosZ));
    }
}

void NPC::AddWords(int nUID, const char *strCNPC)
{

}

void NPC::AddDialog(int nUID, int nPage, int nFlag, const char *strCNPC)
{

}

void NPC::AddItem(int nUID, int nRaceID, int nTab, int nPage, int nKindItem)
{
    // Item�� �����Ѵ�.
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
    {
        CNPC::GoodInfo goodInfo(nKindItem, nRaceID, nTab);

        lpNPC->AddGoodsToNPC(goodInfo);
		lpNPC->SetRandomGrade();
	}
}

void NPC::AddZoneMove(int nUID, int nZoneNumber, float fPosX, float fPosY, float fPosZ)
{
}

void NPC::AddQuest(int nNPCID, int nQuestID)
{
	QuestNode* lpQuestNode = CQuestMgr::GetInstance().GetQuestNode(nQuestID);
	if (lpQuestNode)
	{
		CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nNPCID);
		if (NULL != lpNPC) 
		{
			lpNPC->AddQuestsToNPC(nQuestID);
		}
	}
	else
	{
		char szErrMsg[MAX_PATH];
		sprintf(szErrMsg, "NPC�� ����� Quest�� �������� �ʽ��ϴ�. NPCID:0x%08x QuestID:0x%04x", nNPCID, nQuestID);
		MessageBox(NULL, szErrMsg, "Script Error", MB_OK);
	}
}

void NPC::SetDropGrade(int nUID, int nDropType, int nGradeF, int nGradeD, int nGradeC, int nGradeB, int nGradeA)
{
	// �� �׷��̵庰�� �������� ������ Ȯ���� �����Ѵ�.
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
		lpNPC->SetItemDropGrade(nDropType, nGradeF, nGradeD, nGradeC, nGradeB, nGradeA);
	}
}

void NPC::SetDropBase(int nUID, int nDropType, int nGradeF, int nGradeD, int nGradeC, int nGradeB, int nGradeA)
{
	// �� �׷��̵庰�� �پ�� �ɼ��� ����(�ּҰ�)�� �����Ѵ�.
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
		lpNPC->SetItemDropBaseNum(nDropType, nGradeF, nGradeD, nGradeC, nGradeB, nGradeA);
	}
}

void NPC::AddQuestWords(int nUID, int nQuestID, const char* strHelpMessage)
{
}

void NPC::AddPopup(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, 
                   int nQuestID, int nLimitLevel, int nLimitClass)
{
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
        unsigned long dwFlags = 0;

    	switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������
			case 13:    // �ΰ� �����
			case 14:    // �ΰ� ���� & ������
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
			case 21:    // �ΰ� ��������
			case 27:	// �ΰ� �޴޻��� 
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����
			case 31:    // �ΰ� ���谡 Ʈ���̳�
			case 32:    // �ΰ� ���� Ʈ���̳�
			case 33:    // �ΰ� �α� Ʈ���̳�
			case 34:    // �ΰ� ������ Ʈ���̳�
			case 35:    // �ΰ� ������ Ʈ���̳�    
			case 42:    // �ΰ� ��������
		   
			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������
			case 63:    // ��ĭ �����
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��
			case 71:    // ��ĭ ��������
			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����
			case 81:    // ��ĭ ���� Ʈ���̳�
			case 82:    // ��ĭ �Ĺ���Ʈ Ʈ���̳�
			case 83:    // ��ĭ ���Ǽ����� Ʈ���̳�    
			case 92:    // ��ĭ ��������

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
		    
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// �ΰ� �޴޻���
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����

			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// ���谡 ���� Ʈ���̳�
			case 124:	// ���� ���� Ʈ���̳�
			case 125:	// �α� ���� Ʈ���̳�
			case 126:	// ������ ���� Ʈ���̳�
			case 127:	// ������ ���� Ʈ���̳�

			case 128:	// ���� ���� Ʈ���̳�
			case 129:	// �Ĺ���Ʈ ���� Ʈ���̳�
			case 130:	// ���Ǽ����� ���� Ʈ���̳�

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������

			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������

				dwFlags |= NPC::DISPLAY_EQUIP; 
				break;
		}

		lpNPC->SetFlags(dwFlags);
	}
}

void NPC::AddPopupGuild(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, 
                   int nQuestID, int nLimitLevel, int nLimitFame)
{
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
        unsigned long dwFlags = 0;

    	switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������
			case 13:    // �ΰ� �����
			case 14:    // �ΰ� ���� & ������
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
			case 21:    // �ΰ� ��������
			case 27:	// �ΰ� �޴޻��� 
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����
			case 31:    // �ΰ� ���谡 Ʈ���̳�
			case 32:    // �ΰ� ���� Ʈ���̳�
			case 33:    // �ΰ� �α� Ʈ���̳�
			case 34:    // �ΰ� ������ Ʈ���̳�
			case 35:    // �ΰ� ������ Ʈ���̳�    
			case 42:    // �ΰ� ��������
		   
			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������
			case 63:    // ��ĭ �����
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��
			case 71:    // ��ĭ ��������
			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����
			case 81:    // ��ĭ ���� Ʈ���̳�
			case 82:    // ��ĭ �Ĺ���Ʈ Ʈ���̳�
			case 83:    // ��ĭ ���Ǽ����� Ʈ���̳�    
			case 92:    // ��ĭ ��������

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
		    
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// �ΰ� �޴޻���
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����

			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// ���谡 ���� Ʈ���̳�
			case 124:	// ���� ���� Ʈ���̳�
			case 125:	// �α� ���� Ʈ���̳�
			case 126:	// ������ ���� Ʈ���̳�
			case 127:	// ������ ���� Ʈ���̳�

			case 128:	// ���� ���� Ʈ���̳�
			case 129:	// �Ĺ���Ʈ ���� Ʈ���̳�
			case 130:	// ���Ǽ����� ���� Ʈ���̳�

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������

			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������

				dwFlags |= NPC::DISPLAY_EQUIP; 
				break;
		}

		lpNPC->SetFlags(dwFlags);
	}
}

void NPC::AddPopupFame(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, int nQuestID, int nLimitFame, int nLimitClass)
{
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
        unsigned long dwFlags = 0;

    	switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������
			case 13:    // �ΰ� �����
			case 14:    // �ΰ� ���� & ������
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
			case 21:    // �ΰ� ��������
			case 27:	// �ΰ� �޴޻��� 
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����
			case 31:    // �ΰ� ���谡 Ʈ���̳�
			case 32:    // �ΰ� ���� Ʈ���̳�
			case 33:    // �ΰ� �α� Ʈ���̳�
			case 34:    // �ΰ� ������ Ʈ���̳�
			case 35:    // �ΰ� ������ Ʈ���̳�    
			case 42:    // �ΰ� ��������
		   
			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������
			case 63:    // ��ĭ �����
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��
			case 71:    // ��ĭ ��������
			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����
			case 81:    // ��ĭ ���� Ʈ���̳�
			case 82:    // ��ĭ �Ĺ���Ʈ Ʈ���̳�
			case 83:    // ��ĭ ���Ǽ����� Ʈ���̳�    
			case 92:    // ��ĭ ��������

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
		    
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// �ΰ� �޴޻���
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����

			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// ���谡 ���� Ʈ���̳�
			case 124:	// ���� ���� Ʈ���̳�
			case 125:	// �α� ���� Ʈ���̳�
			case 126:	// ������ ���� Ʈ���̳�
			case 127:	// ������ ���� Ʈ���̳�

			case 128:	// ���� ���� Ʈ���̳�
			case 129:	// �Ĺ���Ʈ ���� Ʈ���̳�
			case 130:	// ���Ǽ����� ���� Ʈ���̳�

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������

			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������

				dwFlags |= NPC::DISPLAY_EQUIP; 
				break;
		}

		lpNPC->SetFlags(dwFlags);
	}
}

void NPC::AddPopupAbility(int nUID, int nPopupKind, int nPopupFunction, const char* strPopupString, int nAbilityID, int nAbilityLV, int nLimitClass)
{
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
        unsigned long dwFlags = 0;

    	switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������
			case 13:    // �ΰ� �����
			case 14:    // �ΰ� ���� & ������
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
			case 21:    // �ΰ� ��������
			case 27:	// �ΰ� �޴޻��� 
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����
			case 31:    // �ΰ� ���谡 Ʈ���̳�
			case 32:    // �ΰ� ���� Ʈ���̳�
			case 33:    // �ΰ� �α� Ʈ���̳�
			case 34:    // �ΰ� ������ Ʈ���̳�
			case 35:    // �ΰ� ������ Ʈ���̳�    
			case 42:    // �ΰ� ��������
		   
			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������
			case 63:    // ��ĭ �����
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��
			case 71:    // ��ĭ ��������
			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����
			case 81:    // ��ĭ ���� Ʈ���̳�
			case 82:    // ��ĭ �Ĺ���Ʈ Ʈ���̳�
			case 83:    // ��ĭ ���Ǽ����� Ʈ���̳�    
			case 92:    // ��ĭ ��������

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // �ΰ� �ϻ� - �����
			case 17:    // �ΰ� �ϻ� - ����
			case 18:    // �ΰ� �ϻ� - �� + ��ų�� 
		    
			case 66:    // ��ĭ �ϻ� - �����
			case 67:    // ��ĭ �ϻ� - ����
			case 68:    // ��ĭ �ϻ� - �� + ��ų��

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// �ΰ� �޴޻���
			case 28:	// �ΰ� �޴޻��� �����
			case 29:	// �ΰ� �޴޻��� ����

			case 77:	// ��ĭ �޴޻��� 
			case 78:	// ��ĭ �޴޻��� �����
			case 79:	// ��ĭ �޴޻��� ����

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// ���谡 ���� Ʈ���̳�
			case 124:	// ���� ���� Ʈ���̳�
			case 125:	// �α� ���� Ʈ���̳�
			case 126:	// ������ ���� Ʈ���̳�
			case 127:	// ������ ���� Ʈ���̳�

			case 128:	// ���� ���� Ʈ���̳�
			case 129:	// �Ĺ���Ʈ ���� Ʈ���̳�
			case 130:	// ���Ǽ����� ���� Ʈ���̳�

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // �ΰ� �������
			case 12:    // �ΰ� ������

			case 61:    // ��ĭ �������
			case 62:    // ��ĭ ������

				dwFlags |= NPC::DISPLAY_EQUIP; 
				break;
		}

		lpNPC->SetFlags(dwFlags);
	}
}

void NPC::SetNPCAttribute(int nUID, int nMinimapIconID, int nNation)
{
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
		lpNPC->SetNation(nNation);
	}
}


CNPC::CNPC(unsigned long dwCID, int nZone, int nTownID, bool bBelongToCastle)
:	CCreature(dwCID), m_nZone(nZone), m_dwTownOrNameID(nTownID), 
	m_dwGID(0), m_cNation(Creature::STATELESS), m_dwFlags(0), m_dwDisplayTime(0)
{
    m_dwFlags |= bBelongToCastle ? NPC::BELONG_TO_CASTLE : 0;

	if (bBelongToCastle && (nTownID & Castle::CASTLE_BIT))
	{
		m_dwTownOrNameID = ((nTownID & ~Castle::CASTLE_BIT) >> Castle::CASTLE_NAME_BIT_SHIFT);
	}

	for (int i = 0; i < Item::EquipType::MAX_OPTION_TYPE; ++i)
	{
		for (int j = 0; j < Item::EquipType::MAX_GRADE; ++j)
		{
			m_cGradeRate[i][j] = 0;
			m_cBaseNum[i][j] = 0;
		}
	}
}

CNPC::~CNPC()
{

}

bool CNPC::IsBelongToCastle(void) const
{
    return (m_dwFlags & NPC::BELONG_TO_CASTLE);
}

unsigned char CNPC::GetNation(void) const
{
	if (0 != m_dwGID)
	{
		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
		if (lpGuild)
		{
			return lpGuild->GetNation();
		}
	}

	return m_cNation;
}

Item::CItem* CNPC::SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
								   Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError)
{
	unsigned long dwCurrentGold = lpCustomer->GetGold();
	unsigned long dwCurrentMileage = lpCustomer->GetMileage();
	unsigned char cBaseLevel = lpCustomer->GetStatus().m_nLevel;

	unsigned char cNum = takeType.m_cNum;
	float		  fTax = 0.0f;
	unsigned long dwTaxIncome = 0;
	Castle::CCastle* lpCastle = NULL;

	// ���� ���� NPC ���, �ŷ� ���� ����
	if (IsBelongToCastle())
	{
		lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByNameID(m_dwTownOrNameID);
		if (lpCastle)
		{
			fTax = lpCastle->GetTax(Castle::TRADE_TAX) / 100.f;
		}
	}

	enum {
		HACK_BLACK_RANDOM = 0,		// �Ͻ��� ���� : �����ɼ� ��� �����ϰ� �߽��ϴ�
		HACK_BLACK_NOTEXITSITEM,	// �Ͻ��� ���� : �������� �ʴ� �������Դϴ�
        HACK_MILEAGE_RANDOM,		// �޴޻��� ���� : �����ɼ� ��� �����ϰ� �߽��ϴ�
		HACK_MILEAGE_NOTEXITSITEM,	// �޴޻��� ���� : �������� �ʴ� �������Դϴ�
		HACK_SHOP_NOTEXITSITEM,		// ���� ���� : ������� �ʴ� �������� �����մϴ�.
		HACK_SHOP_NOEQUIP,			// ���� ���� : ��� �ƴ� �������� �����Ϸ� �մϴ�
		HACK_SHOP_NOTGRADEITEM,		// ���� ���� : �Ǹ����� �ʴ� �׷��̵��� �������� ��û�Ͽ����ϴ�
		HACK_SHOP_NOFGRADEITEM,		// ���� ���� : F�׷��̵尡 �ƴ� �������� ��û�Ͽ����ϴ�
		HACK_SHOP_SOCKETITEM,		// ���� ���� : ������ �� ���� ������ �ִ� �������� ��û�Ͽ����ϴ�
		HACK_SHOP_OVERDURABILITY,	// ���� ���� : �������� 180 �̻��� �������� �����Ǿ����ϴ�.
	};

	
	// ---------------------------------------------------------------------------------------------
	// �Ͻ���
    if (m_dwFlags & NPC::BLACK_MARKETEER)
	{
		// edith 2009.07.20 �Ͻ��忡 �����ɼ� ��� ��� ��Ŷ�� ���´�.
		if(lpRequestItem)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_BLACK_RANDOM, wKindItem, lpRequestItem, usError);
			ERRLOG1(g_Log, "�Ͻ��� ���� : �����ɼ� ��� �����ϰ� �߽��ϴ�. ItemID:%d", wKindItem);
			return NULL;
		}

		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
		if (NULL == lpItemInfo)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_BLACK_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "�Ͻ��� ���� : �������� �ʴ� �������Դϴ�. ItemID:%d", wKindItem);
			return NULL;
		}

		dwPrice = lpItemInfo->m_DetailData.m_dwBlackPrice * cNum;
		dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
		dwPrice += dwTaxIncome;

        if (dwPrice > dwCurrentGold)
		{
			ERRLOG2(g_Log, "�Ͻ��� ���� : ���� �����մϴ�. ����:%d, ������:%d", dwPrice, dwCurrentGold);
			return NULL;
		}

		Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(*lpItemInfo);
		if (NULL == lpItem)
		{
			ERRLOG0(g_Log, "�Ͻ��� ���� : ������ ������ �����Ͽ����ϴ�.");
			return NULL;
		}

		Item::EquipType::OptionType optionType = Item::CItemType::GetInstance().GetOptionType(
			static_cast<Item::ItemType::Type>(lpItem->GetItemInfo().m_DetailData.m_cItemType));

		const unsigned long dwPoint = Math::Random::ComplexRandom(
			accumulate(m_cGradeRate[optionType], m_cGradeRate[optionType] + Item::EquipType::MAX_GRADE, 0));
		unsigned long dwSpace = 0;

		for (unsigned char cGradeIndex = 0; cGradeIndex < Item::EquipType::MAX_GRADE; ++cGradeIndex)
		{
			dwSpace += m_cGradeRate[optionType][cGradeIndex];
			if (dwSpace > dwPoint)
			{
				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL == lpEquip)
				{
					ERRLOG0(g_Log, "�Ͻ��� ���� : �ɼ� ������ �����Ͽ����ϴ�.");
					return NULL;
				}

				int iMagicChancePoint = 0;
				if(lpCustomer->GetMagicChancePoint() > 0)
					iMagicChancePoint = (int)(lpCustomer->GetMagicChancePoint()/2);

				// edith 2009.08.18 �������� ������ 50���η� ������ ���
				//blackmaket dont give 2 slots
				//lpEquip->SetSocketNum(2);
				if(lpEquip->AddRandomOption(static_cast<Item::EquipType::Grade>(cGradeIndex), m_cBaseNum[optionType][cGradeIndex], iMagicChancePoint))
					lpEquip->SetNewEquip(2);
				else
					lpEquip->SetNewEquip();

				if (dwTaxIncome > 0)
				{
					lpCastle->AddTempTaxMoney(Castle::TRADE_TAX, dwTaxIncome);
				}

				return lpItem;
			}
		}

		ERRLOG0(g_Log, "�Ͻ��� ���� : ������ �׷��̵� ������ �����Ͽ����ϴ�.");
		return NULL;
	}

	// ---------------------------------------------------------------------------------------------
	// �޴޻���
	if (m_dwFlags & NPC::MUST_SELL_WITH_MILEAGE)
	{
		// edith 2009.07.20 �Ͻ��忡 �����ɼ� ��� ��� ��Ŷ�� ���´�.
		if(lpRequestItem)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_MILEAGE_RANDOM, wKindItem, lpRequestItem, usError);
			ERRLOG1(g_Log, "�޴޻��� ���� : �����ɼ� ��� �����ϰ� �߽��ϴ�. ItemID:%d", wKindItem);
			return NULL;
		}

		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
		if (NULL == lpItemInfo)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_MILEAGE_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "�޴޻��� ���� : �������� �ʴ� �������Դϴ�. ItemID:%d", wKindItem);
			return NULL;
		}

		dwPrice = lpItemInfo->m_DetailData.m_dwMedalPrice * cNum;
		if (dwPrice > dwCurrentMileage)
		{
			ERRLOG6(g_Log, "NID:0x%08x ����޴��� ���ڶ��ϴ�. ���� ����޴��� %d�Դϴ�. "
				"(����ID:%d, 1�� ����:%d, ����:%d, ��ü �ݾ�:%d)",
				m_dwCID, dwCurrentMileage, wKindItem, dwPrice, cNum, dwPrice * cNum);
			return NULL;
		}

		bool bStackable = Item::DetailData::STACKABLE == (lpItemInfo->m_DetailData.m_dwFlags & Item::DetailData::STACKABLE);                               

		if (false == bStackable || (bStackable && cNum <= lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack))
		{
			Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(*lpItemInfo);

			if (NULL != lpItem)
			{
				if (true == bStackable) 
				{
					lpItem->SetNumOrDurability(cNum);
				}

				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL != lpEquip)
				{
					Item::EquipType::OptionType optionType = Item::CItemType::GetInstance().GetOptionType(
						static_cast<Item::ItemType::Type>(lpItem->GetItemInfo().m_DetailData.m_cItemType));

					const unsigned long dwPoint = Math::Random::ComplexRandom(
						accumulate(m_cGradeRate[optionType], m_cGradeRate[optionType] + Item::EquipType::MAX_GRADE, 0));
					unsigned long dwSpace = 0;

					for (unsigned char cGradeIndex = 0; cGradeIndex < Item::EquipType::MAX_GRADE; ++cGradeIndex)
					{
						dwSpace += m_cGradeRate[optionType][cGradeIndex];
						if (dwSpace > dwPoint)
						{
							Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
							if (NULL == lpEquip)
							{
								ERRLOG0(g_Log, "�޴޻��� ���� : �ɼ� ������ �����Ͽ����ϴ�.");
								return NULL;
							}

							//lpEquip->AddRandomOption(static_cast<Item::EquipType::Grade>(cGradeIndex), m_cBaseNum[optionType][cGradeIndex]);//change this hz wasnt commented
							lpEquip->SetNewEquip();
							lpEquip->SetSocketNum(2);
							break;
						}
					}
				}
			}
			else
			{
				ERRLOG1(g_Log, "NID:0x%08x ������ ������ �����߽��ϴ�", m_dwCID);
			}

			return lpItem;
		}
		else
		{
			ERRLOG4(g_Log, "NID:0x%08x ������ �̻��մϴ�. bStackable:%d, cNum:%d, MaxNumOrDurability:%d",
				m_dwCID, bStackable, cNum, lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack);
		}

		ERRLOG0(g_Log, "�޴޻��� ���� : ������ �׷��̵� ������ �����Ͽ����ϴ�.");
		return NULL;
	}

	// ---------------------------------------------------------------------------------------------
	// ����

	if (NULL != lpRequestItem)//normal shop sell hz check one of the lpitem functions to fix no stat
	{
		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
		if (NULL == lpItemInfo)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_BLACK_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "�Ͻ��� ���� : �������� �ʴ� �������Դϴ�. ItemID:%d", wKindItem);
			return NULL;
		}

		dwPrice = lpRequestItem->GetBuyPrice();
		float fd = lpCustomer->GetAbilityValue(Skill::Type::AB_SHOP_MEMBERSHIP);
		dwPrice -= static_cast<unsigned long>((float)dwPrice*fd/100.0f);

		dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
		dwPrice += dwTaxIncome;

        if (dwPrice > dwCurrentGold)
		{
			ERRLOG2(g_Log, "�Ͻ��� ���� : ���� �����մϴ�. ����:%d, ������:%d", dwPrice, dwCurrentGold);
			return NULL;
		}
		
		Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(*lpItemInfo);
		if (NULL == lpItem)
		{
			ERRLOG0(g_Log, "�Ͻ��� ���� : ������ ������ �����Ͽ����ϴ�.");
			return NULL;
		}

		Item::EquipType::OptionType optionType = Item::CItemType::GetInstance().GetOptionType(
			static_cast<Item::ItemType::Type>(lpItem->GetItemInfo().m_DetailData.m_cItemType));

		const unsigned long dwPoint = Math::Random::ComplexRandom(
			accumulate(m_cGradeRate[optionType], m_cGradeRate[optionType] + Item::EquipType::MAX_GRADE, 0));
		unsigned long dwSpace = 0;

		//for (unsigned char cGradeIndex = 0; cGradeIndex < Item::EquipType::MAX_GRADE; ++cGradeIndex)
		//{
			//if (dwSpace > dwPoint)
			//{
				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL == lpEquip)
				{
					ERRLOG0(g_Log, "�Ͻ��� ���� : �ɼ� ������ �����Ͽ����ϴ�.");
					return NULL;
				}

				int iMagicChancePoint = 0;
				if(lpCustomer->GetMagicChancePoint() > 0)
					iMagicChancePoint = (int)(lpCustomer->GetMagicChancePoint()/2);

				// edith 2009.08.18 �������� ������ 50���η� ������ ���
				//blackmaket dont give 2 slots
				
				//if(lpEquip->AddRandomOption(static_cast<Item::EquipType::Grade>(cGradeIndex), m_cBaseNum[optionType][cGradeIndex], iMagicChancePoint))
				//	lpEquip->SetNewEquip(2);
			//	else
					lpEquip->SetNewEquip();

				if (dwTaxIncome > 0)
				{
					lpCastle->AddTempTaxMoney(Castle::TRADE_TAX, dwTaxIncome);
				}
				lpEquip->SetSocketNum(2);

				return lpItem;
			//}
		//}

		ERRLOG0(g_Log, "�Ͻ��� ���� : ������ �׷��̵� ������ �����Ͽ����ϴ�.");
		return NULL;
		/*CFindGood findGood(lpRequestItem->GetPrototypeID());
		std::vector<GoodInfo>::iterator it = std::find_if(m_Goods.begin(), m_Goods.end(), findGood);

		if (it == m_Goods.end())
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG2(g_Log, "NID:0x%08x ���� ���� : �� NPC�� %d������ �������� ������� �ʽ��ϴ�.", 
				m_dwCID, lpRequestItem->GetPrototypeID());

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpRequestItem);
		if (NULL == lpEquip)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOEQUIP, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "���� ���� : ��� �ƴ� �������� �����Ϸ� �մϴ�. ������ID:%d", 
				lpRequestItem->GetPrototypeID());

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		Item::EquipType::OptionType optionType = Item::CItemType::GetInstance().GetOptionType(
			static_cast<Item::ItemType::Type>(lpEquip->GetItemInfo().m_DetailData.m_cItemType));
		Item::Grade::GradeInfo gradeInfo = lpEquip->GetItemGrade();

		GoodInfo& goodInfo = *it;

		if (0 == m_cGradeRate[optionType][gradeInfo.m_eItemGrade])
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOTGRADEITEM, wKindItem, lpEquip, usError);
			ERRLOG2(g_Log, "NID:0x%08x ���� ���� : �Ǹ����� �ʴ� �׷��̵��� �������� ��û�Ͽ����ϴ�. "
				"��û �������� �׷��̵� - %d", m_dwCID, gradeInfo.m_eItemGrade);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		// edith 2009.07.24 ���� ��� F�׷��̵尡 �ƴϸ� ���Ű� �Ұ��� Disconnect ��Ų��.
		if(Item::EquipType::F_GRADE != gradeInfo.m_eItemGrade)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOFGRADEITEM, wKindItem, lpEquip, usError);

			ERRLOG2(g_Log, "NID:0x%08x ���� ���� : F�׷��̵尡 �ƴ� �������� ��û�Ͽ����ϴ�. "
				"��û �������� �׷��̵� - %d", m_dwCID, gradeInfo.m_eItemGrade);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		// edith 2009.07.24 ���� ��� ������ �پ����� ���Ű� �Ұ���. Disconnect ��Ų��.
		if( 0 > lpEquip->GetMaxSocketNum() )
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_SOCKETITEM, wKindItem, lpEquip, usError);

			ERRLOG2(g_Log, "NID:0x%08x ���� ���� : ������ �� ���� ������ �ִ� �������� ��û�Ͽ����ϴ�. "
				"��û �������� �׷��̵� - %d", m_dwCID, gradeInfo.m_eItemGrade);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}
		lpEquip->SetSocketNum(2);
		// edith 2009.07.24 ���� ����� �������� 180�̻��̸� �� �ǽ����� �α׸� �����.
		if( 180 <= lpEquip->GetMaxNumOrDurability() )
		{
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_OVERDURABILITY, wKindItem, lpEquip, usError);
		}

		dwPrice = lpRequestItem->GetBuyPrice();

		float fd = lpCustomer->GetAbilityValue(Skill::Type::AB_SHOP_MEMBERSHIP);
		dwPrice -= static_cast<unsigned long>((float)dwPrice*fd/100.0f);

		dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
		dwPrice += dwTaxIncome;

		if (dwPrice > dwCurrentGold)
		{
			ERRLOG2(g_Log, "���� ���� : ���� �����մϴ�. ����:%d, ������:%d", dwPrice, dwCurrentGold);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		if (dwTaxIncome > 0)
		{
			lpCastle->AddTempTaxMoney(Castle::TRADE_TAX, dwTaxIncome);
		}

		// �������� �Ĵ� �������� �������� Ǯ�� ��ǰ�̾�� �Ѵ�.
		lpRequestItem->SetNumOrDurability(lpRequestItem->GetMaxNumOrDurability());

		return lpRequestItem;*/
	}



	// ---------------------------------------------------------------------------------------------
	// �Ϲ� ����

	CFindGood findGood(wKindItem);
	std::vector<GoodInfo>::iterator it = std::find_if(m_Goods.begin(), m_Goods.end(), findGood);

	if (it != m_Goods.end())
	{
        const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
        if (NULL != lpItemInfo)
        {
			// ����(3,4�ܰ� ��ų��)���� ����
			if (m_dwFlags & NPC::MUST_SELL_WITH_SKILLCOUPON)
			{
				bool bError = false;
				switch (wCouponID)
				{
					case Item::EtcItemID::SKILL_COUPON_3:	
						if (2 != lpItemInfo->m_UseItemInfo.m_usSkill_LockCount)	{ bError = true; }		
						break;

					case Item::EtcItemID::SKILL_COUPON_4:	
						if (3 != lpItemInfo->m_UseItemInfo.m_usSkill_LockCount)	{ bError = true; }		
						break;

					default:				
						bError = true;	
						break;
				}			

				if (true == bError)
				{
					ERRLOG3(g_Log, "NID:0x%08x �˸��� ������ �ƴմϴ�. ����ID:%d, ���Թ�ǰID:%d", 
						m_dwCID, wCouponID, wKindItem);
					return NULL;
				}
			}
			// ������ ����
			else
			{
				dwPrice = lpItemInfo->m_DetailData.m_dwPrice * cNum;

				float fd = lpCustomer->GetAbilityValue(Skill::Type::AB_SHOP_MEMBERSHIP);
				dwPrice -= static_cast<unsigned long>((float)dwPrice*fd/100.0f);

				dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
				dwPrice += dwTaxIncome;

				if (dwPrice > dwCurrentGold)
				{
					ERRLOG6(g_Log, "NID:0x%08x ���� ���ڶ��ϴ�. ���� �������� %d���Դϴ�. "
						"(����ID:%d, 1�� ����:%d, ����:%d, ��ü �ݾ�:%d)",
						m_dwCID, dwCurrentGold, wKindItem, dwPrice, cNum, dwPrice * cNum);
					return NULL;
				}
			}

			bool bStackable = Item::DetailData::STACKABLE == (lpItemInfo->m_DetailData.m_dwFlags & Item::DetailData::STACKABLE);                               

			if (false == bStackable || (bStackable && cNum <= lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack))
			{
				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(*lpItemInfo);

                if (NULL != lpItem)
                {
					if (true == bStackable) 
                    {
                        lpItem->SetNumOrDurability(cNum);
                    }

					Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
					if (NULL != lpEquip)
					{
						lpEquip->SetNewEquip();
						/* Generates 2 socket automatically in every created item, (ignores "Shirt") #slot #socket */
						if((lpEquip->GetItemInfo().m_DetailData.m_cItemType != Item::EquipmentPos::SHIRT)&&(lpEquip->GetItemInfo().m_DetailData.m_cItemType != Item::EquipmentPos::TUNIC))
							lpEquip->SetSocketNum(2);
					}

					if (dwTaxIncome > 0)
					{
						lpCastle->AddTempTaxMoney(Castle::TRADE_TAX, dwTaxIncome);
					}
				}
                else
                {
                    ERRLOG1(g_Log, "NID:0x%08x ������ ������ �����߽��ϴ�", m_dwCID);
                }
				
				return lpItem;
			}
            else
            {
                ERRLOG4(g_Log, "NID:0x%08x ������ �̻��մϴ�. bStackable:%d, cNum:%d, MaxNumOrDurability:%d",
                    m_dwCID, bStackable, cNum, lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack);
			}
        }
        else
        {
            ERRLOG2(g_Log, "NID:0x%08x �� NPC�� ������ ��ũ��Ʈ�� ���� %d������ �������� �ȷ��� �Ͽ����ϴ�.",
                m_dwCID, wKindItem);
        }
    }
    else
    {
        ERRLOG2(g_Log, "NID:0x%08x �� NPC�� %d������ �������� ������� �ʽ��ϴ�.", m_dwCID, wKindItem);
    }

	return NULL;
}


unsigned long CNPC::RepairItem(Item::CEquipment* lpEquipment, unsigned long& dwCurrentGold) 
{
    if (NULL != lpEquipment && (m_dwFlags & NPC::REPAIRABLE))
    {
	    unsigned long dwRepairGold = lpEquipment->GetRepairPrice();

		// ���� ���� NPC ��� ���� ���� ����
		if (IsBelongToCastle())
		{
			CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByNameID(m_dwTownOrNameID);
			if (NULL != lpCastle)
			{
				const unsigned long dwRepairTax = static_cast<unsigned long>(dwRepairGold * (lpCastle->GetTax(Castle::REPAIR_TAX) / 100.0f));

				if (dwRepairTax > 0)
				{
					lpCastle->AddTempTaxMoney(Castle::REPAIR_TAX, dwRepairTax);
					dwRepairGold += dwRepairTax;
				}
			}
		}

	    if (dwRepairGold <= dwCurrentGold) 
        {
	        dwCurrentGold -= dwRepairGold;
            lpEquipment->SetNumOrDurability(lpEquipment->GetMaxNumOrDurability());
	        return dwRepairGold;
        }
    }

    return 0;
}


bool CNPC::GetQuest(CCharacter* lpCharacter, unsigned short wQuestID, QuestNode** lppQuestNode)
{
	if (false == std::binary_search(m_Quests.begin(), m_Quests.end(), wQuestID))
	{
		ERRLOG3(g_Log, "CID:0x%08x NPC 0x%08x�� ������ ���� �ʴ� ����Ʈ %d�� ��û�Ͽ����ϴ�.", 
			lpCharacter->GetCID(), m_dwCID, wQuestID);
		return false;
	}

	QuestNode* lpQuestNode = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
	if (NULL == lpQuestNode)
	{
		ERRLOG2(g_Log, "NID:0x%08x ����Ʈ ��ũ��Ʈ�� �������� �ʴ� ����Ʈ�Դϴ�. QuestID:0x%04x", m_dwCID, wQuestID);
		return false;
	}

	unsigned short wLevel = lpCharacter->GetLevel(); //GetStatus().m_nLevel;
	if(lpQuestNode->m_wMinLevel != 0 && lpQuestNode->m_wMaxLevel != 0)
	{
		if (lpQuestNode->m_wMinLevel > wLevel || lpQuestNode->m_wMaxLevel < wLevel)
		{
			ERRLOG3(g_Log, "CID:0x%08x ������ ���� �ʴ� ����Ʈ�� ��û�Ͽ����ϴ�. QuestID:0x%04x, Level:%d", 
				m_dwCID, wQuestID, wLevel);
			return false;
		}
	}

	unsigned long wFame = lpCharacter->GetFame();
	if(lpQuestNode->m_dwMinFame != 0 && lpQuestNode->m_dwMaxFame != 0)
	{
		if (lpQuestNode->m_dwMinFame > wFame || lpQuestNode->m_dwMaxFame < wFame)
		{
			ERRLOG3(g_Log, "CID:0x%08x ���� ���� �ʴ� ����Ʈ�� ��û�Ͽ����ϴ�. QuestID:0x%04x, Fame:%d", 
				m_dwCID, wQuestID, wFame);
			return false;
		}
	}

	if (0 == (lpQuestNode->m_dwClass & (0x00000001 << (lpCharacter->GetClass() - 1))))
	{
		ERRLOG3(g_Log, "CID:0x%08x Ŭ������ ���� �ʴ� ����Ʈ�� ��û�Ͽ����ϴ�. QuestID:0x%04x, Class:%d", 
			m_dwCID, wQuestID, lpCharacter->GetClass());
		return false;
	}

	// TODO : ���� ����Ʈ ���� üũ

	*lppQuestNode = lpQuestNode;
	return true;
}

void CNPC::SetItemDropGrade(unsigned char cDropType, unsigned char cGradeF, unsigned char cGradeD, 
							unsigned char cGradeC, unsigned char cGradeB, unsigned char cGradeA)
{
	m_cGradeRate[cDropType][Item::EquipType::F_GRADE] = cGradeF;
	m_cGradeRate[cDropType][Item::EquipType::D_GRADE] = cGradeD;
	m_cGradeRate[cDropType][Item::EquipType::C_GRADE] = cGradeC;
	m_cGradeRate[cDropType][Item::EquipType::B_GRADE] = cGradeB;
	m_cGradeRate[cDropType][Item::EquipType::A_GRADE] = cGradeA;
}

void CNPC::SetItemDropBaseNum(unsigned char cDropType, unsigned char cGradeF, unsigned char cGradeD, 
							  unsigned char cGradeC, unsigned char cGradeB, unsigned char cGradeA)
{
	m_cBaseNum[cDropType][Item::EquipType::F_GRADE] = cGradeF;
	m_cBaseNum[cDropType][Item::EquipType::D_GRADE] = cGradeD;
	m_cBaseNum[cDropType][Item::EquipType::C_GRADE] = cGradeC;
	m_cBaseNum[cDropType][Item::EquipType::B_GRADE] = cGradeB;
	m_cBaseNum[cDropType][Item::EquipType::A_GRADE] = cGradeA;
}

void CNPC::SetRandomGrade(void)
{
	if (m_dwFlags & NPC::DISPLAY_EQUIP)
	{
		for (std::vector<GoodInfo>::iterator it = m_Goods.begin(); it != m_Goods.end(); ++it)
		{
			GoodInfo& goodInfo = *it;

			// ���� ���� �׷��̵嵵 ��ȿ�ϴ�.
			goodInfo.m_ePreGrade = goodInfo.m_eCurrentGrade;

			const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(goodInfo.m_wItemID);
			if (NULL == lpItemInfo) { continue; }

			Item::EquipType::OptionType optionType = Item::CItemType::GetInstance().GetOptionType(
				static_cast<Item::ItemType::Type>(lpItemInfo->m_DetailData.m_cItemType));

			const unsigned long dwPoint = Math::Random::ComplexRandom(
				std::accumulate(m_cGradeRate[optionType], m_cGradeRate[optionType] + Item::EquipType::MAX_GRADE, 0));
			unsigned long dwSpace = 0;

			for (unsigned char cIndex = 0; cIndex < Item::EquipType::MAX_GRADE; ++cIndex)
			{
				dwSpace += m_cGradeRate[optionType][cIndex];
				if (dwSpace > dwPoint)
				{
					goodInfo.m_eCurrentGrade = static_cast<Item::EquipType::Grade>(cIndex);
					break;
				}
			}
		}

		m_dwDisplayTime = GetTickCount();
	}
}

unsigned char CNPC::GetEquipShopInfo(unsigned char cRace, unsigned char cTabPage, 
									 unsigned short *aryItemID, unsigned char *aryItemGrade)
{
	unsigned char cNum = 0;

	unsigned long dwCurrentTime = GetTickCount();
	if (dwCurrentTime - m_dwDisplayTime > DISPLAY_CYCLE)
	{
		// �ֱ������� �Ǹ� ������ �׷��̵带 �缳�����ش�.
		SetRandomGrade();
	}

	for (std::vector<GoodInfo>::iterator it = m_Goods.begin(); it != m_Goods.end(); ++it)
	{
		GoodInfo& goodInfo = *it;
		if (goodInfo.m_cRace == cRace && goodInfo.m_cTabPage == cTabPage)
		{
			*aryItemID = goodInfo.m_wItemID;
			*aryItemGrade = goodInfo.m_eCurrentGrade;

			++aryItemID;
			++aryItemGrade;
			++cNum;
		}
	}

	return cNum;
}
