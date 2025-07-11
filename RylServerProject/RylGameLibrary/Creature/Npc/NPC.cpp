
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


// NPC 스크립트용 함수 목록
namespace NPC
{
    enum NPCFlag
    {
        BELONG_TO_CASTLE            = (1 << 0), // 성 소유 NPC인가?
        REPAIRABLE                  = (1 << 1), // 수리 가능한 NPC인가?
        BLACK_MARKETEER             = (1 << 2), // 암시장 상인인가?
        MUST_SELL_WITH_MILEAGE      = (1 << 3), // 마일리지로 판매하는 상인인가?
        MUST_SELL_WITH_SKILLCOUPON  = (1 << 4), // 스킬 쿠폰으로 판매하는 상인인가?
        DISPLAY_EQUIP               = (1 << 5), // 장비를 보여주는 상인인가?
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
		ERRLOG1(g_Log, "NID:0x%08x NPC등록에 실패했습니다.", nUID);
        delete lpNPC;
    }
}

void NPC::SetPosition(int nUID, float fDirection, float fPosX, float fPosY, float fPosZ)
{
    // NPC위치를 세팅한다.
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
    // Item을 세팅한다.
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
		sprintf(szErrMsg, "NPC에 등록할 Quest가 존재하지 않습니다. NPCID:0x%08x QuestID:0x%04x", nNPCID, nQuestID);
		MessageBox(NULL, szErrMsg, "Script Error", MB_OK);
	}
}

void NPC::SetDropGrade(int nUID, int nDropType, int nGradeF, int nGradeD, int nGradeC, int nGradeB, int nGradeA)
{
	// 각 그레이드별로 아이템이 등장할 확률을 설정한다.
	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(nUID);
	if (NULL != lpNPC) 
	{
		lpNPC->SetItemDropGrade(nDropType, nGradeF, nGradeD, nGradeC, nGradeB, nGradeA);
	}
}

void NPC::SetDropBase(int nUID, int nDropType, int nGradeF, int nGradeD, int nGradeC, int nGradeB, int nGradeA)
{
	// 각 그레이드별로 붙어나올 옵션의 갯수(최소값)를 설정한다.
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
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인
			case 13:    // 인간 잡상인
			case 14:    // 인간 무기 & 방어구상인
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
			case 21:    // 인간 대장장이
			case 27:	// 인간 메달상인 
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상
			case 31:    // 인간 모험가 트레이너
			case 32:    // 인간 전사 트레이너
			case 33:    // 인간 로그 트레이너
			case 34:    // 인간 마법사 트레이너
			case 35:    // 인간 성직자 트레이너    
			case 42:    // 인간 무구상인
		   
			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인
			case 63:    // 아칸 잡상인
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상
			case 71:    // 아칸 대장장이
			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상
			case 81:    // 아칸 비기너 트레이너
			case 82:    // 아칸 컴배턴트 트레이너
			case 83:    // 아칸 오피세이터 트레이너    
			case 92:    // 아칸 무구상인

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
		    
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// 인간 메달상인
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상

			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// 모험가 상위 트레이너
			case 124:	// 전사 상위 트레이너
			case 125:	// 로그 상위 트레이너
			case 126:	// 마법사 상위 트레이너
			case 127:	// 성직자 상위 트레이너

			case 128:	// 비기너 상위 트레이너
			case 129:	// 컴배턴트 상위 트레이너
			case 130:	// 오피세이터 상위 트레이너

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인

			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인

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
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인
			case 13:    // 인간 잡상인
			case 14:    // 인간 무기 & 방어구상인
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
			case 21:    // 인간 대장장이
			case 27:	// 인간 메달상인 
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상
			case 31:    // 인간 모험가 트레이너
			case 32:    // 인간 전사 트레이너
			case 33:    // 인간 로그 트레이너
			case 34:    // 인간 마법사 트레이너
			case 35:    // 인간 성직자 트레이너    
			case 42:    // 인간 무구상인
		   
			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인
			case 63:    // 아칸 잡상인
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상
			case 71:    // 아칸 대장장이
			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상
			case 81:    // 아칸 비기너 트레이너
			case 82:    // 아칸 컴배턴트 트레이너
			case 83:    // 아칸 오피세이터 트레이너    
			case 92:    // 아칸 무구상인

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
		    
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// 인간 메달상인
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상

			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// 모험가 상위 트레이너
			case 124:	// 전사 상위 트레이너
			case 125:	// 로그 상위 트레이너
			case 126:	// 마법사 상위 트레이너
			case 127:	// 성직자 상위 트레이너

			case 128:	// 비기너 상위 트레이너
			case 129:	// 컴배턴트 상위 트레이너
			case 130:	// 오피세이터 상위 트레이너

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인

			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인

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
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인
			case 13:    // 인간 잡상인
			case 14:    // 인간 무기 & 방어구상인
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
			case 21:    // 인간 대장장이
			case 27:	// 인간 메달상인 
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상
			case 31:    // 인간 모험가 트레이너
			case 32:    // 인간 전사 트레이너
			case 33:    // 인간 로그 트레이너
			case 34:    // 인간 마법사 트레이너
			case 35:    // 인간 성직자 트레이너    
			case 42:    // 인간 무구상인
		   
			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인
			case 63:    // 아칸 잡상인
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상
			case 71:    // 아칸 대장장이
			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상
			case 81:    // 아칸 비기너 트레이너
			case 82:    // 아칸 컴배턴트 트레이너
			case 83:    // 아칸 오피세이터 트레이너    
			case 92:    // 아칸 무구상인

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
		    
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// 인간 메달상인
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상

			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// 모험가 상위 트레이너
			case 124:	// 전사 상위 트레이너
			case 125:	// 로그 상위 트레이너
			case 126:	// 마법사 상위 트레이너
			case 127:	// 성직자 상위 트레이너

			case 128:	// 비기너 상위 트레이너
			case 129:	// 컴배턴트 상위 트레이너
			case 130:	// 오피세이터 상위 트레이너

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인

			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인

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
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인
			case 13:    // 인간 잡상인
			case 14:    // 인간 무기 & 방어구상인
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
			case 21:    // 인간 대장장이
			case 27:	// 인간 메달상인 
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상
			case 31:    // 인간 모험가 트레이너
			case 32:    // 인간 전사 트레이너
			case 33:    // 인간 로그 트레이너
			case 34:    // 인간 마법사 트레이너
			case 35:    // 인간 성직자 트레이너    
			case 42:    // 인간 무구상인
		   
			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인
			case 63:    // 아칸 잡상인
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상
			case 71:    // 아칸 대장장이
			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상
			case 81:    // 아칸 비기너 트레이너
			case 82:    // 아칸 컴배턴트 트레이너
			case 83:    // 아칸 오피세이터 트레이너    
			case 92:    // 아칸 무구상인

				dwFlags |= NPC::REPAIRABLE;
				break;
		}

		switch (nPopupFunction)
		{
			case 16:    // 인간 암상 - 무기상
			case 17:    // 인간 암상 - 방어구상
			case 18:    // 인간 암상 - 잡 + 스킬상 
		    
			case 66:    // 아칸 암상 - 무기상
			case 67:    // 아칸 암상 - 방어구상
			case 68:    // 아칸 암상 - 잡 + 스킬상

				dwFlags |= NPC::BLACK_MARKETEER;
				break;
		}

		switch (nPopupFunction)
		{
			case 27:	// 인간 메달상인
			case 28:	// 인간 메달상인 무기상
			case 29:	// 인간 메달상인 방어구상

			case 77:	// 아칸 메달상인 
			case 78:	// 아칸 메달상인 무기상
			case 79:	// 아칸 메달상인 방어구상

				dwFlags |= NPC::MUST_SELL_WITH_MILEAGE;
				break;
		}

		switch (nPopupFunction)
		{
			case 123:	// 모험가 상위 트레이너
			case 124:	// 전사 상위 트레이너
			case 125:	// 로그 상위 트레이너
			case 126:	// 마법사 상위 트레이너
			case 127:	// 성직자 상위 트레이너

			case 128:	// 비기너 상위 트레이너
			case 129:	// 컴배턴트 상위 트레이너
			case 130:	// 오피세이터 상위 트레이너

                dwFlags |= NPC::MUST_SELL_WITH_SKILLCOUPON;				
				break;
		}

		switch (nPopupFunction)
		{
			case 11:    // 인간 무기상인
			case 12:    // 인간 방어구상인

			case 61:    // 아칸 무기상인
			case 62:    // 아칸 방어구상인

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

	// 성에 속한 NPC 라면, 거래 세율 적용
	if (IsBelongToCastle())
	{
		lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByNameID(m_dwTownOrNameID);
		if (lpCastle)
		{
			fTax = lpCastle->GetTax(Castle::TRADE_TAX) / 100.f;
		}
	}

	enum {
		HACK_BLACK_RANDOM = 0,		// 암시장 오류 : 랜덤옵션 장비를 구입하게 했습니다
		HACK_BLACK_NOTEXITSITEM,	// 암시장 오류 : 존재하지 않는 아이템입니다
        HACK_MILEAGE_RANDOM,		// 메달상점 오류 : 랜덤옵션 장비를 구입하게 했습니다
		HACK_MILEAGE_NOTEXITSITEM,	// 메달상점 오류 : 존재하지 않는 아이템입니다
		HACK_SHOP_NOTEXITSITEM,		// 장비상 오류 : 취급하지 않는 아이템을 구매합니다.
		HACK_SHOP_NOEQUIP,			// 장비상 오류 : 장비가 아닌 아이템을 구매하려 합니다
		HACK_SHOP_NOTGRADEITEM,		// 장비상 오류 : 판매하지 않는 그레이드의 아이템을 요청하였습니다
		HACK_SHOP_NOFGRADEITEM,		// 장비상 오류 : F그레이드가 아닌 아이템을 요청하였습니다
		HACK_SHOP_SOCKETITEM,		// 장비상 오류 : 구매할 수 없는 소켓이 있는 아이템을 요청하였습니다
		HACK_SHOP_OVERDURABILITY,	// 장비상 오류 : 내구도가 180 이상인 아이템이 생성되었습니다.
	};

	
	// ---------------------------------------------------------------------------------------------
	// 암시장
    if (m_dwFlags & NPC::BLACK_MARKETEER)
	{
		// edith 2009.07.20 암시장에 랜덤옵션 장비를 사게 패킷을 보냈다.
		if(lpRequestItem)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_BLACK_RANDOM, wKindItem, lpRequestItem, usError);
			ERRLOG1(g_Log, "암시장 오류 : 랜덤옵션 장비를 구입하게 했습니다. ItemID:%d", wKindItem);
			return NULL;
		}

		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
		if (NULL == lpItemInfo)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_BLACK_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "암시장 오류 : 존재하지 않는 아이템입니다. ItemID:%d", wKindItem);
			return NULL;
		}

		dwPrice = lpItemInfo->m_DetailData.m_dwBlackPrice * cNum;
		dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
		dwPrice += dwTaxIncome;

        if (dwPrice > dwCurrentGold)
		{
			ERRLOG2(g_Log, "암시장 오류 : 돈이 부족합니다. 가격:%d, 소지금:%d", dwPrice, dwCurrentGold);
			return NULL;
		}

		Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(*lpItemInfo);
		if (NULL == lpItem)
		{
			ERRLOG0(g_Log, "암시장 오류 : 아이템 생성에 실패하였습니다.");
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
					ERRLOG0(g_Log, "암시장 오류 : 옵션 설정에 실패하였습니다.");
					return NULL;
				}

				int iMagicChancePoint = 0;
				if(lpCustomer->GetMagicChancePoint() > 0)
					iMagicChancePoint = (int)(lpCustomer->GetMagicChancePoint()/2);

				// edith 2009.08.18 블랙마켓이 럭찬의 50프로로 럭찬템 드랍
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

		ERRLOG0(g_Log, "암시장 오류 : 아이템 그레이드 설정에 실패하였습니다.");
		return NULL;
	}

	// ---------------------------------------------------------------------------------------------
	// 메달상점
	if (m_dwFlags & NPC::MUST_SELL_WITH_MILEAGE)
	{
		// edith 2009.07.20 암시장에 랜덤옵션 장비를 사게 패킷을 보냈다.
		if(lpRequestItem)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_MILEAGE_RANDOM, wKindItem, lpRequestItem, usError);
			ERRLOG1(g_Log, "메달상점 오류 : 랜덤옵션 장비를 구입하게 했습니다. ItemID:%d", wKindItem);
			return NULL;
		}

		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
		if (NULL == lpItemInfo)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_MILEAGE_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "메달상점 오류 : 존재하지 않는 아이템입니다. ItemID:%d", wKindItem);
			return NULL;
		}

		dwPrice = lpItemInfo->m_DetailData.m_dwMedalPrice * cNum;
		if (dwPrice > dwCurrentMileage)
		{
			ERRLOG6(g_Log, "NID:0x%08x 공헌메달이 모자랍니다. 현재 공헌메달은 %d입니다. "
				"(종류ID:%d, 1개 가격:%d, 개수:%d, 전체 금액:%d)",
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
								ERRLOG0(g_Log, "메달상점 오류 : 옵션 설정에 실패하였습니다.");
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
				ERRLOG1(g_Log, "NID:0x%08x 아이템 생성에 실패했습니다", m_dwCID);
			}

			return lpItem;
		}
		else
		{
			ERRLOG4(g_Log, "NID:0x%08x 갯수가 이상합니다. bStackable:%d, cNum:%d, MaxNumOrDurability:%d",
				m_dwCID, bStackable, cNum, lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack);
		}

		ERRLOG0(g_Log, "메달상점 오류 : 아이템 그레이드 설정에 실패하였습니다.");
		return NULL;
	}

	// ---------------------------------------------------------------------------------------------
	// 장비상

	if (NULL != lpRequestItem)//normal shop sell hz check one of the lpitem functions to fix no stat
	{
		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
		if (NULL == lpItemInfo)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_BLACK_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "암시장 오류 : 존재하지 않는 아이템입니다. ItemID:%d", wKindItem);
			return NULL;
		}

		dwPrice = lpRequestItem->GetBuyPrice();
		float fd = lpCustomer->GetAbilityValue(Skill::Type::AB_SHOP_MEMBERSHIP);
		dwPrice -= static_cast<unsigned long>((float)dwPrice*fd/100.0f);

		dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
		dwPrice += dwTaxIncome;

        if (dwPrice > dwCurrentGold)
		{
			ERRLOG2(g_Log, "암시장 오류 : 돈이 부족합니다. 가격:%d, 소지금:%d", dwPrice, dwCurrentGold);
			return NULL;
		}
		
		Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(*lpItemInfo);
		if (NULL == lpItem)
		{
			ERRLOG0(g_Log, "암시장 오류 : 아이템 생성에 실패하였습니다.");
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
					ERRLOG0(g_Log, "암시장 오류 : 옵션 설정에 실패하였습니다.");
					return NULL;
				}

				int iMagicChancePoint = 0;
				if(lpCustomer->GetMagicChancePoint() > 0)
					iMagicChancePoint = (int)(lpCustomer->GetMagicChancePoint()/2);

				// edith 2009.08.18 블랙마켓이 럭찬의 50프로로 럭찬템 드랍
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

		ERRLOG0(g_Log, "암시장 오류 : 아이템 그레이드 설정에 실패하였습니다.");
		return NULL;
		/*CFindGood findGood(lpRequestItem->GetPrototypeID());
		std::vector<GoodInfo>::iterator it = std::find_if(m_Goods.begin(), m_Goods.end(), findGood);

		if (it == m_Goods.end())
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOTEXITSITEM, wKindItem, NULL, usError);
			ERRLOG2(g_Log, "NID:0x%08x 장비상 오류 : 이 NPC는 %d종류의 아이템을 취급하지 않습니다.", 
				m_dwCID, lpRequestItem->GetPrototypeID());

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpRequestItem);
		if (NULL == lpEquip)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOEQUIP, wKindItem, NULL, usError);
			ERRLOG1(g_Log, "장비상 오류 : 장비가 아닌 아이템을 구매하려 합니다. 아이템ID:%d", 
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
			ERRLOG2(g_Log, "NID:0x%08x 장비상 오류 : 판매하지 않는 그레이드의 아이템을 요청하였습니다. "
				"요청 아이템의 그레이드 - %d", m_dwCID, gradeInfo.m_eItemGrade);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		// edith 2009.07.24 만약 장비가 F그레이드가 아니면 구매가 불가능 Disconnect 시킨다.
		if(Item::EquipType::F_GRADE != gradeInfo.m_eItemGrade)
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_NOFGRADEITEM, wKindItem, lpEquip, usError);

			ERRLOG2(g_Log, "NID:0x%08x 장비상 오류 : F그레이드가 아닌 아이템을 요청하였습니다. "
				"요청 아이템의 그레이드 - %d", m_dwCID, gradeInfo.m_eItemGrade);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		// edith 2009.07.24 만약 장비에 소켓이 붙어있음 구매가 불가능. Disconnect 시킨다.
		if( 0 > lpEquip->GetMaxSocketNum() )
		{
			usError = 1;
			GAMELOG::LogHackBoubt(*lpCustomer, HACK_SHOP_SOCKETITEM, wKindItem, lpEquip, usError);

			ERRLOG2(g_Log, "NID:0x%08x 장비상 오류 : 구매할 수 없는 소켓이 있는 아이템을 요청하였습니다. "
				"요청 아이템의 그레이드 - %d", m_dwCID, gradeInfo.m_eItemGrade);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}
		lpEquip->SetSocketNum(2);
		// edith 2009.07.24 만약 장비의 내구도가 180이상이면 핵 의심으로 로그를 남긴다.
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
			ERRLOG2(g_Log, "장비상 오류 : 돈이 부족합니다. 가격:%d, 소지금:%d", dwPrice, dwCurrentGold);

			DELETE_ITEM(lpRequestItem);
			return NULL;
		}

		if (dwTaxIncome > 0)
		{
			lpCastle->AddTempTaxMoney(Castle::TRADE_TAX, dwTaxIncome);
		}

		// 상점에서 파는 아이템은 내구도가 풀인 상품이어야 한다.
		lpRequestItem->SetNumOrDurability(lpRequestItem->GetMaxNumOrDurability());

		return lpRequestItem;*/
	}



	// ---------------------------------------------------------------------------------------------
	// 일반 상점

	CFindGood findGood(wKindItem);
	std::vector<GoodInfo>::iterator it = std::find_if(m_Goods.begin(), m_Goods.end(), findGood);

	if (it != m_Goods.end())
	{
        const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wKindItem);
        if (NULL != lpItemInfo)
        {
			// 쿠폰(3,4단계 스킬북)으로 구매
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
					ERRLOG3(g_Log, "NID:0x%08x 알맞은 쿠폰이 아닙니다. 쿠폰ID:%d, 구입물품ID:%d", 
						m_dwCID, wCouponID, wKindItem);
					return NULL;
				}
			}
			// 돈으로 구매
			else
			{
				dwPrice = lpItemInfo->m_DetailData.m_dwPrice * cNum;

				float fd = lpCustomer->GetAbilityValue(Skill::Type::AB_SHOP_MEMBERSHIP);
				dwPrice -= static_cast<unsigned long>((float)dwPrice*fd/100.0f);

				dwTaxIncome = static_cast<unsigned long>(dwPrice * fTax);
				dwPrice += dwTaxIncome;

				if (dwPrice > dwCurrentGold)
				{
					ERRLOG6(g_Log, "NID:0x%08x 돈이 모자랍니다. 현재 소지금은 %d원입니다. "
						"(종류ID:%d, 1개 가격:%d, 개수:%d, 전체 금액:%d)",
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
                    ERRLOG1(g_Log, "NID:0x%08x 아이템 생성에 실패했습니다", m_dwCID);
                }
				
				return lpItem;
			}
            else
            {
                ERRLOG4(g_Log, "NID:0x%08x 갯수가 이상합니다. bStackable:%d, cNum:%d, MaxNumOrDurability:%d",
                    m_dwCID, bStackable, cNum, lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack);
			}
        }
        else
        {
            ERRLOG2(g_Log, "NID:0x%08x 이 NPC는 아이템 스크립트에 없는 %d종류의 아이템을 팔려고 하였습니다.",
                m_dwCID, wKindItem);
        }
    }
    else
    {
        ERRLOG2(g_Log, "NID:0x%08x 이 NPC는 %d종류의 아이템을 취급하지 않습니다.", m_dwCID, wKindItem);
    }

	return NULL;
}


unsigned long CNPC::RepairItem(Item::CEquipment* lpEquipment, unsigned long& dwCurrentGold) 
{
    if (NULL != lpEquipment && (m_dwFlags & NPC::REPAIRABLE))
    {
	    unsigned long dwRepairGold = lpEquipment->GetRepairPrice();

		// 성에 속한 NPC 라면 수리 세율 적용
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
		ERRLOG3(g_Log, "CID:0x%08x NPC 0x%08x가 가지고 있지 않는 퀘스트 %d를 요청하였습니다.", 
			lpCharacter->GetCID(), m_dwCID, wQuestID);
		return false;
	}

	QuestNode* lpQuestNode = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
	if (NULL == lpQuestNode)
	{
		ERRLOG2(g_Log, "NID:0x%08x 퀘스트 스크립트에 존재하지 않는 퀘스트입니다. QuestID:0x%04x", m_dwCID, wQuestID);
		return false;
	}

	unsigned short wLevel = lpCharacter->GetLevel(); //GetStatus().m_nLevel;
	if(lpQuestNode->m_wMinLevel != 0 && lpQuestNode->m_wMaxLevel != 0)
	{
		if (lpQuestNode->m_wMinLevel > wLevel || lpQuestNode->m_wMaxLevel < wLevel)
		{
			ERRLOG3(g_Log, "CID:0x%08x 레벨에 맞지 않는 퀘스트를 요청하였습니다. QuestID:0x%04x, Level:%d", 
				m_dwCID, wQuestID, wLevel);
			return false;
		}
	}

	unsigned long wFame = lpCharacter->GetFame();
	if(lpQuestNode->m_dwMinFame != 0 && lpQuestNode->m_dwMaxFame != 0)
	{
		if (lpQuestNode->m_dwMinFame > wFame || lpQuestNode->m_dwMaxFame < wFame)
		{
			ERRLOG3(g_Log, "CID:0x%08x 명성에 맞지 않는 퀘스트를 요청하였습니다. QuestID:0x%04x, Fame:%d", 
				m_dwCID, wQuestID, wFame);
			return false;
		}
	}

	if (0 == (lpQuestNode->m_dwClass & (0x00000001 << (lpCharacter->GetClass() - 1))))
	{
		ERRLOG3(g_Log, "CID:0x%08x 클래스에 맞지 않는 퀘스트를 요청하였습니다. QuestID:0x%04x, Class:%d", 
			m_dwCID, wQuestID, lpCharacter->GetClass());
		return false;
	}

	// TODO : 선행 퀘스트 여부 체크

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

			// 지난 턴의 그레이드도 유효하다.
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
		// 주기적으로 판매 장비들의 그레이드를 재설정해준다.
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
