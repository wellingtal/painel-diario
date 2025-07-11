#include "stdafx.h"
#include "DummyCharacters.h"

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/Cell.h>
#include <Creature/Character/Character.h>
#include <Creature/Character/CharacterClass.h>


#include <Item/Item.h>
#include <Item/ItemFactory.h>


struct DummyData
{
    unsigned long m_dwDressChangeFreq;
    unsigned long m_dwAttackFreq;
    unsigned long m_dwSkillFreq;

    DummyData() : m_dwDressChangeFreq(0), m_dwAttackFreq(0), m_dwSkillFreq(0) { }
};


class CModifyDummyCharacter : public CCharacter
{
public:

    CModifyDummyCharacter(unsigned long dwCID) : CCharacter(dwCID, 0) {  }
    virtual ~CModifyDummyCharacter() { };

    void ModifyCharacterData(CharacterDBData& DBData, DummyData& dummyData)
    {
        m_bLogout   = false;
        m_DBData    = DBData; 
        m_DummyData = dummyData;
    }

    void ModifyEquipmentData(CDummyCharacterList::EquipmentData& EquipmentData);
    void SetLogout() { m_bLogout = true; }

private:

    DummyData   m_DummyData;
};

    
void CModifyDummyCharacter::ModifyEquipmentData(CDummyCharacterList::EquipmentData& equipmentData)
{
    Item::CEquipmentsContainer& Equipments = GetEquipments();
    
    Equipments.ClearItems();

    for(int nCount = 0; nCount < Item::EquipmentPos::MAX_EQUPMENT_POS; ++nCount)
    {
        if(0 != equipmentData.m_usEquipments[nCount])
        {
            // UID없는 임시 아이템을 생성한다.
            Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateTempItem(
                equipmentData.m_usEquipments[nCount]);

            if(0 != lpItem)
            {
                // 아이템을 스탯에 관계없이 강제로 삽입한다.
                Equipments.CListContainer::SetItem(
                    Item::ItemPos(TakeType::TS_EQUIP, nCount), lpItem);
            }
        }
    };
}


CDummyCharacterList& CDummyCharacterList::GetInstance()
{
    static CDummyCharacterList dummyCharacterList;
    return dummyCharacterList;
}

CDummyCharacterList::CDummyCharacterList()
{


}

CDummyCharacterList::~CDummyCharacterList()
{
    Destroy();
}


bool CDummyCharacterList::Initialize(unsigned long dwLoadCharNum)
{
    EquipmentVector emptyVector;
    std::pair<EquipmentClassSet::iterator, bool> insertResult;

#define INSERT_CLASS_SET(fileName, Class) \
    insertResult = m_EquipmentClassSet.insert(std::make_pair(Class, emptyVector)); \
    if(insertResult.second) \
    { if(!LoadEquipments(fileName, insertResult.first->second)) { return false; } }

    INSERT_CLASS_SET("./Dummies/CON디펜더.txt",        CClass::Defender);
    INSERT_CLASS_SET("./Dummies/스탠다드워리어.txt",   CClass::Warrior);
    INSERT_CLASS_SET("./Dummies/스탠다드어쌔신.txt",   CClass::Assassin);
    INSERT_CLASS_SET("./Dummies/스탠다드아처.txt",     CClass::Archer);
    INSERT_CLASS_SET("./Dummies/스탠다드소서러.txt",   CClass::Sorcerer);
    INSERT_CLASS_SET("./Dummies/스탠다드인챈터.txt",   CClass::Enchanter);
    INSERT_CLASS_SET("./Dummies/스탠다드프리스트.txt", CClass::Priest);
    INSERT_CLASS_SET("./Dummies/스탠다드클레릭.txt",   CClass::Cleric);

    INSERT_CLASS_SET("./Dummies/스탠다드템플러.txt",     CClass::Templar);
    INSERT_CLASS_SET("./Dummies/스탠다드어태커.txt",     CClass::Attacker);
    INSERT_CLASS_SET("./Dummies/스탠다드거너.txt",       CClass::Gunner);
    INSERT_CLASS_SET("./Dummies/스탠다드룬오프.txt",     CClass::RuneOff);
    INSERT_CLASS_SET("./Dummies/스탠다드라이프오프.txt", CClass::LifeOff);
    INSERT_CLASS_SET("./Dummies/스탠다드쉐도우오프.txt", CClass::ShadowOff);

    // 더미 캐릭터들을 로드해서 세팅한다.
    return LoadDummyChars("./Dummies/캐릭터데이터.txt", 
        m_EquipmentClassSet, m_DummyCharacterList, dwLoadCharNum);
}

bool CDummyCharacterList::Destroy()
{
    DummyCharList::iterator pos = m_DummyCharacterList.begin();
    DummyCharList::iterator end = m_DummyCharacterList.end();

    for(;pos != end; ++pos)
    {
        CModifyDummyCharacter* lpDummyChar = *pos;

        CCell* lpCell = lpDummyChar->GetCellPos().m_lpCell;
        if(0 != lpCell)
        {
            lpCell->DeleteCreature(lpDummyChar->GetCID());
        }
       
        lpDummyChar->SetLogout();
        delete lpDummyChar;
    }

    m_DummyCharacterList.clear();
    m_EquipmentClassSet.clear();
    return true;
}


bool CDummyCharacterList::LoadEquipments(const char* szFileName, EquipmentVector& equipmentVector)
{
    FILE* lpFile = fopen(szFileName, "rt");
    if(0 != lpFile)
    {
        const int MAX_BUFFER = 1024;
        char szBuffer[MAX_BUFFER];

        const char* szDelimiter = " \t\r\n";
        char* szToken = 0;

        EquipmentData equipmentData;
        EquipmentData oldEquipmentData;

        while(fgets(szBuffer, MAX_BUFFER, lpFile))
        {
            memset(equipmentData.m_usEquipments, 0, sizeof(unsigned short)
                * Item::EquipmentPos::MAX_EQUPMENT_POS);

            szToken = strtok(szBuffer, szDelimiter);
            if(0 != szToken)
            {   // 무기                
                equipmentData.m_usEquipments[Item::EquipmentPos::WEAPON_HAND1] = 
                    static_cast<unsigned short>(atoi(szToken));
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {   // 방패
                equipmentData.m_usEquipments[Item::EquipmentPos::SHIELD_HAND1] = 
                    static_cast<unsigned short>(atoi(szToken));
            }              

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {   // 투구
                equipmentData.m_usEquipments[Item::EquipmentPos::HELM] = 
                    static_cast<unsigned short>(atoi(szToken));
            }              

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {   // 갑옷
                equipmentData.m_usEquipments[Item::EquipmentPos::ARMOUR] = 
                    static_cast<unsigned short>(atoi(szToken));
            }              

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {   // 장갑
                equipmentData.m_usEquipments[Item::EquipmentPos::GLOVE] = 
                    static_cast<unsigned short>(atoi(szToken));
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {   // 신발
                equipmentData.m_usEquipments[Item::EquipmentPos::BOOTS] = 
                    static_cast<unsigned short>(atoi(szToken));
            }
            
            if(0 != memcmp(oldEquipmentData.m_usEquipments, equipmentData.m_usEquipments, 
                sizeof(unsigned short) * Item::EquipmentPos::MAX_EQUPMENT_POS))
            {
                // 이전 장비 데이터와 겹치지 않으면 추가.
                equipmentVector.push_back(equipmentData);
            }

            oldEquipmentData = equipmentData;                        
        }

        fclose(lpFile);
        return true;
    }

    return false;
}


bool CDummyCharacterList::LoadDummyChars(const char* szFileName, 
                                         EquipmentClassSet& equipmentSet, 
                                         DummyCharList& dummyCharList, 
                                         unsigned long dwLoadCharNum)
{
    FILE* lpFile = fopen(szFileName, "rt");
    if(lpFile)
    {
        const int MAX_BUFFER = 8192;
        char szBuffer[MAX_BUFFER];

        const char* szDelimiter = " \t\r\n";
        char* szToken = 0;

        CharacterDBData charDBData;
        DummyData       dummyData;

        unsigned long dwCharCount = 0;

        while(fgets(szBuffer, MAX_BUFFER - 1, lpFile) && dwCharCount < dwLoadCharNum)
        {
            memset(&charDBData, 0, sizeof(charDBData));
            memset(&dummyData, 0, sizeof(dummyData));
            szBuffer[MAX_BUFFER - 1] = 0;

            szToken = strtok(szBuffer, szDelimiter);
            if(0 != szToken)
            {
                // CID
                charDBData.m_Info.CID = atol(szToken);
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 이름
                strncpy(charDBData.m_Info.Name, szToken, CHAR_INFOST::MAX_NAME_LEN);
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 레벨
                charDBData.m_Info.Level = atoi(szToken);
            }
            
            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 클래스, 종족
                charDBData.m_Info.Class = atoi(szToken);
                charDBData.m_Info.Race = CClass::GetRace(static_cast<unsigned char>(charDBData.m_Info.Class));
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 머리칼
                charDBData.m_Info.Hair = atoi(szToken);
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 얼굴
                charDBData.m_Info.Face = atoi(szToken);
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 성별
                charDBData.m_Info.Sex = atoi(szToken);
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // X좌표
                charDBData.m_Pos.LastPoint.fPointX = static_cast<float>(atoi(szToken));
//                charDBData.m_Pos.SavePoint.fPointX = static_cast<float>(atoi(szToken));
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // Y좌표
                charDBData.m_Pos.LastPoint.fPointY = static_cast<float>(atoi(szToken));
//                charDBData.m_Pos.SavePoint.fPointY = static_cast<float>(atoi(szToken));
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // Z좌표
                charDBData.m_Pos.LastPoint.fPointZ = static_cast<float>(atoi(szToken));
//                charDBData.m_Pos.SavePoint.fPointZ = static_cast<float>(atoi(szToken));
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 옷갈아입기 빈도 (n초당 한번 정도 랜덤으로 결정)
                dummyData.m_dwDressChangeFreq = atoi(szToken);                
            }            

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 공격모션 빈도 (n초당 한번 정도 랜덤으로 결정)
                dummyData.m_dwAttackFreq = atoi(szToken);                
            }

            szToken = strtok(0, szDelimiter);
            if(0 != szToken)
            {
                // 스킬사용 빈도 (n초당 한번 정도 랜덤으로 결정)
                dummyData.m_dwSkillFreq = atoi(szToken);
            }

            CModifyDummyCharacter* lpCharacter = new CModifyDummyCharacter(charDBData.m_Info.CID);
            if(0 != lpCharacter)
            {
                lpCharacter->Initialize(0); 
                CCreatureManager::GetInstance().CancelLogout(lpCharacter);

                charDBData.m_Info.Fame = rand();
                charDBData.m_Info.HP = 100;
                charDBData.m_Info.MP = 100;
                charDBData.m_Info.STR = rand();
	            charDBData.m_Info.DEX = rand();
	            charDBData.m_Info.CON = rand();
	            charDBData.m_Info.INT = rand();
	            charDBData.m_Info.WIS = rand();

                lpCharacter->ModifyCharacterData(charDBData, dummyData);

                EquipmentClassSet::iterator pos = equipmentSet.find(lpCharacter->GetClass());
                EquipmentClassSet::iterator end = equipmentSet.end();

                if(pos != end)
                {
                    EquipmentVector& equipmentVector = pos->second;

                    if(!equipmentVector.empty())
                    {
                        lpCharacter->ModifyEquipmentData(
                            equipmentVector[rand() % equipmentVector.size()]);
                    }
                }

                dummyCharList.push_back(lpCharacter);

                Position newPosition(charDBData.m_Pos.LastPoint.fPointX, 
                    charDBData.m_Pos.LastPoint.fPointY,
                    charDBData.m_Pos.LastPoint.fPointZ);

                lpCharacter->Login();

                lpCharacter->GetSerializeData().PrepareData(*lpCharacter);
                lpCharacter->GetSerializeData().ClearDeltaData();

                lpCharacter->MoveTo(newPosition, false);

                ++dwCharCount;
            }
        }

        fclose(lpFile);
        return true;
    }

    return false;
}