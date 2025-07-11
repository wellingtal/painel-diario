/* TODO : REMOVE HERE
#ifndef _OBJECT_INFO_FACTORY_H_
#define _OBJECT_INFO_FACTORY_H_


#include <Network/Packet/PacketStruct/CharBroadcastPacket.h>


// 전방 참조
class CAggresiveCreature;
class CCharacter;
class CMonster;
class CSummonMonster;


namespace BroadcastInfo
{
    bool SerializeOutAggresiveCreatureInfo(CAggresiveCreature& AggresiveCreature, 
        DataType::Type eDataType, char* szBuffer_Out, unsigned long& dwBufferSize_InOut);
       
    bool SerializeOutMonsterInfo(CMonster& Monster, 
        DataType::Type eDataType, char* szBuffer_Out, unsigned long& dwBufferSize_InOut);

	bool SerializeOutSummonMonsterInfo(CSummonMonster& SummonMonster, 
        DataType::Type eDataType, char* szBuffer_Out, unsigned long& dwBufferSize_InOut);
}


#endif
*/