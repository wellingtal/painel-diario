#ifndef	_TEMP_CHARACTER_H_
#define	_TEMP_CHARACTER_H_

#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>

#include <map>
#include <DB/DBdefine.h>

class CTempCharacterMgr;

// TODO : 환전소(배틀그라운드 서버군)용으로 제작된 클래스입니다. 
//	운영툴에서 사용하고 있는 CModifyCharacter의 기능 일부를 차용해왔는데...
//	추후 CModifyCharacter가 이 클래스를 상속받아 약간 수정하면 좋을 듯 합니다.
class CTempCharacter
{
public:

    enum Const
    {
        SLOT_INFO_SET   = (1 << 0),     // 정섭 UID/CID/이름 이 세팅되었는지 여부
        CHAR_DATA_SET   = (1 << 1),     // 캐릭터 데이터가 세팅되었는지 여부

        SLOT_INFO_REQ   = (1 << 2),     // 정섭 UID/CID/이름 을 요청하였음.
        CHAR_DATA_REQ   = (1 << 3)      // 캐릭터 데이터를 요청하였음
    };

    CTempCharacter();
	~CTempCharacter();

	unsigned char GetGroup(void) { return m_cGroup;   }
    void SetGroup(unsigned char cGroup) { m_cGroup = cGroup; }

	// 임시 객체이므로 아이디를 마음대로 설정할 수 있다. 다른 곳(상/하위 클래스)에서 사용하면 위험하다.
	void SetUID(unsigned long dwUID) { m_dwUID = dwUID; }
	void SetCID(unsigned long dwCID) { m_dwCID = dwCID; }

    // 저장되는 UID/CID는 정섭 UID, 정섭 CID이다. (배틀로한과는 다르다!)
    unsigned long GetUID() const { return m_dwUID; }
    unsigned long GetCID() const { return m_dwCID; }

    const char* GetCharacterName() const { return m_szCharacterName; }
    void SetCharacterName(const char* szName) 
    { 
        strncpy(m_szCharacterName, szName, CHAR_INFOST::MAX_NAME_LEN); 
        m_szCharacterName[CHAR_INFOST::MAX_NAME_LEN] = 0;
    }

    CHAR_INFOEX&    GetCharInfoEx() { return m_CharInfoEX; }
    QUEST&          GetQuest()      { return m_Quest; }
    HISTORY&        GetHistory()    { return m_History; }
    CONFIG&         GetConfig()     { return m_Config; }
    STORE_INFO&     GetStoreInfo()  { return m_StoreInfo; }

    bool IsSetData(unsigned char cFlag) const { return 0 != (m_cFlag & cFlag); }
    void SetData(unsigned char cFlag) { m_cFlag |= cFlag; }
    void ResetData(unsigned char cFlag) { m_cFlag &= ~cFlag; }
    void ClearData() { m_cFlag = 0; }

    int AddDataRequestCount()       { return ++m_nDataRequestCount; }
    int ReleaseDataRequestCount()   { return --m_nDataRequestCount; }
    int GetDataRequestCount()       { return m_nDataRequestCount; }

private:

    unsigned long       m_dwUID;
    unsigned long       m_dwCID;

    int                 m_nDataRequestCount;

    char                m_szCharacterName[CHAR_INFOST::MAX_NAME_LEN];

    CHAR_INFOEX	        m_CharInfoEX;
    QUEST		        m_Quest;
    HISTORY		        m_History;
    CONFIG		        m_Config;
    STORE_INFO          m_StoreInfo;

    unsigned char		m_cGroup;		// 속한 서버군
    unsigned char       m_cFlag;
};

class CTempCharacterMgr
{
public:

    // Key : 배틀로한 캐릭터 CID / Value : 캐릭터 정보
    typedef std::multimap<unsigned long, CTempCharacter*, std::less<unsigned long>, 
        boost::fast_pool_allocator<std::pair<unsigned long, CTempCharacter*> > > isMapCharList;

    // CID/Group을 키로 하고, 일치하는 클래스가 있으면 가져온다. 없으면 생성한 후 리턴한다.
    CTempCharacter*	GetCharacter(unsigned long dwBattleCID, unsigned char cGroup);	

    // 캐릭터 로그아웃시 호출한다.
    bool EraseChar(unsigned long dwBattleCID);        

    ~CTempCharacterMgr();

private:

    boost::object_pool<CTempCharacter>  m_tempCharPool;
    isMapCharList                       m_mapTempChar;
};

#endif