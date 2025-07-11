
#include "stdafx.h"

#include <memory>
#include <algorithm>
#include <functional>

// Ŭ���̾�Ʈ������ ���̹Ƿ� include�� ����Ѵ�.
#include <Log/ServerLog.h>

#include <Network/XORCrypt/XORCrypt.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Utility/DelimitedFile.h>
#include <Utility/TokenlizedFile.h>
#include <Utility/Resource/EnsureCleanup.h>
#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>
#include <Utility/Setup/ServerSetup.h>

#include "OldItemDataParser.h"
#include "OldItemMgr.h"


#define DECODE_HEADER(Start_In, Length_In, PageVer_In, PageNum_In)  CXORCrypt::GetInstance().DecodeHeader((Start_In),(Length_In),(PageVer_In),(PageNum_In))
#define ENCODE_HEADER(Start_In, Length_In, PageVer_In, PageNum_In)  CXORCrypt::GetInstance().EncodeHeader((Start_In),(Length_In),(PageVer_In),(PageNum_In))
#define COMPRESS(In, In_len, Out, Out_len)      CMiniLZO::Compress((In), (In_len), (Out), (Out_len))
#define DECOMPRESS(In, In_len, Out, Out_len)    CMiniLZO::Decompress((In), (In_len), (Out), (Out_len))


using namespace OldItem;

const char* CItemMgr::ms_szItemScriptFileName = "./Script/Game/ItemScript.txt";
const char* CItemMgr::ms_szBGItemScriptFileName = "./Script/Game/BGItemScript.txt";
const char* CItemMgr::ms_szChemicalScriptFileName = "./Script/Game/Chemical.txt";


class CParseDelimitedData
{
public:

    CParseDelimitedData(CTokenlizedFile& TokenlizedFile) : m_TokenlizedFile(TokenlizedFile) { }

    bool operator() (OldItemDataParser::ParseDataArray& ParserArray, OldItem::ItemInfo& itemInfo)
    {
        for (OldItemDataParser::ParseDataArray::iterator itr = ParserArray.begin();
            itr != ParserArray.end(); ++itr)
        {
            const char* szValue = m_TokenlizedFile.GetStringValue(itr->m_szColumnName);

            if (NULL == szValue)
            {
                ERRLOG2(g_Log, "�����͸� �о�� �� �����ϴ�. Line:%d, ColumnName:%d",
                    m_TokenlizedFile.GetCurrentLine(), itr->m_szColumnName);
                return false;
            }

            if (false == itr->m_fnParseFunc(itemInfo, szValue))
            {
                ERRLOG2(g_Log, "�˸��� �����Ͱ� �ƴմϴ�. Line:%d, ColumnName:%d",
                    m_TokenlizedFile.GetCurrentLine(), itr->m_szColumnName);
                return false;
            }
        }

        return true;
    }

private:

    CTokenlizedFile& m_TokenlizedFile;
};


OldItem::CItemMgr& OldItem::CItemMgr::GetInstance()
{
    static CItemMgr itemMgr;
    return itemMgr;
}

CItemMgr::CItemMgr()
:	m_ItemInfoArray(NULL), m_nItemNum(0), m_ChemicalArray(NULL), m_nChemicalNum(0)
{

}

CItemMgr::~CItemMgr() 
{ 	
    DestoryItemInfo();
	DestoryChemical();
}

void CItemMgr::DestoryItemInfo(void)
{
    if (NULL != m_ItemInfoArray)
    {
        delete [] m_ItemInfoArray;
        m_ItemInfoArray = NULL;
    }

    m_nItemNum = 0;
}

void CItemMgr::DestoryChemical(void)
{
	if (NULL != m_ChemicalArray)
	{
		delete [] m_ChemicalArray;
		m_ChemicalArray = NULL;
	}

	m_nChemicalNum = 0;
}

bool CItemMgr::LoadItemProtoType(const char* szFileName)
{
    DestoryItemInfo();
    
    ItemInfo  itemInfo;
	CTokenlizedFile TokenlizedFile;	// ��ü �Ҹ��, �ڵ� Close.

	char szItemScriptName[MAX_PATH];
	if (SERVER_ID::GROUP_BATTLE_SERVER == CServerSetup::GetInstance().GetServerGroup())
	{
		strncpy(szItemScriptName, ms_szBGItemScriptFileName, MAX_PATH);
	}
	else
	{
		strncpy(szItemScriptName, ms_szItemScriptFileName, MAX_PATH);
	}

	if (false == TokenlizedFile.Open((NULL != szFileName) ? szFileName : szItemScriptName))
    {
        ERRLOG0(g_Log, "������ ��ũ��Ʈ ���� �ε��� �����߽��ϴ�.");
        return false;
    }
        
    if (false == TokenlizedFile.ReadColumn())
    {
        ERRLOG0(g_Log, "������ ��ũ��Ʈ �÷��� �о���� �� �����߽��ϴ�.");
        return false;
    }

    OldItemDataParser::ParseDataArray  DefaultItemData;
    OldItemDataParser::ParseDataArray  EquipItemData;
    OldItemDataParser::ParseDataArray  UseItemData;
    OldItemDataParser::ParseDataArray  EtcItemData;

    OldItemDataParser::SetDefaultData(DefaultItemData);
    OldItemDataParser::SetEquipData(EquipItemData);
    OldItemDataParser::SetUseItemData(UseItemData);
    OldItemDataParser::SetEtcItemData(EtcItemData);

    CItemType& ItemType = CItemType::GetInstance();
         
    std::vector<ItemInfo>     ItemInfoVector;

    CParseDelimitedData ParseData(TokenlizedFile);

    while (TokenlizedFile.ReadLine())
	{
        itemInfo.Initialize();

        if (false == ParseData(DefaultItemData, itemInfo)) 
		{ 
			return false; 
		}
        
        if (ItemType.IsCorrectItemType(CItemType::EQUIP_TYPE, 
            itemInfo.m_DetailData.m_cItemType))
        {
            // ��� ������.
            if (false == ParseData(EquipItemData, itemInfo)) 
			{ 
				return false; 
			}
            ItemType.SetEquipTypeFlags(itemInfo);
        }        
        else if (ItemType.IsCorrectItemType(CItemType::USE_ITEM_TYPE, 
            itemInfo.m_DetailData.m_cItemType))
        {
            // ��� ������.
            if (false == ParseData(UseItemData, itemInfo)) 
			{ 
				return false; 
			}
            ItemType.SetUseItemTypeFlags(itemInfo);
        }
        else
        {
            // ��Ÿ ������.
            if (false == ParseData(EtcItemData, itemInfo))  
			{ 
				return false; 
			}
            ItemType.SetEtcItemTypeFlags(itemInfo);
        }

        ItemInfoVector.push_back(itemInfo);
    }

    if (false == TokenlizedFile.IsEOF())
    {
        ERRLOG0(g_Log, "������ ��ũ��Ʈ ������ �� ���� ���ϰ� �����մϴ�.");
        return false;
    }

    std::sort(ItemInfoVector.begin(), ItemInfoVector.end());

    for (std::vector<ItemInfo>::iterator itr = ItemInfoVector.begin();
        itr != ItemInfoVector.end() - 1; ++itr)
    {
        if (itr->m_usProtoTypeID == (itr+1)->m_usProtoTypeID)
        {
            ERRLOG1(g_Log, "��ġ�� ������ ���� ID�� �ֽ��ϴ�. ����ID:%d", itr->m_usProtoTypeID);
            return false;
        }
    };

    m_nItemNum = ItemInfoVector.size();
    m_ItemInfoArray = new ItemInfo[m_nItemNum];
    if (NULL == m_ItemInfoArray)
    {
        ERRLOG0(g_Log, "������ ��ũ��Ʈ ������ ������ �� �����ϴ�. �޸𸮰� �����մϴ�.");
        return false;
    }

    std::copy(ItemInfoVector.begin(), ItemInfoVector.end(), m_ItemInfoArray);
    return true;
}


const ItemInfo* CItemMgr::GetItemInfo(unsigned short usProtoTypeID)
{
    ItemInfo* lpFirst = m_ItemInfoArray;
    ItemInfo* lpLast  = m_ItemInfoArray + m_nItemNum;
    ItemInfo* lpMid   = NULL;

    size_t nCount   = m_nItemNum;
    size_t nCount2  = 0;

    for (; 0 < nCount; )
    {
        nCount2 = nCount / 2;
        lpMid = lpFirst + nCount2;

        if (lpMid->m_usProtoTypeID < usProtoTypeID)
        {
            lpFirst = ++lpMid, nCount -= nCount2 + 1;
        }
        else
        {
            nCount = nCount2;
        }
    }

    return (lpFirst != lpLast && !(usProtoTypeID < lpFirst->m_usProtoTypeID)) ? lpFirst : NULL;
}

const ItemInfo* CItemMgr::GetItemInfoFromItemName(const char* szItemName)
{
    ItemInfo* lpFirst = m_ItemInfoArray;
    ItemInfo* lpLast  = m_ItemInfoArray + m_nItemNum;

    for (; lpFirst != lpLast; ++lpFirst)
    {
        if (0 == strcmp(lpFirst->m_SpriteData.m_szName, szItemName))
        {
            return lpFirst;
        }
    }

    return NULL;
}

const ItemInfo* CItemMgr::GetItemInfoFromEquipName(const char* szEquipName)
{
    ItemInfo* lpFirst = m_ItemInfoArray;
    ItemInfo* lpLast  = m_ItemInfoArray + m_nItemNum;

    for (; lpFirst != lpLast; ++lpFirst)
    {
        if (0 == strcmp(lpFirst->m_StringData.m_szEquipModelName, szEquipName))
        {
            return lpFirst;
        }
    }

    return NULL;
}

const char* CItemMgr::GetEquipName(const unsigned short usItemProtoTypeID)
{
	const ItemInfo* pItemInfo = GetItemInfo(usItemProtoTypeID);

	if(pItemInfo)
	{
		return pItemInfo->m_StringData.m_szEquipModelName;
	}

	return NULL;
}

const unsigned short CItemMgr::GetItemIDFromSkillID(const unsigned short usSkill_ID, const unsigned short unSkill_LockCount)
{
    ItemInfo* lpFirst = m_ItemInfoArray;
    ItemInfo* lpLast  = m_ItemInfoArray + m_nItemNum;

    for (; lpFirst != lpLast; ++lpFirst)
    {
        if (lpFirst->m_UseItemInfo.m_usSkill_ID == usSkill_ID && 
			lpFirst->m_UseItemInfo.m_usSkill_LockCount == unSkill_LockCount)
        {
            return lpFirst->m_usProtoTypeID;
        }
    }

    return 0;
}


bool CItemMgr::LoadItemProtoTypeBinary(const char* szFileNameBinary)
{
	if (NULL == szFileNameBinary) { return false; }

	HANDLE hFile = CreateFile(szFileNameBinary, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
 
	if (hFile == INVALID_HANDLE_VALUE) { return false; } 

    CEnsureCloseHandle EnsureCloseHandle(hFile);

	unsigned long dwRead            = 0;
	unsigned long dwFileHighSize    = 0;
	unsigned long dwFileSize        = GetFileSize(hFile, &dwFileHighSize);

    char* lpAllocated = new char[dwFileSize];
    CEnsureDeleteArray<char> EnsureFileInfo(lpAllocated);
	
	if (false == ReadFile(hFile, lpAllocated, dwFileSize, &dwRead, NULL)) 
    {        
    	return false;
	}
    
	unsigned long dwHeaderSize          = sizeof(unsigned long) + *reinterpret_cast<unsigned long *>(lpAllocated) + sizeof(unsigned long);
    unsigned long dwDecompressedSize    = *reinterpret_cast<unsigned long *>(lpAllocated + dwHeaderSize - sizeof(unsigned long));

    DestoryItemInfo();

    m_nItemNum = dwDecompressedSize / sizeof(ItemInfo);
    m_ItemInfoArray = new ItemInfo[m_nItemNum];
    if (NULL == m_ItemInfoArray)
    {
        ERRLOG0(g_Log, "������ ��ũ��Ʈ ������ �ҷ��� �� �����ϴ�. �޸𸮰� �����մϴ�.");
        return false;
    }

    DECOMPRESS(lpAllocated + dwHeaderSize, dwFileSize - dwHeaderSize, 
        reinterpret_cast<char *>(m_ItemInfoArray), &dwDecompressedSize);
    DECODE_HEADER(reinterpret_cast<char *>(m_ItemInfoArray), dwDecompressedSize, 0, 2);

	return true;
}

bool CItemMgr::SaveItemProtoTypeBinary(const char* szFileNameBinary, const char* szTrashFile)
{
	if (NULL == szFileNameBinary || NULL == szTrashFile) { return false; }

	HANDLE hFile = CreateFile(szFileNameBinary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) { return false; }
    
    CEnsureCloseHandle EnsureCloseSaveHandle(hFile);

    unsigned long   nItemInfoSize       = static_cast<unsigned long>(sizeof(ItemInfo) * m_nItemNum);
    unsigned long   nCompressedInfoSize = nItemInfoSize;
    
    char*   lpItemInfo          = new char[nItemInfoSize];
    char*   lpCompressedInfo    = new char[nItemInfoSize];

    CEnsureDeleteArray<char> EnsureDeleteItemInfo(lpItemInfo);
    CEnsureDeleteArray<char> EnsureDeleteCompressedInfo(lpCompressedInfo);

	if (NULL == lpItemInfo || NULL == lpCompressedInfo)
    {
		return false;
	}

    memcpy(lpItemInfo, m_ItemInfoArray, nItemInfoSize);
    ENCODE_HEADER(lpItemInfo, nItemInfoSize, 0, 2);
    COMPRESS(lpItemInfo, nItemInfoSize, lpCompressedInfo, &nCompressedInfoSize);

	// ������(����) �ڷ�
	HANDLE hTrashFile = CreateFile(szTrashFile, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hTrashFile == INVALID_HANDLE_VALUE) { return false; } 

    CEnsureCloseHandle EnsureCloseTrashHandle(hTrashFile);

	unsigned long dwRead = 0;
    unsigned long dwWritten = 0;
	unsigned long dwFileHighSize = 0;
	unsigned long dwTrashFileSize = GetFileSize(hTrashFile, &dwFileHighSize);

    char* lpTrashFileData = new char[dwTrashFileSize];
    CEnsureDeleteArray<char> EnsureDeleteTrashFile(lpTrashFileData);

	if (false == ReadFile(hTrashFile, lpTrashFileData, dwTrashFileSize, &dwRead, NULL))
    {
		return false;
	}

    // Trash File������
	WriteFile(hFile, &dwTrashFileSize, sizeof(unsigned long), &dwWritten, 0);	// Trash ������ ũ��
	WriteFile(hFile, lpTrashFileData, dwTrashFileSize, &dwWritten, 0);			// Trash ���� ����

	// �ùٸ� �ڷ�
	WriteFile(hFile, &nItemInfoSize, sizeof(unsigned long), &dwWritten, 0);     // ���� ������ ũ��
	WriteFile(hFile, lpCompressedInfo, nCompressedInfoSize, &dwWritten, 0);		// ����� ������

	return true;
}


bool CItemMgr::LoadItemChemical(const char* szFileName)
{
	const int	        ITEM_NAME_LEN = 32;
	char		        szBuffer[ITEM_NAME_LEN];

	int                 nLineCount  = 0;
	CDelimitedFile      DelimitedFile;      // ��ü �Ҹ��, �ڵ� Close.

	// ��ũ�ο� �α� �ڵ� ������ ���� �� ��.
	// ��ũ�ο��� \�ڿ� �����̳� ���� ���Ե��� �ʵ��� ������ ��.
	// ( '�̽������� �������� �߸��Ǿ����ϴ�' ���� �߻� )
	#define READ_DATA(ColumnName, Argument) \
		if (false == DelimitedFile.ReadData(Argument)) \
		{ \
			ERRLOG2(g_Log, "������ �ռ� ��ũ��Ʈ �б� ���� : %d�� %s�÷����� ���� �߻�!", nLineCount, #ColumnName); \
			return false; \
		}

	#define READ_STRING(ColumnName, Buffer, BufferSize) \
		if (false == DelimitedFile.ReadString(Buffer, BufferSize)) \
		{\
			ERRLOG2(g_Log, "������ �ռ� ��ũ��Ʈ �б� ���� : %d�� %s�÷����� ���� �߻�!", nLineCount, #ColumnName);\
			return false;\
		}


	if (false == DelimitedFile.Open(szFileName ? szFileName : ms_szChemicalScriptFileName, 2)) 
	{
		ERRLOG1(g_Log, "%s ������ �� �� �����ϴ�.", szFileName ? szFileName : ms_szChemicalScriptFileName);
		return false;
	}

	std::vector<ChemicalInfo>		ChemicalVector;
	ChemicalInfo					tempChemical;

	while (DelimitedFile.ReadLine()) 
	{
		++nLineCount;

		READ_STRING("PickkingItem Name", szBuffer, ITEM_NAME_LEN);
		READ_DATA("PickkingItem ID", tempChemical.m_wPickkingItemID);
		READ_DATA("PickkingItem Num", tempChemical.m_cPickkingItemNum);

		READ_STRING("TargetItem Name", szBuffer, ITEM_NAME_LEN);
		READ_DATA("TargetItem ID", tempChemical.m_wTargetItemID);
		READ_DATA("TargetItem Num", tempChemical.m_cTargetItemNum);

		READ_STRING("ResultItem Name", szBuffer, ITEM_NAME_LEN);
		READ_DATA("ResultItem ID", tempChemical.m_wResultItemID);
		READ_DATA("ResultItem Num", tempChemical.m_cResultItemNum);

		const ItemInfo* lpPickkingItem = GetItemInfo(tempChemical.m_wPickkingItemID);
		const ItemInfo* lpTargetItem = GetItemInfo(tempChemical.m_wTargetItemID);
		const ItemInfo* lpResultItem = GetItemInfo(tempChemical.m_wResultItemID);

		if (NULL == lpPickkingItem || NULL == lpTargetItem || NULL == lpResultItem)
		{
			ERRLOG3(g_Log, "�������� �ʴ� �������� �ռ����� �� �ֽ��ϴ�. "
				"����ִ� ������ : %d, �ռ���� ������ : %d, �ռ������ : %d", 
				tempChemical.m_wPickkingItemID, tempChemical.m_wTargetItemID, tempChemical.m_wResultItemID);
			return false;
		}

		if (lpTargetItem->m_DetailData.m_cXSize < lpResultItem->m_DetailData.m_cXSize || 
			lpTargetItem->m_DetailData.m_cYSize < lpResultItem->m_DetailData.m_cYSize)
		{
			ERRLOG6(g_Log, "�ռ���� �������� �ռ���������� �۽��ϴ�. "
				"�ռ���� ������ : %d(%dx%d), �ռ������ : %d(%dx%d)", 
				tempChemical.m_wTargetItemID, lpTargetItem->m_DetailData.m_cXSize, lpTargetItem->m_DetailData.m_cYSize, 
				tempChemical.m_wResultItemID, lpResultItem->m_DetailData.m_cXSize, lpResultItem->m_DetailData.m_cYSize);
			return false;
		}

		ChemicalVector.push_back(tempChemical);
	}

	std::sort(ChemicalVector.begin(), ChemicalVector.end());

	m_nChemicalNum = ChemicalVector.size();
	m_ChemicalArray = new ChemicalInfo[m_nChemicalNum];
	if (NULL == m_ChemicalArray)
	{
		ERRLOG0(g_Log, "�޸� �Ҵ翡 �����߽��ϴ�.");
		return false;
	}

	std::copy(ChemicalVector.begin(), ChemicalVector.end(), m_ChemicalArray);

	return true;
}

bool CItemMgr::LoadItemChemicalBinary(const char* szFileNameBinary)
{
	HANDLE hFile = CreateFile((0 == szFileNameBinary) ? ms_szChemicalScriptFileName : szFileNameBinary,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) { return false; } 

	CEnsureCloseHandle EnsureCloseHandle(hFile);

	unsigned long dwRead            = 0;
	unsigned long dwFileHighSize    = 0;
	unsigned long dwFileSize        = GetFileSize(hFile, &dwFileHighSize);

	char* lpAllocated = new char[dwFileSize];
	CEnsureDeleteArray<char> EnsureFileInfo(lpAllocated);

	if (false == ReadFile(hFile, lpAllocated, dwFileSize, &dwRead, NULL)) 
	{        
		return false;
	}

	unsigned long dwHeaderSize          = sizeof(unsigned long) + *reinterpret_cast<unsigned long *>(lpAllocated) + sizeof(unsigned long);
	unsigned long dwDecompressedSize    = *reinterpret_cast<unsigned long *>(lpAllocated + dwHeaderSize - sizeof(unsigned long));

	DestoryChemical();

	m_nChemicalNum = dwDecompressedSize / sizeof(ChemicalInfo);
	m_ChemicalArray = new ChemicalInfo[m_nChemicalNum];
	if (NULL == m_ChemicalArray)
	{
		ERRLOG0(g_Log, "������ �Լ� ��ũ��Ʈ ������ �ҷ��� �� �����ϴ�. �޸𸮰� �����մϴ�.");
		return false;
	}

	DECOMPRESS(lpAllocated + dwHeaderSize, dwFileSize - dwHeaderSize, 
		reinterpret_cast<char *>(m_ChemicalArray), &dwDecompressedSize);
	DECODE_HEADER(reinterpret_cast<char *>(m_ChemicalArray), dwDecompressedSize, 0, 2);

	return true;
}

bool CItemMgr::SaveItemChemicalBinary(const char* szFileNameBinary, const char* szTrashFile)
{
	HANDLE hFile = CreateFile((0 == szFileNameBinary) ? ms_szChemicalScriptFileName : szFileNameBinary,
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) { return false; }
    
    CEnsureCloseHandle EnsureCloseSaveHandle(hFile);

    unsigned long   nChemicalInfoSize	= static_cast<unsigned long>(sizeof(ChemicalInfo) * m_nChemicalNum);
    unsigned long   nCompressedInfoSize = nChemicalInfoSize;
    
    char*   lpChemicalInfo		= new char[nChemicalInfoSize];
    char*   lpCompressedInfo    = new char[nChemicalInfoSize];

    CEnsureDeleteArray<char> EnsureDeleteItemInfo(lpChemicalInfo);
    CEnsureDeleteArray<char> EnsureDeleteCompressedInfo(lpCompressedInfo);

	if (NULL == lpChemicalInfo || NULL == lpCompressedInfo)
    {
		return false;
	}

    memcpy(lpChemicalInfo, m_ChemicalArray, nChemicalInfoSize);
    ENCODE_HEADER(lpChemicalInfo, nChemicalInfoSize, 0, 2);
    COMPRESS(lpChemicalInfo, nChemicalInfoSize, lpCompressedInfo, &nCompressedInfoSize);

	// ������(����) �ڷ�
	HANDLE hTrashFile = CreateFile(szTrashFile, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hTrashFile == INVALID_HANDLE_VALUE) { return false; } 

    CEnsureCloseHandle EnsureCloseTrashHandle(hTrashFile);

	unsigned long dwRead = 0;
    unsigned long dwWritten = 0;
	unsigned long dwFileHighSize = 0;
	unsigned long dwTrashFileSize = GetFileSize(hTrashFile, &dwFileHighSize);

    char* lpTrashFileData = new char[dwTrashFileSize];
    CEnsureDeleteArray<char> EnsureDeleteTrashFile(lpTrashFileData);

	if (false == ReadFile(hTrashFile, lpTrashFileData, dwTrashFileSize, &dwRead, NULL))
    {
		return false;
	}

    // Trash File������
	WriteFile(hFile, &dwTrashFileSize, sizeof(unsigned long), &dwWritten, 0);	// Trash ������ ũ��
	WriteFile(hFile, lpTrashFileData, dwTrashFileSize, &dwWritten, 0);			// Trash ���� ����

	// �ùٸ� �ڷ�
	WriteFile(hFile, &nChemicalInfoSize, sizeof(unsigned long), &dwWritten, 0);	// ���� ������ ũ��
	WriteFile(hFile, lpCompressedInfo, nCompressedInfoSize, &dwWritten, 0);		// ����� ������

	return true;
}

class CFindChemical : public std::unary_function<ChemicalInfo, bool>
{
public:

	explicit CFindChemical(unsigned short wPickkingItemID, unsigned short wTargetItemID) 
		:	m_wPickkingItemID(wPickkingItemID), m_wTargetItemID(wTargetItemID)
	{ }		

	bool operator() (ChemicalInfo& chemical) 
	{ 
		return (m_wPickkingItemID == chemical.m_wPickkingItemID && 
			m_wTargetItemID == chemical.m_wTargetItemID); 
	}


private:

	const unsigned short m_wPickkingItemID;
	const unsigned short m_wTargetItemID;
};

unsigned char CItemMgr::GetChemicalResult(ChemicalInfo& chemical)
{
	CFindChemical findChemical(chemical.m_wPickkingItemID, chemical.m_wTargetItemID);
	const ChemicalInfo* lpChemicalRecipe = 
		std::find_if(&m_ChemicalArray[0], &m_ChemicalArray[m_nChemicalNum], findChemical);
	if (&m_ChemicalArray[m_nChemicalNum] == lpChemicalRecipe)
	{
		return Chemical::F_WRONG_RECIPE;
	}

	if (lpChemicalRecipe->m_cPickkingItemNum > chemical.m_cPickkingItemNum || 
		lpChemicalRecipe->m_cTargetItemNum > chemical.m_cTargetItemNum)
	{
		return Chemical::F_NOT_ENOUGH_ITEM;
	}

	if (0 != chemical.m_cTargetItemNum % lpChemicalRecipe->m_cTargetItemNum || 
		0 != chemical.m_cPickkingItemNum % lpChemicalRecipe->m_cPickkingItemNum)
	{
		return Chemical::F_WRONG_RATE;
	}

	chemical.m_wResultItemID = lpChemicalRecipe->m_wResultItemID;

	unsigned char cPickkingItemUse = chemical.m_cPickkingItemNum / lpChemicalRecipe->m_cPickkingItemNum;
	unsigned char cTargetItemUse = chemical.m_cTargetItemNum / lpChemicalRecipe->m_cTargetItemNum;

	if (cPickkingItemUse >= cTargetItemUse)
	{
		chemical.m_cPickkingItemNum = chemical.m_cPickkingItemNum - cTargetItemUse * lpChemicalRecipe->m_cPickkingItemNum;
		chemical.m_cTargetItemNum = 0;

		chemical.m_cResultItemNum = cTargetItemUse * lpChemicalRecipe->m_cResultItemNum;
	}
	else
	{
		chemical.m_cPickkingItemNum = 0;
		chemical.m_cTargetItemNum = chemical.m_cTargetItemNum - cPickkingItemUse * lpChemicalRecipe->m_cTargetItemNum;

		chemical.m_cResultItemNum = cPickkingItemUse * lpChemicalRecipe->m_cResultItemNum;
	}

	return Chemical::S_SUCCESS;
}
