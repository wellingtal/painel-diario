
#include "stdafx.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>

#include <Creature/Character/Character.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>
#include "DepositContainer.h"


Item::CDepositContainer::CDepositContainer()
:   m_lpArrayContainer(NULL),
    m_lpOwner(NULL),
    m_dwTabFlag(0),
    m_dwGold(0),
    m_nXSize(0),
    m_nYSize(0),
    m_nTabNum(0),
    m_bLoginSuccess(false)
{
    memset(m_szPassword, 0, Deposit::PASSWORD_LENGTH);
}


Item::CDepositContainer::~CDepositContainer()
{
    DestroyDepositContainer();
}


bool Item::CDepositContainer::Initialize(CCharacter* lpCharacter, unsigned char nXSize, 
                                         unsigned char nYSize, unsigned char nTabNum)
{
    m_lpOwner           = lpCharacter;
    m_dwCID             = lpCharacter->GetCID();

    m_nXSize            = nXSize;
    m_nYSize            = nYSize;
    m_nTabNum           = nTabNum;

    m_lpArrayContainer  = new Item::CArrayContainer[nTabNum];

    if(NULL != m_lpArrayContainer)
    {
        for(int nCount = 0; nCount < m_nTabNum; ++nCount)
        {
            if(!m_lpArrayContainer[nCount].Initialize(m_dwCID, nXSize, nYSize, 1))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}


void Item::CDepositContainer::DestroyDepositContainer()
{
    if(NULL != m_lpArrayContainer)
    {
        delete [] m_lpArrayContainer;
        m_lpArrayContainer = NULL;
    }
}


Item::CItem* Item::CDepositContainer::GetItem(ItemPos itemPos)
{
    unsigned char cTabNum = itemPos.GetZIndex();

    if(cTabNum < m_nTabNum && NULL != m_lpArrayContainer)
    {
        itemPos.SetZIndex(0);
        return m_lpArrayContainer[cTabNum].GetItem(itemPos);
    }

    return NULL;
}


bool Item::CDepositContainer::SetItem(ItemPos itemPos, CItem* lpItem)
{
    unsigned char cTabNum = itemPos.GetZIndex();

    bool bResult = false;

    if(NULL != m_lpArrayContainer && NULL != lpItem &&
        cTabNum < m_nTabNum && (0 != (m_dwTabFlag & (1 << cTabNum))))
    {
        // 이미 구입한 탭에만 Setting할 수 있다.
        itemPos.SetZIndex(0);
        bResult = m_lpArrayContainer[cTabNum].SetItem(itemPos, lpItem);

        itemPos.SetZIndex(cTabNum);
        lpItem->MoveItem(itemPos);
    }
    else
    {
        ERRLOG4(g_Log, "CID:0x%10u 창고 탭에 아이템을 놓을 수 없습니다. 탭 플래그 : 0x%08x, 위치(%d, 0x%04x)",
            m_dwCID, m_dwTabFlag, itemPos.m_cPos, itemPos.m_cIndex);
    }

    return bResult;
}


bool Item::CDepositContainer::RemoveItem(ItemPos itemPos)
{
    unsigned char cTabNum = itemPos.GetZIndex();

    if(NULL != m_lpArrayContainer && 
        cTabNum < m_nTabNum && (0 != (m_dwTabFlag & (1 << cTabNum))))
    {
        // 이미 구입한 탭에만 Remove할 수 있다.
        itemPos.SetZIndex(0);
        return m_lpArrayContainer[cTabNum].RemoveItem(itemPos);
    }

    return false;
}


bool Item::CDepositContainer::SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In)
{
    size_t nBufferSize = static_cast<size_t>(dwBufferSize_In);
    size_t nUsed = 0;

    Item::CItemFactory& ItemFactory = CItemFactory::GetInstance();

    while(nBufferSize > 0)
    {
        size_t nItemSize = nBufferSize;
        Item::CItem* lpItem = ItemFactory.CreateItem(szItemBuffer_In + nUsed, nItemSize);

        if(NULL == lpItem)
        {
            nItemSize = reinterpret_cast<const Item::ItemData*>(szItemBuffer_In + nUsed)->m_cItemSize;
            ERRLOG1(g_Log, "CID:0x%10u 아이템 생성에 실패했습니다.", m_dwCID);
        }
        else if(!CDepositContainer::SetItem(lpItem->GetPos(), lpItem))
        {
            nItemSize = reinterpret_cast<const Item::ItemData*>(szItemBuffer_In + nUsed)->m_cItemSize;
            ERRLOG4(g_Log, "CID:0x%10u 아이템 놓기를 실패했습니다. (%d, 0x%x) TabFlag : 0x%08x", 
                m_dwCID, lpItem->GetPos().m_cPos, lpItem->GetPos().m_cIndex, m_dwTabFlag);
            
            DELETE_ITEM(lpItem);
        }

        nUsed += nItemSize;
        nBufferSize -= nItemSize;
    }
    
    return true;
}



bool Item::CDepositContainer::ClientUpdate(CSendStream& ClientSendStream)
{
    char szData[PktDeposit::MIN_DATA_SIZE];

    memset(szData, 0, sizeof(PktDeposit::MIN_DATA_SIZE));
    *reinterpret_cast<unsigned long*>(szData) = m_dwGold;
    *reinterpret_cast<unsigned char*>(szData + sizeof(unsigned long)) = IsPasswordSaved() ? 1 : 0;

    // 돈하고 패스워드 저장 여부 리턴
    if(GameClientSendPacket::SendCharDepositCmd(ClientSendStream, PktDeposit::SAVED_PASS, 
        szData, PktDeposit::MIN_DATA_SIZE, 0))
    {
        return Update(ClientSendStream);
    }

    return false;
}


bool Item::CDepositContainer::DBUpdate(CSendStream& AgentSendStream)
{   
    if(GameClientSendPacket::SendCharDepositGoldToDBAgent(AgentSendStream, m_lpOwner->GetUID(), m_dwGold))
    {
        return Update(AgentSendStream);
    }

    return false;
}


bool Item::CDepositContainer::Update(CSendStream& SendStream)
{
    const int MAX_ITEM_BUFFER = 8000;
    const int MAX_BUFFER = MAX_ITEM_BUFFER + sizeof(PktDepositUpdateDB);
    char szBuffer[MAX_BUFFER];

    PktDepositUpdateDB* lpPktDepositUpdateDB = reinterpret_cast<PktDepositUpdateDB*>(szBuffer);
    char* szItemBuffer = szBuffer + sizeof(PktDepositUpdateDB);

    // 헤더 세팅
    lpPktDepositUpdateDB->m_dwUID               = m_lpOwner->GetUID();
    lpPktDepositUpdateDB->m_dwCID               = m_dwCID;
    lpPktDepositUpdateDB->m_dwTabFlag           = m_dwTabFlag;        // 현재 탭의 구입 상태를 나타내는 bitset
    
    // 두 탭씩 Serialize한다.
    for(unsigned int nSerializeIndex = 0; nSerializeIndex < m_nTabNum; ++nSerializeIndex)
    {
        unsigned long dwUsedSize    = 0;
        unsigned long dwBufferSize  = MAX_ITEM_BUFFER;

        // 한번에 두탭씩 Serialize한다. 짝수탭 Serialize.
        if(m_lpArrayContainer[nSerializeIndex].SerializeOut(szItemBuffer, dwBufferSize))
        {
            dwUsedSize += dwBufferSize;
        }
        else
        {
            const Item::ItemData* lpItemData = reinterpret_cast<Item::ItemData*>(szItemBuffer);
            lpItemData->DumpInfo(m_dwCID, __FUNCTION__ " 의 첫번째 탭 업데이트에서 에러 발생");
        }

        // 홀수탭 Serialize.
        ++nSerializeIndex;

        if(nSerializeIndex < m_nTabNum)
        {
            dwBufferSize  = MAX_ITEM_BUFFER - dwUsedSize;
            if(m_lpArrayContainer[nSerializeIndex].SerializeOut(szItemBuffer + dwUsedSize, dwBufferSize))
            {
                dwUsedSize += dwBufferSize;
            }
            else
            {
                const Item::ItemData* lpItemData = reinterpret_cast<Item::ItemData*>(szItemBuffer);
                lpItemData->DumpInfo(m_dwCID, __FUNCTION__ " 의 두번째 탭 업데이트에서 에러 발생");        
            }
        }

        lpPktDepositUpdateDB->m_usDataSize      = static_cast<unsigned short>(dwUsedSize);
        lpPktDepositUpdateDB->m_cTabNum         = nSerializeIndex / 2;
        lpPktDepositUpdateDB->m_bUpdateComplete = (nSerializeIndex + 1 < m_nTabNum) ? false : true;

        if(!SendStream.WrapCompress(szBuffer, 
            static_cast<unsigned short>(sizeof(PktDepositUpdateDB) + dwUsedSize), CmdDepositUpdate, 0, 0))
        {
            return false;
        }
    }

    return true;
}

bool Item::CDepositContainer::ChangePassword(const char* szCurrentPassword, size_t nCurrentPasswordLength,
                                       const char* szChangePassword, size_t nChangePasswordLength)
{
    if(0 == memcmp(m_szPassword, szCurrentPassword, 
        (Deposit::PASSWORD_LENGTH < nCurrentPasswordLength) ? Deposit::PASSWORD_LENGTH : nCurrentPasswordLength))
    {
        SetPassword(szChangePassword, nChangePasswordLength);
        return true;
    }

    return false;
}



bool Item::CDepositContainer::AddGold(unsigned long dwGold)
{
    if(m_dwGold <= ULONG_MAX - dwGold)
    {
        m_dwGold += dwGold;
        return true;
    }

    ERRLOG2(g_Log, "CID:0x%10u 창고 돈 오버플로우가 발생했습니다. : %dGold", m_dwCID, dwGold);
    return false;
}

bool Item::CDepositContainer::DeductGold(unsigned long dwGold)
{
    if(dwGold <= m_dwGold) 
    {
        m_dwGold -= dwGold;
        return true;    
    }
    
    ERRLOG1(g_Log, "CID:0x%10u 창고 돈 언더플로우가 발생했습니다.", m_dwCID);
    return false;
}


// 정보 출력
void Item::CDepositContainer::DumpItemInfo()
{
    for(int nCount = 0; nCount < m_nTabNum; ++nCount)
    {
        m_lpArrayContainer[nCount].DumpItemInfo();
    }
}


// 창고 아이템을 로그로 남김.
bool Item::CDepositContainer::LogUpdate(char* szLogBuffer_Out, unsigned long& dwBufferSize_InOut)
{
    char* szBufferOut       = szLogBuffer_Out;
    unsigned long dwUsed    = 0;
    unsigned long dwRemain  = dwBufferSize_InOut;

    for(int nIndex = 0; nIndex < m_nTabNum; ++nIndex)
    {        
        if(!m_lpArrayContainer[nIndex].SerializeOut(szBufferOut + dwUsed, dwRemain))
        {
            ERRLOG1(g_Log, "CID:0x%10u 창고 로그 업데이트에 실패했습니다", m_dwCID);
            dwBufferSize_InOut = dwUsed;
            return false;
        }

        dwUsed += dwRemain;
        dwRemain = dwBufferSize_InOut - dwUsed;
    }

    dwBufferSize_InOut = dwUsed;
    return true;
}