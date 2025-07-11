
#include "stdafx.h"	

#include <Utility/Math/Math.h>
#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>
#include <Utility/Resource/EnsureCleanup.h>

#include <Network/XORCrypt/XORCrypt.h>

// Ŭ���̾�Ʈ������ ���̹Ƿ� include�� ����Ѵ�.
#include <Log/ServerLog.h>

#define ENCODEHEADER(Start_In, Length_In, PageNum_In, PageVer_In)   CXORCrypt::GetInstance().EncodeHeader((Start_In), (Length_In), (PageNum_In), (PageVer_In))
#define DECODEHEADER(Start_In, Length_In, PageNum_In, PageVer_In)   CXORCrypt::GetInstance().DecodeHeader((Start_In), (Length_In), (PageNum_In), (PageVer_In))
#define COMPRESS(In, In_len, Out, Out_len)      CMiniLZO::Compress((In), (In_len), (Out), (Out_len))
#define DECOMPRESS(In, In_len, Out, Out_len)    CMiniLZO::Decompress((In), (In_len), (Out), (Out_len))

#include <vector>
#include <algorithm>
#include <functional>
#include "MonsterMgr.h"
#include "GMMemory.h"

CMonsterMgr CMonsterMgr::ms_this;

const char* CMonsterMgr::m_szMonsterScriptFileName = "./Script/Game/MonsterProtoType.txt";


CMonsterMgr::CMonsterMgr(void)
:   m_nMonsterNum(0), m_ProtoTypeArray(NULL)
{

}

CMonsterMgr::~CMonsterMgr(void)
{
	ClearProtoType();	
}

void CMonsterMgr::ClearProtoType()
{
  	if (NULL != m_ProtoTypeArray) 
    {
        delete [] m_ProtoTypeArray;
    }
}

bool CMonsterMgr::LoadMonstersFromFile(const char* szFileName)
{
	int nIndex = 0;
    int nLineCount = 0;
    char strTemp[MAX_PATH];
	float fTemp = 0;

	CDelimitedFile                  DelimitedFile;	// ��ü �Ҹ��, �ڵ� Close.
    std::vector<MonsterProtoType>   monsterProtoTypeVector;

    monsterProtoTypeVector.reserve(1000);
    MonsterProtoType    tempProtoType;

    // ��ũ�ο� �α� �ڵ� ������ ���� �� ��.
	// ��ũ�ο��� \�ڿ� �����̳� ���� ���Ե��� �ʵ��� ������ ��.
	// ( '�̽������� �������� �߸��Ǿ����ϴ�' ���� �߻� )
	#define READ_DATA(ColumnName, Argument) \
		if (!DelimitedFile.ReadData(Argument)) { \
            ERRLOG2(g_Log, "���� ��ũ��Ʈ �б� ���� : %d�� %s�÷����� ���� �߻�!", nLineCount, #ColumnName); \
			return false; \
		}
	
	#define READ_STRING(ColumnName, Buffer, BufferSize) \
		if (!DelimitedFile.ReadString(Buffer, BufferSize)) { \
            ERRLOG2(g_Log, "���� ��ũ��Ʈ �б� ���� : %d�� %s�÷����� ���� �߻�!", nLineCount, #ColumnName); \
			return false; \
		}

	#define READ_DATA_ARRAY(ColumnName, Argument, ArgumentNum) \
		for (nIndex=0; nIndex < ArgumentNum; ++nIndex) { \
			READ_DATA(ColumnName, Argument[nIndex]); \
		}

	#define READ_SKILLID_ARRAY(ColumnName, Argument, ArgumentNum) \
		for (nIndex=0; nIndex < ArgumentNum; ++nIndex) { \
			if (!DelimitedFile.ReadString(strTemp, MAX_PATH)) { \
				ERRLOG2(g_Log, "���� ��ũ��Ʈ �б� ���� : %d�� %s�÷����� ���� �߻�!", nLineCount, #ColumnName); \
				return false; \
			} \
			char strString[MAX_PATH] = "0x0000"; \
			strcat(strString, (strTemp+2)); \
			Argument[nIndex] = static_cast<unsigned short>(Math::Convert::Atoi(strString)); \
		}

	#define READ_DATA_BOOL(ColumnName, Argument) \
		if (!DelimitedFile.ReadString(strTemp, MAX_PATH)) { \
            ERRLOG2(g_Log, "���� ��ũ��Ʈ �б� ���� : %d�� %s�÷����� ���� �߻�!", nLineCount, #ColumnName); \
			return false; \
		} \
        Argument = (!strcmp(strTemp, "O")) ? true : false;


	if (!DelimitedFile.Open(szFileName ? szFileName : m_szMonsterScriptFileName)) 
	{
		ERRLOG1(g_Log, "%s ������ �� �� �����ϴ�.", szFileName ? szFileName : m_szMonsterScriptFileName);
		return false;
	}

	while (DelimitedFile.ReadLine()) 
	{
        ++nLineCount;

		// ������ �ٲ�� ����ϴٴϱ�~!!! (����!)
		READ_DATA("KID", tempProtoType.m_MonsterInfo.m_dwKID);
		
		READ_STRING("�̸�", tempProtoType.m_MonsterInfo.m_strName, MonsterInfo::MAX_NAME_LEN);
		READ_STRING("�𵨸� ���� �÷���", tempProtoType.m_MonsterInfo.m_strModelingFlag, MonsterInfo::MAX_MODELING_FLAG_LENGTH);

		READ_DATA_BOOL("LOD ��� ����", tempProtoType.m_MonsterInfo.m_bUseLOD);

		READ_DATA("����", tempProtoType.m_MonsterInfo.m_cNation);

		READ_STRING("��ų����", strTemp, MAX_PATH);
        tempProtoType.m_MonsterInfo.m_cSkillPattern = MonsterInfo::GetMonsterPattern(strTemp);

		READ_DATA_BOOL("��æƮ���ÿ���", tempProtoType.m_MonsterInfo.m_bIgnoreEnchant);

		// ��� ����
		tempProtoType.m_MonsterInfo.m_MonsterMotions[0].m_wAction = MonsterInfo::Z3D_CA_WALK;												
		READ_DATA("�ȱ�������", tempProtoType.m_MonsterInfo.m_MonsterMotions[0].m_dwFrame);

		READ_DATA("�ȱ�Ÿ�", tempProtoType.m_MonsterInfo.m_MonsterMotions[0].m_fVelocity);
		tempProtoType.m_MonsterInfo.m_MonsterMotions[0].m_fVelocity /= 100.0f;  // ��ũ��Ʈ ����cm ��. ���ͷ� ��ȯ��.

		tempProtoType.m_MonsterInfo.m_MonsterMotions[1].m_wAction = MonsterInfo::Z3D_CA_RUN;											
		READ_DATA("�޸���������", tempProtoType.m_MonsterInfo.m_MonsterMotions[1].m_dwFrame);

		READ_DATA("�޸���Ÿ�", tempProtoType.m_MonsterInfo.m_MonsterMotions[1].m_fVelocity);
		tempProtoType.m_MonsterInfo.m_MonsterMotions[1].m_fVelocity /= 100.0f;  // ��ũ��Ʈ ����cm ��. ���ͷ� ��ȯ��.

		tempProtoType.m_MonsterInfo.m_MonsterMotions[2].m_wAction = MonsterInfo::Z3D_CA_ATTACK;							
		READ_DATA("����������", tempProtoType.m_MonsterInfo.m_MonsterMotions[2].m_dwFrame);

		tempProtoType.m_MonsterInfo.m_MonsterMotions[3].m_wAction = MonsterInfo::Z3D_CA_CASTING;						
		READ_DATA("��ų������", tempProtoType.m_MonsterInfo.m_MonsterMotions[3].m_dwFrame);

		READ_DATA_ARRAY("Ÿ�ݹڽ�", tempProtoType.m_MonsterInfo.m_fHitBox, MonsterInfo::MAX_HITBOX_NUM);
		READ_DATA("���ݰŸ�", tempProtoType.m_MonsterInfo.m_wAttackRange);
		READ_DATA("���ݰ���", tempProtoType.m_MonsterInfo.m_fAttackAngle);

        // �⺻ ����
		READ_DATA("�⺻ ������", tempProtoType.m_CreatureStatus.m_nExp);						
		READ_DATA("����", tempProtoType.m_CreatureStatus.m_nLevel);							
		READ_DATA("�ּҵ�����", tempProtoType.m_CreatureStatus.m_StatusInfo.m_lMinDamage);	
        READ_DATA("�ִ뵥����", tempProtoType.m_CreatureStatus.m_StatusInfo.m_lMaxDamage);	
		READ_DATA("����", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wHitRate);
		READ_DATA("����", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wArmor);			
		READ_DATA("ȸ��", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wEvade);
		READ_DATA("���", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wBlock);

		READ_DATA("DRC", fTemp);	// ��ȹ���� ���Ǹ� ���� ��ũ��Ʈ������ ����

		READ_DATA("ũ��Ƽ��", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wCritical);	
		READ_DATA("ũ��Ƽ��Ÿ��", tempProtoType.m_CreatureStatus.m_StatusInfo.m_cCriticalType);	
		READ_DATA("������", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wMagicPower);			
		READ_DATA("���׷�", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wMagicResist);			
		READ_DATA("�ӵ�", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wSpeed);			
		READ_DATA("HP Max", tempProtoType.m_CreatureStatus.m_StatusInfo.m_nMaxHP);				
		READ_DATA("MP Max", tempProtoType.m_CreatureStatus.m_StatusInfo.m_nMaxMP);				
		READ_DATA("HP ȸ����", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wHPRegen);		
		READ_DATA("MP ȸ����", tempProtoType.m_CreatureStatus.m_StatusInfo.m_wMPRegen);		

        // ��Ÿ
		READ_DATA("������", tempProtoType.m_MonsterInfo.m_fSize);
		READ_DATA("��ų����", tempProtoType.m_MonsterInfo.m_cSkillLevel);

		READ_DATA_BOOL("�浹Ÿ�Կ���", tempProtoType.m_MonsterInfo.m_bCollision);
		READ_DATA_BOOL("�������ݿ���", tempProtoType.m_MonsterInfo.m_bFirstAttack);
		READ_DATA_BOOL("��ȯ����", tempProtoType.m_MonsterInfo.m_bReturnPosition);

		READ_DATA_BOOL("�������������뿩��", tempProtoType.m_MonsterInfo.m_bFixLevelGap);
		READ_DATA("����������", tempProtoType.m_MonsterInfo.m_cFixLevelGap);

		READ_DATA("��ų��� Ȯ��", tempProtoType.m_MonsterInfo.m_wSkillUseRate);		

 		READ_SKILLID_ARRAY("���� ��ųID", tempProtoType.m_MonsterInfo.m_wSkillID, MonsterInfo::MAX_SKILL_PATTERN);
		READ_DATA("�ο���ų ����Ÿ��", tempProtoType.m_MonsterInfo.m_cEnchantSpellType);
		READ_DATA("æƮ��ų ����Ÿ��", tempProtoType.m_MonsterInfo.m_cChantSpellType);
		READ_DATA("��ų����Ʈ ������", tempProtoType.m_MonsterInfo.m_fSkillEffectSize);
		
		READ_DATA("������Ÿ��", tempProtoType.m_MonsterInfo.m_dwRespawnTime);
		READ_DATA_ARRAY("��������������", tempProtoType.m_MonsterInfo.m_aryAwardItem, MonsterInfo::MAX_ORIGINAL_ITEM_NUM);
		READ_DATA_ARRAY("�����۵����", tempProtoType.m_MonsterInfo.m_aryDropRate, MonsterInfo::MAX_AWARD_KIND);
		READ_DATA("Standard-Option Item Base", tempProtoType.m_MonsterInfo.m_cStandardBaseNum);
		READ_DATA("Over-Option Item Base", tempProtoType.m_MonsterInfo.m_cOverBaseNum);

		READ_STRING("���� ȿ�� Ÿ��", strTemp, MAX_PATH);
		tempProtoType.m_MonsterInfo.m_cStatueEffectType = MonsterInfo::GetStatueEffectType(strTemp);
		READ_DATA("���� ȿ�� �ۼ�Ʈ", tempProtoType.m_MonsterInfo.m_cStatueEffectPercent);
		READ_DATA_BOOL("���� ������ ���� ����", tempProtoType.m_MonsterInfo.m_bStatueRespawnEnable);

		// ���� ������Ʈ ���� �о���̱�
		READ_DATA("���� ���", tempProtoType.m_MonsterInfo.m_dwDevelopGold);
		READ_DATA("���� �ӵ�", tempProtoType.m_MonsterInfo.m_cDevelopSpeed);
		READ_DATA("���׷��̵� ���", tempProtoType.m_MonsterInfo.m_dwUpgradeGold);
		READ_DATA("���׷��̵� �ӵ�", tempProtoType.m_MonsterInfo.m_cUpgradeSpeed);

		READ_DATA("���� ��� ����", tempProtoType.m_MonsterInfo.m_fDevelopGoldDown);
		READ_DATA("���� �ӵ� ���", tempProtoType.m_MonsterInfo.m_fDevelopSpeedUp);
		READ_DATA("���׷��̵� ��� ����", tempProtoType.m_MonsterInfo.m_fUpgradeGoldDown);
		READ_DATA("���׷��̵� �ӵ� ���", tempProtoType.m_MonsterInfo.m_fUpgradeSpeedUp);
		READ_DATA("���� ���", tempProtoType.m_MonsterInfo.m_fDefenseUp);
		READ_DATA("����� ���", tempProtoType.m_MonsterInfo.m_fOffenseUp);
		READ_DATA("HP ���", tempProtoType.m_MonsterInfo.m_fHPUp);
		READ_DATA("�� ���� ���ʽ�", tempProtoType.m_MonsterInfo.m_fBonusRate);
		READ_DATA("PC ������ �ӵ� ���", tempProtoType.m_MonsterInfo.m_fRespawnSpeedUp);

        monsterProtoTypeVector.push_back(tempProtoType);
	}

    std::sort(monsterProtoTypeVector.begin(), monsterProtoTypeVector.end());

    for (std::vector<MonsterProtoType>::iterator itr = monsterProtoTypeVector.begin();
        itr != monsterProtoTypeVector.end() - 1; ++itr)
    {
        if (itr->m_MonsterInfo.m_dwKID == (itr+1)->m_MonsterInfo.m_dwKID)
        {
            ERRLOG1(g_Log, "��ġ�� ���� ���� ID�� �ֽ��ϴ�. ����ID:%d", itr->m_MonsterInfo.m_dwKID);
            return false;
        }
    }

    m_nMonsterNum = monsterProtoTypeVector.size();
    m_ProtoTypeArray = new MonsterProtoType[m_nMonsterNum];
    if (NULL == m_ProtoTypeArray) 
    {
        ERRLOG0(g_Log, "���� ��ũ��Ʈ �ʱ�ȭ ���� : �޸� ����");
        return false;
    }
        
    std::copy(monsterProtoTypeVector.begin(), monsterProtoTypeVector.end(), m_ProtoTypeArray);
	return true;
}


class CFindProtoTypeFromKID : public std::unary_function<CMonsterMgr::MonsterProtoType, bool>
{
public:

	explicit CFindProtoTypeFromKID(unsigned long dwKID) 
	:	m_dwKID(dwKID) 
	{ }		

	bool operator() (CMonsterMgr::MonsterProtoType& protoType) 
	{ 
		return (m_dwKID == protoType.m_MonsterInfo.m_dwKID); 
	}


private:

	const unsigned long m_dwKID;
};

const CMonsterMgr::MonsterProtoType* CMonsterMgr::GetMonsterProtoType(unsigned long dwKID)
{
	MonsterProtoType* lpFirst = m_ProtoTypeArray;
    MonsterProtoType* lpLast  = m_ProtoTypeArray + m_nMonsterNum;
    MonsterProtoType* lpMid   = NULL;

    size_t nCount   = m_nMonsterNum;
    size_t nCount2  = 0;

    for (; 0 < nCount; )
    {
        nCount2 = nCount / 2;
        lpMid = lpFirst + nCount2;

        if (lpMid->m_MonsterInfo.m_dwKID < dwKID)
        {
            lpFirst = ++lpMid, nCount -= nCount2 + 1;
        }
        else
        {
            nCount = nCount2;
        }
    }

    return (lpFirst != lpLast && !(dwKID < lpFirst->m_MonsterInfo.m_dwKID)) ? lpFirst : NULL;
}


const CMonsterMgr::MonsterProtoType* CMonsterMgr::GetMonsterProtoType(char* szName)
{
	for (size_t nIndex = 0; nIndex < m_nMonsterNum; nIndex++)
	{
		if (0 == strncmp(szName, m_ProtoTypeArray[nIndex].m_MonsterInfo.m_strName, MonsterInfo::MAX_NAME_LEN))
		{
			return m_ProtoTypeArray + nIndex;
		}
	}

	return NULL;
}


bool CMonsterMgr::LoadMonstersFromBinary(const char* szFileNameBinary)
{
	HANDLE hFile = CreateFile((0 == szFileNameBinary) ? m_szMonsterScriptFileName : szFileNameBinary,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
 
	if (hFile == INVALID_HANDLE_VALUE) { return false; } 

    CEnsureCloseHandle readFile(hFile);

	unsigned long dwRead = 0;
	unsigned long dwFileHighSize = 0;
	unsigned long dwFileSize = GetFileSize(hFile, &dwFileHighSize);

	char* lpAllocated = new char[dwFileSize];    
    if (NULL == lpAllocated)
    {
        return false;
    }
    
    CEnsureDeleteArray<char> allocated(lpAllocated);
    
	if (!ReadFile(hFile, lpAllocated, dwFileSize, &dwRead, NULL)) 
    {
		return false;
	}

	MonsterProtoType*   lpProtoType = 0;
	MonsterInfo*        lpMonsterInfo = 0;
	CreatureStatus*     lpCreatureStatus = 0;

	unsigned long dwStructSize = 0;
	unsigned long dwHeaderSize = sizeof(unsigned long) + *reinterpret_cast<unsigned long*>(lpAllocated) + sizeof(unsigned long);
    unsigned long dwBufferSize = *reinterpret_cast<unsigned long*>(lpAllocated + dwHeaderSize - sizeof(unsigned long));

    char* lpBuffer = new char[dwBufferSize];
	if (NULL == lpBuffer)
    { 
        return false;
    }

    CEnsureDeleteArray<char> buffer(lpBuffer);

	char* lpBufferStartPointer = lpBuffer;
	
	DECOMPRESS(lpAllocated + dwHeaderSize, dwFileSize - dwHeaderSize, lpBuffer, &dwBufferSize);
    DECODEHEADER(lpBuffer, dwBufferSize, 3, 2);

    m_nMonsterNum = dwBufferSize / (sizeof(MonsterInfo) + sizeof(CreatureStatus));
	m_ProtoTypeArray = new MonsterProtoType[m_nMonsterNum];
	if (NULL == m_ProtoTypeArray)
    {
        return false;
    }

    for (size_t nIndex = 0; nIndex < m_nMonsterNum; ++nIndex)
    {
		lpMonsterInfo       = reinterpret_cast<MonsterInfo*>(lpBuffer);
		lpCreatureStatus    = reinterpret_cast<CreatureStatus*>(lpBuffer + sizeof(MonsterInfo));

		m_ProtoTypeArray[nIndex].m_MonsterInfo = MonsterInfo(*lpMonsterInfo);
		dwBufferSize -= sizeof(MonsterInfo);
        lpBuffer += sizeof(MonsterInfo);

		m_ProtoTypeArray[nIndex].m_CreatureStatus = CreatureStatus(*lpCreatureStatus);
		dwBufferSize -= sizeof(CreatureStatus);
		lpBuffer += sizeof(CreatureStatus);		
    }

	return true;
}


bool CMonsterMgr::SaveMonstersToBinary(const char* szFileNameBinary, const char* szTrashFile)
{
	if (0 == m_ProtoTypeArray)
    {
        return false;        
    }

	HANDLE hFile = CreateFile((0 == szFileNameBinary) ? m_szMonsterScriptFileName : szFileNameBinary,
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { return false; }

    CEnsureCloseHandle writeFile(hFile);

    const size_t MAX_SCRIPT_FILE_SIZE = m_nMonsterNum * (sizeof(MonsterInfo) + sizeof(CreatureStatus));

	char *pInputBuffer  = new char[MAX_SCRIPT_FILE_SIZE];
	char *pOutputBuffer = new char[MAX_SCRIPT_FILE_SIZE];

    CEnsureDeleteArray<char> input(pInputBuffer);
    CEnsureDeleteArray<char> output(pOutputBuffer);
    
    if (0 == pInputBuffer || 0 == pOutputBuffer)
    {
		return false;
	}

	char *InputStartPointer = pInputBuffer;
	char *OutputStartPointer = pOutputBuffer;

	unsigned long dwInputBufferSize = 0;
	unsigned long dwOutputBufferSize = 0;

	for (size_t nCount = 0; nCount < m_nMonsterNum; ++nCount)
    {
		memcpy(pInputBuffer, &m_ProtoTypeArray[nCount].m_MonsterInfo, sizeof(MonsterInfo));
		dwInputBufferSize += sizeof(MonsterInfo);		
		pInputBuffer += sizeof(MonsterInfo);

		memcpy(pInputBuffer, &m_ProtoTypeArray[nCount].m_CreatureStatus, sizeof(CreatureStatus));
		dwInputBufferSize += sizeof(CreatureStatus);	
		pInputBuffer += sizeof(CreatureStatus);
	}

    ENCODEHEADER(InputStartPointer, dwInputBufferSize, 3, 2);
    COMPRESS(InputStartPointer, dwInputBufferSize, pOutputBuffer, &dwOutputBufferSize);

	unsigned long dwWritten = 0;

	// ������(����) �ڷ�
	HANDLE hTrashFile = CreateFile(szTrashFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hTrashFile == INVALID_HANDLE_VALUE) 
	{ 
		ERRLOG1(g_Log, "%s ������ �� �� �����ϴ�.", szTrashFile);
		return false; 
	} 

    CEnsureCloseHandle trashFile(hTrashFile);

	unsigned long dwRead            = 0;
	unsigned long dwFileHighSize    = 0;
	unsigned long dwFileSize        = GetFileSize(hTrashFile, &dwFileHighSize);

	char* lpAllocated = new char[dwFileSize];
    if (NULL == lpAllocated)
    {
		ERRLOG0(g_Log, "�޸� �Ҵ翡 �����Ͽ����ϴ�.");
        return false;
    }

    CEnsureDeleteArray<char> allocated(lpAllocated);

	if (false == ReadFile(hTrashFile, lpAllocated, dwFileSize, &dwRead, NULL)) 
    {
		ERRLOG0(g_Log, "������ ������ ���� �� �����ϴ�.");
		return false;
	}

	WriteFile(hFile, &dwFileSize, sizeof(unsigned long), &dwWritten, 0);
	WriteFile(hFile, lpAllocated, dwFileSize, &dwWritten, 0);

	// �ùٸ� �ڷ�
	WriteFile(hFile, &dwInputBufferSize, sizeof(unsigned long), &dwWritten, 0);
	WriteFile(hFile, pOutputBuffer, dwOutputBufferSize, &dwWritten, 0);
	return true;
}
