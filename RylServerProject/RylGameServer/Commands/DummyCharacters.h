#ifndef _GAMA_GAME_DUMMY_CHARACTERS_H_
#define _GAMA_GAME_DUMMY_CHARACTERS_H_

#include <list>
#include <map>
#include <vector>

#include <Item/ItemConstants.h>

/*
 캐릭터 브로드캐스팅 기능 작성 - 
    게임 서버를 똑같이 복사한 다음 별도의 프로젝트로  이름을 바꾸어서 작업을 진행한다. 
 
 설정 파일을 읽어 재현하는 기능을 갖는다. default는 off이다.
 가능한 행동 - 옷갈아입기. 손바꾸기. 이동. 공격모션.
 
 설정 파일은 다음과 같은 항목을 갖는다.
 - 브로드캐스팅에 필요한 기본 캐릭터 정보 (이름, 레벨, 클래스 등)
 - 종족 및 클래스에 맞는 랜덤 복장 셋. (각 복장 부위에 따른 itemUID. 휴먼, 아칸 따로.)
 - 캐릭터별 이동 경로, 옷갈아입기 빈도, 손바꾸기 빈도, 이동 및 공격모션 빈도.
 - 캐릭터를 한 셀에 100명씩 800명을 띄우고 들어갔을 경우의 트래픽 및 서버 상태를 본다.
 
1. 먼저 할 일은 프로젝트 생성, 설정 파일 포맷 결정과 그에 맞는 데이터 구조를 생성하고 읽어온다.
2. 가짜 캐릭터를 생성해서 데이터를 세팅하고 셀에 집어 넣는다.
3. 가짜 캐릭터가 1초에 4번씩 가짜 이동 패킷을 보내도록 한다.
4. 가짜 캐릭터가 경로를 따라서 이동을 할 수 있게 한다. 이동처리 및 가짜 이동 패킷 보내기..
5. 가짜 캐릭터가 주기적으로 옷을 갈아입을 수 있게 한다.
6. 가짜 캐릭터가 주기적으로 손을 바꿀 수 있게 한다.
7. 가짜 캐릭터가 주기적으로 공격 모션을 표현할 수 있게 한다.
8. 가짜 캐릭터가 주기적으로 주변에 있는 캐릭터를 붙잡고 스킬(!) 을 쓸 수 있게 한다.

더미 캐릭터 파일 포맷

// 캐릭터 정보
 
CID/이름/레벨/클래스/머리칼/얼굴/성별/옷갈아입기 빈도/공격모션 빈도/스킬사용 빈도

// 복장 정보
인간 : 무기 방패 투구 갑옷 장갑 신발
아칸 : 무기 스킬암 투구 갑옷 프로텍트암 거들

// 이동 경로

*/

// forward decl.
class CModifyDummyCharacter;

class CDummyCharacterList
{
public:

    struct EquipmentData
    {
        unsigned short  m_usEquipments[Item::EquipmentPos::MAX_EQUPMENT_POS];
    };

    static CDummyCharacterList& GetInstance();

    bool Initialize(unsigned long dwLoadCharNum);
    bool Destroy();

private:

    CDummyCharacterList();
    ~CDummyCharacterList();

    typedef std::vector<EquipmentData>                  EquipmentVector;
    typedef std::map<unsigned long, EquipmentVector>    EquipmentClassSet;

    typedef std::list<CModifyDummyCharacter*>           DummyCharList;

    static bool LoadEquipments(const char* szFileName, EquipmentVector& equipmentVector);
    static bool LoadDummyChars(const char* szFileName, 
        EquipmentClassSet& equipmentSet, DummyCharList& dummyCharList, unsigned long dwLoadCharNum);

    DummyCharList       m_DummyCharacterList;
    EquipmentClassSet   m_EquipmentClassSet;
};


#endif