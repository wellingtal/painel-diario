
#include "stdafx.h"

#include "BroadcastProcess.h"
#include "../GameHandler.h"

#include <Item/ItemMgr.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Creature/EnemyCheck.h>

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"

#include "SkillProcess.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharBroadcastPacket.h>
#include "GMMemory.h"

Broadcast2nd::CClientCellMgr    g_ClientCellMgr;

const bool g_bLogCharBroadcast = true;


class CBroadcastCharDataProcess
{
public:

    enum CELL_OPER_TYPE
    {
        CELL_LOGIN,
        CELL_LOGOUT,
        CELL_BROADCAST
    };

    typedef std::set<unsigned long> GIDList;
    
    CBroadcastCharDataProcess(GIDList& lstGID);

    bool isValid() { return 0 != m_lpGuildPannel; }
    CRYLGuildInfoPanel* GetGuildInfoPannel() { return m_lpGuildPannel; }

    void operator() (Broadcast2nd::CCharacterData& charData);
    void operator() (Broadcast2nd::CMonsterData& monData);

    void RequestGuildInfo(unsigned long dwGID, bool bForceRequest = false);
    void SetChantEffectToCreature(RYLCreature* lpRYLCreature, const EnchantInfo& enchantInfo);

    static bool ProcessBroadcastLogin(Broadcast2nd::PktBroadcast* lpBroadcast, unsigned short usDataLen);
    static bool ProcessBroadcastDelta(Broadcast2nd::PktBroadcast* lpBroadcast, unsigned short usDataLen);
    static bool ProcessRemoveCharacterByPulse(unsigned long dwCurrentPulse);

    static void LogCharCell(Broadcast2nd::CCharacterData& charData, 
        CELL_OPER_TYPE eOperationType, const char* szExtData = 0);

private:

    GIDList&            m_lstGID;    
    CRYLGuildInfoPanel* m_lpGuildPannel;
};

struct CLogCharUpdateData
{
    void operator() (Broadcast2nd::CCharacterData& charData)
    {
        if (charData.IsSetDataFlag(Broadcast2nd::CHARDATA_CHANGED)) 
        {
            CBroadcastCharDataProcess::LogCharCell(charData, 
                CBroadcastCharDataProcess::CELL_LOGIN); 
        } 
    }
};

struct CLogCharCellLogin
{
    void operator() (Broadcast2nd::CCharacterData& charData)
    { 
        if (charData.IsSetDataFlag(Broadcast2nd::CHARDATA_CHANGED)) 
        {
            CBroadcastCharDataProcess::LogCharCell(charData, 
                CBroadcastCharDataProcess::CELL_LOGOUT); 
        }
    }
};


GAME_EVENT_FUNC(ParseBroadcast2nd);

bool GameProcess::RegisterBroadcastHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;

    dwErrorCount += GameEventHandler.AddHandler(CmdCellBroadCast2nd, ParseBroadcast2nd) ? 0 : 1;

    return (0 == dwErrorCount);
}

GAME_EVENT_FUNC(ParseBroadcast2nd)
{
    Broadcast2nd::PktBroadcast* lpBroadcast = 
        reinterpret_cast<Broadcast2nd::PktBroadcast*>(lpPktBase);

    int nDataLen = lpBroadcast->GetLen() - sizeof(Broadcast2nd::PktBroadcast);
    if (0 <= nDataLen)
    {
        unsigned short usDataLen = static_cast<unsigned short>(nDataLen);

        switch(lpBroadcast->m_cBroadcastType)
        {
        case Broadcast2nd::PktBroadcast::LOGIN:             
            CBroadcastCharDataProcess::ProcessBroadcastLogin(lpBroadcast, usDataLen);
            break;

        case Broadcast2nd::PktBroadcast::BROADCAST:         
            CBroadcastCharDataProcess::ProcessBroadcastDelta(lpBroadcast, usDataLen);
            break;

        case Broadcast2nd::PktBroadcast::BROADCAST_END:
            CBroadcastCharDataProcess::ProcessBroadcastDelta(lpBroadcast, usDataLen);
            CBroadcastCharDataProcess::ProcessRemoveCharacterByPulse(lpBroadcast->m_dwCurrentPulse);
            break;
        }
    }

    return true;
}


CBroadcastCharDataProcess::CBroadcastCharDataProcess(GIDList& lstGID) 
:   m_lstGID(lstGID)
{ 
    CRYLGameScene* lpScene = 
        static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene(GAME_SCENE));

    CRYLGuildFrameDlg* lpDlg = 
        static_cast<CRYLGuildFrameDlg*>(lpScene->GetGuildFrameDlg());

    m_lpGuildPannel = static_cast<CRYLGuildInfoPanel*>(lpDlg->GetGuildInfoPanel());
}


void CBroadcastCharDataProcess::operator() (Broadcast2nd::CCharacterData& charData)
{       
    // �����Ͱ� ����Ǿ���, �� ĳ���Ͱ� �ƴ� ��쿡�� ȣ��
    if (charData.IsSetDataFlag(Broadcast2nd::CHARDATA_CHANGED) &&
        charData.GetCID() != CRYLNetworkData::Instance()->m_dwMyChrID )
    {
        charData.ResetDataFlag(Broadcast2nd::CHARDATA_CHANGED);
        
        // Hide ��ų�� ����� ���ɴ�.
        EnchantInfo enchantInfo = charData.GetEnchantInfo();
        bool bHide = enchantInfo.GetFlag( Skill::SpellID::Hide );

        const CNetworkPos& netPos = charData.GetNetworkPos();

        vector3 vecPos;
        vecPos.x = netPos.GetXPos() * 100.0f;
        vecPos.y = netPos.GetYPos() * 100.0f + 15.0f;
        vecPos.z = netPos.GetZPos() * 100.0f;

        RYLCreature* lpTempCreature = 
            RYLCreatureManager::Instance()->GetCreature(charData.GetCID());

        if (0 == lpTempCreature)
        {
            // ũ���İ� ����. �ϴ� ������ �����.
            RYLCreatureManager::Instance()->AddCreature(charData.GetCID(), vecPos);
            lpTempCreature = RYLCreatureManager::Instance()->GetCreature(charData.GetCID());

            if (0 != lpTempCreature && !bHide)
            {
                //  ������ �̸��� �ִ´�.
                CharDataNode* lpTempData = 
                    RYLCharacterDataManager::Instance()->GetCharData( lpTempCreature->GetCharID() );

                if ( lpTempData )
                {
                    lpTempData->m_strStreetStallName = charData.GetStallName();
                }
            }

            // Hide ���°� �ƴ� ĳ���ʹ� m_bHide �� false �� ����
            CZ3DGeneralChrModel* lpTempModel = lpTempCreature->GetCharModel();

            if (0 != lpTempModel && !lpTempCreature->GetRideEnable())
            {
                CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
                    CSceneManager::GetCharacterDataNode(lpTempModel);

                CheckNode.m_bHide = bHide;

                CSceneManager::SetCharacterDataNode(CheckNode);
            }
        }
        else
        {
            // Hide ���°� �ƴ� ĳ���ʹ� m_bHide �� false �� ����
            CZ3DGeneralChrModel* lpTempModel = lpTempCreature->GetCharModel();

            if (0 != lpTempModel && !lpTempCreature->GetRideEnable())
            {
                CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
                    CSceneManager::GetCharacterDataNode(lpTempModel);

                CheckNode.m_bHide = bHide;

                CSceneManager::SetCharacterDataNode(CheckNode);
            }
        }

		if(RYLCreatureManager::Instance()->IsArmsCreature(charData.GetCID()))
		{
			if(lpTempCreature)
			{
				lpTempCreature->SetRideEnable(true);
			}			
		}

		lpTempCreature->SetObjectType(0);

        CharDataNode* lpTempData = 
            RYLCharacterDataManager::Instance()->GetCharData(charData.GetCID());

        if (0 != lpTempCreature && 0 != lpTempData)  
        {
            // ĳ���� �����͸� �����Ѵ�.
			lpTempData->m_strName = charData.GetName();

            lpTempData->m_bPeaceMode = charData.IsPeaceMode();
            lpTempData->m_cNation    = charData.GetNation();
            lpTempData->m_cRace      = charData.GetRace();
            lpTempData->m_cSex       = charData.GetSex();
            lpTempData->m_usClass    = charData.GetClassType();
            lpTempData->m_cLevel     = charData.GetLevel();
            lpTempData->m_ulFame     = charData.GetFame();
            lpTempData->m_ulMileage  = charData.GetMileage();
            lpTempData->m_lCurrHP    = charData.GetHP();

            lpTempCreature->SetWeaponPos(charData.GetHandPos());
			lpTempCreature->SetRideFlag(charData.GetRideFlag());
			           

            // ĳ���� ������ �����Ѵ�.
            const int MAX_EQUIPMENT_NAME = 256;
			const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];
            unsigned short wEquipID[Item::EquipmentPos::MAX_EQUPMENT_POS];
            charData.GetEquipmentsID(wEquipID, Item::EquipmentPos::MAX_EQUPMENT_POS);

			// GM �� üũ. //

			unsigned char cGMFlag = 0;

			if (wEquipID[0]==Item::GM_MODEL_ITEM)
			{
				cGMFlag = 1;
			}

			if (!cGMFlag)
			{
				for( int nIndex = 0; nIndex < Item::EquipmentPos::MAX_EQUPMENT_POS; ++nIndex )
				{
					strShape[ nIndex ] = g_ItemMgr.GetEquipName( wEquipID[ nIndex ] );
				}
			}
			else
			{
				std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));
			}
            
			char strSex[MAX_PATH];

            if ( !lpTempCreature->GetIsUsed() )
            {
                lpTempCreature->SetIsUsed( true );

				SetChantEffectToCreature(lpTempCreature, charData.GetEnchantInfo());

				if (!cGMFlag)
				{
					switch( lpTempData->m_cRace )
					{
					case CClass::RaceType::HUMAN:
						strcpy( strSex, ( lpTempData->m_cSex == 1 ) ? "PC_MAN.GCMDS" : "PC_WOMAN.GCMDS" );						
						break;

					case CClass::RaceType::AKHAN:                        
						strcpy( strSex, ( lpTempData->m_cSex == 1 ) ? "PC_Akhan_A.GCMDS" : "PC_Akhan_B.GCMDS" );													
						break;
					}

					char pTempStyle[ MAX_EQUIPMENT_NAME ];

					sprintf(pTempStyle, "FACE%02d", charData.GetFace());
					lpTempCreature->SetFaceType(pTempStyle);

					sprintf(pTempStyle, "HAIR%02d", charData.GetHair());
					lpTempCreature->SetHairType( pTempStyle );					
				}
				else
				{
					strcpy( strSex, "PC_GM.GCMDS");					
				}

				lpTempCreature->SetModelFileName(strSex);

				CZ3DGeneralChrModel* lpTempModel = RYLCreatureManager::Instance()->AllocModel(lpTempCreature);
					
                if ( lpTempModel )
                {
					// hide�� ����
					CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
									CSceneManager::GetCharacterDataNode(lpTempModel);
					CheckNode.m_bHide = bHide;
					CSceneManager::SetCharacterDataNode(CheckNode);

                    lpTempModel->SetPosition(vecPos);
                    lpTempModel->SetDirection(lpTempCreature->GetDirection());
                }

                lpTempCreature->SetShape(strShape, charData.GetNetworkEquipGrade(), true);

				lpTempCreature->SetGuildWarFlag(charData.GetGuildWarFlag());
				lpTempCreature->SetRealmWarFlag(charData.GetRealmWarFlag());
				lpTempCreature->SetRealmPoint(charData.GetRealmPoint());
				lpTempCreature->SetTactics(charData.GetTacticsFlag());
            } 
            else
            {
				if (cGMFlag != lpTempCreature->GetGMModelFlag())
				{					
					SetChantEffectToCreature(lpTempCreature, charData.GetEnchantInfo());

					CZ3DGeneralChrModel* pCharModel = lpTempCreature->GetCharModel();
					if (pCharModel)
					{
						const vector3 vPos = *(lpTempCreature->GetPosition());

						RYLCreatureManager::Instance()->FreeModel(pCharModel);
						lpTempCreature->SetCharModel( NULL );
						
						if (cGMFlag)
						{
							// PC ���� GM ����.																																
							strcpy(strSex, "PC_GM.GCMDS");
						}
						else
						{
							// GM ���� PC ��							
							switch (lpTempData->m_cRace)
							{
							case CClass::RaceType::HUMAN :
								{
									strcpy( strSex, ( lpTempData->m_cSex == 1 ) ? "PC_MAN.GCMDS" : "PC_WOMAN.GCMDS" ) ;

									break;
								}

							case CClass::RaceType::AKHAN :
								{
									strcpy( strSex, ( lpTempData->m_cSex == 1 ) ? "PC_Akhan_A.GCMDS" : "PC_Akhan_B.GCMDS" ) ; 

									break;
								}
							}							
						}

						lpTempCreature->SetModelFileName(strSex);

						lpTempCreature->SetGMModelFlag(cGMFlag);

						pCharModel = RYLCreatureManager::Instance()->AllocModel(lpTempCreature, true);

						if (pCharModel)
						{
							// hide�� ����
							CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
											CSceneManager::GetCharacterDataNode(pCharModel);
							CheckNode.m_bHide = bHide;
							CSceneManager::SetCharacterDataNode(CheckNode);

							pCharModel->SetPosition(vPos);
							pCharModel->SetDirection(lpTempCreature->GetDirection());																								
						}

						// ������ ����Ʈ ���.
						CEffScript *pLevelUpEff = new CEffScript;
						CSceneManager::AddEffectScript( pLevelUpEff );

						pLevelUpEff->GetScriptBinData( "respawn.esf", CRYLGameData::Instance()->m_bSelfEffect );
						pLevelUpEff->SetStartPos( vPos.x, vPos.y, vPos.z );			

						pLevelUpEff->SetEndPos( (D3DXVECTOR3*)lpTempCreature->GetPosition(), 1 );

						lpTempCreature->SetShape(strShape, charData.GetNetworkEquipGrade(), true);

						lpTempCreature->SetGuildWarFlag(charData.GetGuildWarFlag());
						lpTempCreature->SetRealmWarFlag(charData.GetRealmWarFlag());
					}					
				}
				else
				{
					bool bChangeHideInfo = false;
					if (enchantInfo.GetFlag(Skill::SpellID::Hide) != lpTempCreature->GetEffect()->GetChantStatus(Skill::SpellID::Hide) || 
						enchantInfo.GetFlag(Skill::SpellID::Stealth) != lpTempCreature->GetEffect()->GetChantStatus(Skill::SpellID::Stealth))
					{
						bChangeHideInfo = true;
					}

					// ��æƮ ������ ���� ���θ� üũ�� �� �������ش�.
					SetChantEffectToCreature(lpTempCreature, charData.GetEnchantInfo());

					lpTempCreature->SetShape(strShape, charData.GetNetworkEquipGrade(), bChangeHideInfo);

					lpTempCreature->SetGuildWarFlag(charData.GetGuildWarFlag());
					lpTempCreature->SetRealmWarFlag(charData.GetRealmWarFlag());
					lpTempCreature->SetRealmPoint(charData.GetRealmPoint());
					lpTempCreature->SetTactics(charData.GetTacticsFlag());
				}
            }


            lpTempCreature->SetPosition(vecPos);

            // Ŀ�´�Ƽ ���� ����
            lpTempData->m_ulPartyID	= charData.GetPartyID();
            lpTempData->m_ulGuildID	= charData.GetGuildID();			

            // ��� ������ ��û�Ѵ�.
            RequestGuildInfo(lpTempData->m_ulGuildID);
            RequestGuildInfo(lpTempData->m_ulTempGID);
        }
    }
    else if (charData.IsSetDataFlag(Broadcast2nd::CHARDATA_CHANGED) &&
        charData.GetCID() == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		// ����Ÿ�� ����Ǿ��� ���ڽ��ϰ�� ������ ���� ��ȭ���𿡼��� �����Ѵ�.
        CharDataNode* lpTempData = 
            RYLCharacterDataManager::Instance()->GetCharData(charData.GetCID());

		if(lpTempData)
		{
            lpTempData->m_bPeaceMode = charData.IsPeaceMode();
		}
	}
}


void CBroadcastCharDataProcess::operator() (Broadcast2nd::CMonsterData& monData)
{
	// ���� ������ ���, �ʿ� �����͸� �����Ѵ�.
	unsigned long dwMonsterID = monData.GetCID();

	const CMonsterMgr::MonsterProtoType *lpMonster = 
        g_MonsterMgr.GetMonsterProtoType(dwMonsterID & 0x0000FFFF);

	if (0 != lpMonster && 
        monData.IsSetDataFlag(Broadcast2nd::CHARDATA_CHANGED))
	{
        monData.ResetDataFlag(Broadcast2nd::CHARDATA_CHANGED);

        const CNetworkPos& netPos = monData.GetNetworkPos();

        vector3 vecPos;
		vecPos.x = netPos.GetXPos() * 100.0f;
		vecPos.y = netPos.GetYPos() * 100.0f + 15.0f;
		vecPos.z = netPos.GetZPos() * 100.0f;

		RYLCreature*  lpMonsterCreature = RYLCreatureManager::Instance()->GetCreature(dwMonsterID);
		CharDataNode* lpMonsterData = RYLCharacterDataManager::Instance()->GetCharData(dwMonsterID);

        if (0 == lpMonsterCreature)
        {
            // ���Ͱ� ��� ���� �����Ѵ�.
			RYLCreatureManager::Instance()->AddCreature( dwMonsterID, 
                lpMonster->m_MonsterInfo.m_strName, 
				lpMonster->m_MonsterInfo.m_strModelingFlag, 
                vecPos, 0.0f, 
                lpMonster->m_MonsterInfo.m_bCollision, 
                lpMonster->m_MonsterInfo.m_cNation);

            GameProcess::LogCharLoad(dwMonsterID, LOG_RFL);

            lpMonsterCreature = RYLCreatureManager::Instance()->GetCreature(dwMonsterID);
            lpMonsterData = RYLCharacterDataManager::Instance()->GetCharData(dwMonsterID);

            if (0 != lpMonsterCreature)
            {
                // �� ������ ���� ������ ����
                lpMonsterCreature->SetScale(lpMonster->m_MonsterInfo.m_fSize);

                CZ3DGeneralChrModel* lpMonsterModel = lpMonsterCreature->GetCharModel();
			    if (lpMonsterModel) { lpMonsterModel->SetScale(lpMonster->m_MonsterInfo.m_fSize); }
            }
        }

        if (0 != lpMonsterCreature && 0 != lpMonsterData)
        {
            // ���� ������ ����
			lpMonsterData->m_lCurrHP = monData.GetHP();

		    if (lpMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE)
		    {
			    lpMonsterData->m_nChrPattern = lpMonster->m_MonsterInfo.m_cSkillPattern;
			    lpMonsterData->m_ulMaxHP = lpMonster->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
			    lpMonsterData->m_ulMaxMP = lpMonster->m_CreatureStatus.m_StatusInfo.m_nMaxMP;

                // �ǹ��� ���, �ǹ� �μ��� üũ                
				float fNowHp = (0 != lpMonsterData->m_ulMaxHP) 
					? ( (float)lpMonsterData->m_lCurrHP / (float)lpMonsterData->m_ulMaxHP ) : 0.0f;
                
				if (fNowHp < 0.34f)     { lpMonsterCreature->SetAction(CA_WAITH2, CA_WAITH2); } 
				else if (fNowHp < 0.67f) { lpMonsterCreature->SetAction(CA_WAITH1, CA_WAITH1); }
                else if (lpMonsterCreature->GetUpperActionID() == CA_WAITH1 || 
                    lpMonsterCreature->GetLowerActionID() == CA_WAITH2)
				{
					lpMonsterCreature->SetAction(CA_WAIT, CA_WAIT);
				}
		    }

            // æƮ ����Ʈ ����
            SetChantEffectToCreature(lpMonsterCreature, monData.GetEnchantInfo());
        }                
	}
}

void CBroadcastCharDataProcess::RequestGuildInfo(unsigned long dwGID, bool bForceRequest)
{
    if (0 != m_lpGuildPannel)
    {
        if (0 != dwGID && 
            0 == m_lpGuildPannel->FindGuild(dwGID & ~Guild::MEMBER_WAIT_BIT))
        {
            m_lstGID.insert(dwGID & ~Guild::MEMBER_WAIT_BIT);
        }

        size_t nGuildNum = m_lstGID.size();
        if (10 <= nGuildNum || (0 != nGuildNum && bForceRequest))
        {
            m_lpGuildPannel->RequestNewGuild(m_lstGID);
            m_lstGID.clear();
        }
    }
}

void CBroadcastCharDataProcess::SetChantEffectToCreature(RYLCreature* lpRYLCreature, const EnchantInfo& enchantInfo)
{
    // by Hades Kang - option dialog < chant effect >
    unsigned long dwCID = lpRYLCreature->GetCharID();

	int	index = 0;
	if (dwCID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		index = 0;
	}
	else
	{
		switch(g_CharacterData.GetClientEnemyType(dwCID))
		{
			case EnemyCheck::EC_ENEMY:		index = 1;  break;
			case EnemyCheck::EC_NEUTRAL:	index = 2;  break;
			case EnemyCheck::EC_FRIEND:		index = 3;  break;
		}
	}

    if (0 != lpRYLCreature->GetEffect())
    {
        lpRYLCreature->GetEffect()->SetChantEffect(
            enchantInfo, (0 == index),
            g_CharacterData.m_bGoodChantEffectShow[index], 
            g_CharacterData.m_bBadChantEffectShow[index]);
    }
}

bool CBroadcastCharDataProcess::ProcessBroadcastLogin(Broadcast2nd::PktBroadcast* lpBroadcast, 
                                                             unsigned short usDataLen)
{
    CRYLGameData* lpGame = CRYLGameData::Instance();

    if (lpGame)
    {
        switch(lpBroadcast->m_cDataType)
        {
			case Broadcast2nd::PktBroadcast::CHAR_DATA:
            {
                g_ClientCellMgr.CellLogin(reinterpret_cast<char*>(lpBroadcast + 1), usDataLen);
                
                if (g_bLogCharBroadcast)
                {
                    // �� �α��� �����͸� ��´�. (ĳ���͸�)
                    g_ClientCellMgr.ProcessCharacter(CLogCharCellLogin());                    
                }

                // ĳ���͸� ���� ���鼭, ������ ĳ���� �����ϰ�, ������ �����͸� ������Ʈ�Ѵ�.               
                CBroadcastCharDataProcess::GIDList lstGID;
                CBroadcastCharDataProcess broadcastLogin(lstGID);

                if (broadcastLogin.isValid())
                {
                    g_ClientCellMgr.ProcessCharacter(broadcastLogin);
                    g_ClientCellMgr.ProcessMonster(broadcastLogin);

                    broadcastLogin.RequestGuildInfo(0, true);                    
                }
            }
            break;

			case Broadcast2nd::PktBroadcast::ITEM_DATA:
            {
                // ������ �����͸� ���� �ʵ忡 ���.
                FieldObject* lpFieldPos = reinterpret_cast<FieldObject*>(lpBroadcast + 1);
                FieldObject* lpFieldEnd = lpFieldPos + (usDataLen / sizeof(FieldObject));

                for(; lpFieldPos != lpFieldEnd; ++lpFieldPos)
                {
                    lpGame->AddFieldObject(lpFieldPos);
                }
            }
            break;
        }
    }

    return true;
}

bool CBroadcastCharDataProcess::ProcessBroadcastDelta(Broadcast2nd::PktBroadcast* lpBroadcast, 
                                                      unsigned short usDataLen)
{
    switch(lpBroadcast->m_cDataType)
    {
    case Broadcast2nd::PktBroadcast::CHAR_DATA:
        {
            g_ClientCellMgr.CellUpdateData(reinterpret_cast<char*>(lpBroadcast + 1), 
                usDataLen, lpBroadcast->m_dwCurrentPulse);
            
            if (g_bLogCharBroadcast)
            {
                // �� ������ ������Ʈ�� ��´�. (ĳ���͸�)
                g_ClientCellMgr.ProcessCharacter(CLogCharUpdateData());                
            }

            // ĳ���͸� ���� ���鼭, ������ �����ϰ�, ����� ĳ���͸� �����͸� ������Ʈ�Ѵ�.
            CBroadcastCharDataProcess::GIDList lstGID;
            CBroadcastCharDataProcess broadcastUpdate(lstGID);

            if (broadcastUpdate.isValid())
            {
                g_ClientCellMgr.ProcessCharacter(broadcastUpdate);
                g_ClientCellMgr.ProcessMonster(broadcastUpdate);

                broadcastUpdate.RequestGuildInfo(0, true);
            }
        }
        break;
    }

    return true;
}


class CRemoveNotCurrentPulse
{
public:

    CRemoveNotCurrentPulse(unsigned long dwCurrentPulse) : m_dwCurrentPulse(dwCurrentPulse) { }

    void operator() (Broadcast2nd::CCharacterData& charData)
    {
        if (charData.GetLastUpdatedPulse() != m_dwCurrentPulse &&
            charData.GetCID() != CRYLNetworkData::Instance()->m_dwMyChrID)
        {
            CBroadcastCharDataProcess::LogCharCell(charData, 
                CBroadcastCharDataProcess::CELL_LOGOUT, "By Not Current Pulse"); 

            RYLCreatureManager::Instance()->DeleteCreature(charData.GetCID());
        }        
    }

	void operator() (Broadcast2nd::CMonsterData& monData)
	{
		if (monData.GetLastUpdatedPulse() != m_dwCurrentPulse)
		{
			RYLCreature* pMonster = RYLCreatureManager::Instance()->GetCreature(monData.GetCID());
			if (NULL != pMonster && !pMonster->GetIsDead())
			{
				RYLCreatureManager::Instance()->DeleteCreature(monData.GetCID());
			}
		}
	}

private:

    unsigned long m_dwCurrentPulse;
};


bool CBroadcastCharDataProcess::ProcessRemoveCharacterByPulse(unsigned long dwCurrentPulse)
{
    // ���� �޽��� �ƴ� �༮�� ũ���� �ʿ��� �����Ѵ�.
    g_ClientCellMgr.ProcessCharacter(
        CRemoveNotCurrentPulse(dwCurrentPulse));

	g_ClientCellMgr.ProcessMonster(
		CRemoveNotCurrentPulse(dwCurrentPulse));

    // ���� �޽��� �ƴ� �༮�� ���� ���鼭 �����Ѵ�.
    g_ClientCellMgr.RemoveCharDataByPulse(dwCurrentPulse, 
        CRYLNetworkData::Instance()->m_dwMyChrID);

    return true;
}

void CBroadcastCharDataProcess::LogCharCell(Broadcast2nd::CCharacterData& charData, 
                                            CELL_OPER_TYPE eOperationType, const char* szExtData)
{
    if (g_bLogCharBroadcast)
    {
        const char* szOperation = "Unknown";
        if (0 == szExtData) { szExtData = ""; }
        
        switch(eOperationType)
        {
        case CELL_LOGIN:        szOperation = "CellLogin";      break;
        case CELL_LOGOUT:       szOperation = "CellLogout";     break;
        case CELL_BROADCAST:    szOperation = "CellBoadcast";   break;
        }

        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        // ǥ���׸� : CID / �̸� / ��ǥ(X,Y,Z)
        const CNetworkPos& pos = charData.GetNetworkPos();

        fprintf(stderr, "[%4d-%2d-%2d %2d:%2d:%2d]\tCID:0x%08x\tName:%s\tPos[%4f][%4f][%4f]\t%s:%s\n",
            sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
            charData.GetCID(), charData.GetName(), pos.GetXPos(), pos.GetYPos(), pos.GetZPos(),
            szOperation, szExtData);
    }
}


void GameProcess::LogCharLoad(unsigned long dwCID, const char* szRoutine, 
                              const char* szFile, const int nLine)
{
    if (g_bLogCharBroadcast)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        fprintf(stderr, "[%4d-%2d-%2d %2d:%2d:%2d]\tCID:0x%08x\t\tCharLoad\tRtn:%s\tFile:%s\tLine:%d\n",
            sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
            dwCID, szRoutine, szFile, nLine);
    }
}


void GameProcess::LogCharUnload(unsigned long dwCID, const char* szRoutine, 
                                const char* szFile, const int nLine)
{
    if (g_bLogCharBroadcast)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        fprintf(stderr, "[%4d-%2d-%2d %2d:%2d:%2d]\tCID:0x%08x\t\tCharUnload\tRtn:%s\tFile:%s\tLine:%d\n",
            sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
            dwCID, szRoutine, szFile, nLine);
    }
}


