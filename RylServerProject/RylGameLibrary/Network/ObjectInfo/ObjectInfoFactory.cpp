/* TODO : REMOVE HERE
#include "stdafx.h"
#include "ObjectInfoFactory.h"

#include <Creature/AggresiveCreature.h>
#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

using namespace BroadcastInfo;

bool BroadcastInfo::SerializeOutAggresiveCreatureInfo(CAggresiveCreature& AggresiveCreature, DataType::Type eDataType, 
                                                      char* szBuffer_Out, unsigned long& dwBufferSize_InOut)
{
    // 기본 정보 삽입.
    unsigned long dwObjectSize = GetObjectSize(eDataType);

    if (0 != dwObjectSize && dwObjectSize <= dwBufferSize_InOut)
    {
        const Position& Pos = AggresiveCreature.GetCurrentPos();

        SObject* lpObjectInfo = new (szBuffer_Out) SObject(AggresiveCreature.GetCID());
        
        const MotionInfo& motionInfo = AggresiveCreature.GetMotionInfo();

        lpObjectInfo->m_NetworkPos.Initialize(Pos.m_fPointX, Pos.m_fPointY, Pos.m_fPointZ, 
            motionInfo.m_fDirection, (0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame);
        
        lpObjectInfo->SetObjectType(ObjectType::AGGRESIVE_CREATURE);        
        lpObjectInfo->SetDataType(eDataType);

        if (IsAggresiveCreatureInfo(eDataType))
        {
            SAggresiveCreature* lpAggresiveCreatureInfo = static_cast<SAggresiveCreature*>(lpObjectInfo);
			std::copy(AggresiveCreature.GetEnchantInfo().m_dwStatusFlag, 
				AggresiveCreature.GetEnchantInfo().m_dwStatusFlag + EnchantInfo::MAX_ARRAY, 
				lpAggresiveCreatureInfo->m_EnchantInfo.m_dwStatusFlag);
			lpAggresiveCreatureInfo->m_nNowHP = AggresiveCreature.GetStatus().m_nNowHP;
        }

        dwBufferSize_InOut = dwObjectSize;
        return true;
    }

    dwBufferSize_InOut = 0;
    return false;
}


bool BroadcastInfo::SerializeOutMonsterInfo(CMonster& Monster, DataType::Type eDataType, 
                                            char* szBuffer_Out, unsigned long& dwBufferSize_InOut)
{
    if (BroadcastInfo::SerializeOutAggresiveCreatureInfo(Monster, eDataType, szBuffer_Out, dwBufferSize_InOut))
    {
        SObject* lpObjectInfo = reinterpret_cast<SObject*>(szBuffer_Out);
        lpObjectInfo->SetObjectType(ObjectType::MONSTER);

		if (IsMonsterInfo(eDataType))
		{
			Monster::SMonster* lpMonsterInfo  = static_cast<Monster::SMonster*>(lpObjectInfo);
		}

		return true;
    }

    return false;
}


bool BroadcastInfo::SerializeOutSummonMonsterInfo(CSummonMonster& SummonMonster, DataType::Type eDataType, 
												  char* szBuffer_Out, unsigned long& dwBufferSize_InOut)
{
    if (BroadcastInfo::SerializeOutMonsterInfo(SummonMonster, eDataType, szBuffer_Out, dwBufferSize_InOut))
    {
        SObject* lpObjectInfo = reinterpret_cast<SObject*>(szBuffer_Out);

        if (IsSummonMonsterInfo(eDataType))
        {
			Monster::SSummonMonster* lpSummonMonsterInfo  = static_cast<Monster::SSummonMonster*>(lpObjectInfo);

			CCharacter* lpMaster = SummonMonster.GetMaster();
			if (NULL == lpMaster) 
			{ 
				// 운영자 명령으로 소환한 소환수
				return true; 
			}
			
			lpSummonMonsterInfo->m_dwMasterID = lpMaster->GetCID();
		}

		lpObjectInfo->SetObjectType(ObjectType::SUMMON_MONSTER);
		return true;
    }

    return false;
}

*/