#ifndef	_TEMP_CHARACTER_H_
#define	_TEMP_CHARACTER_H_

#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>

#include <map>
#include <DB/DBdefine.h>

class CTempCharacterMgr;

// TODO : ȯ����(��Ʋ�׶��� ������)������ ���۵� Ŭ�����Դϴ�. 
//	������� ����ϰ� �ִ� CModifyCharacter�� ��� �Ϻθ� �����ؿԴµ�...
//	���� CModifyCharacter�� �� Ŭ������ ��ӹ޾� �ణ �����ϸ� ���� �� �մϴ�.
class CTempCharacter
{
public:

    enum Const
    {
        SLOT_INFO_SET   = (1 << 0),     // ���� UID/CID/�̸� �� ���õǾ����� ����
        CHAR_DATA_SET   = (1 << 1),     // ĳ���� �����Ͱ� ���õǾ����� ����

        SLOT_INFO_REQ   = (1 << 2),     // ���� UID/CID/�̸� �� ��û�Ͽ���.
        CHAR_DATA_REQ   = (1 << 3)      // ĳ���� �����͸� ��û�Ͽ���
    };

    CTempCharacter();
	~CTempCharacter();

	unsigned char GetGroup(void) { return m_cGroup;   }
    void SetGroup(unsigned char cGroup) { m_cGroup = cGroup; }

	// �ӽ� ��ü�̹Ƿ� ���̵� ������� ������ �� �ִ�. �ٸ� ��(��/���� Ŭ����)���� ����ϸ� �����ϴ�.
	void SetUID(unsigned long dwUID) { m_dwUID = dwUID; }
	void SetCID(unsigned long dwCID) { m_dwCID = dwCID; }

    // ����Ǵ� UID/CID�� ���� UID, ���� CID�̴�. (��Ʋ���Ѱ��� �ٸ���!)
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

    unsigned char		m_cGroup;		// ���� ������
    unsigned char       m_cFlag;
};

class CTempCharacterMgr
{
public:

    // Key : ��Ʋ���� ĳ���� CID / Value : ĳ���� ����
    typedef std::multimap<unsigned long, CTempCharacter*, std::less<unsigned long>, 
        boost::fast_pool_allocator<std::pair<unsigned long, CTempCharacter*> > > isMapCharList;

    // CID/Group�� Ű�� �ϰ�, ��ġ�ϴ� Ŭ������ ������ �����´�. ������ ������ �� �����Ѵ�.
    CTempCharacter*	GetCharacter(unsigned long dwBattleCID, unsigned char cGroup);	

    // ĳ���� �α׾ƿ��� ȣ���Ѵ�.
    bool EraseChar(unsigned long dwBattleCID);        

    ~CTempCharacterMgr();

private:

    boost::object_pool<CTempCharacter>  m_tempCharPool;
    isMapCharList                       m_mapTempChar;
};

#endif