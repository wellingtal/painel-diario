#ifndef _DEPOSIT_CONTAINER_H_
#define _DEPOSIT_CONTAINER_H_

#include <climits>
#include "ItemContainer.h"
#include <Network/Packet/PacketStruct/CharItemPacket.h>

// ���� ����
class CSendStream;
class CCharacter;

namespace Item
{
    class CDepositContainer : public CItemContainer
    {
    public:
        
        CDepositContainer();
        virtual ~CDepositContainer();
        bool Initialize(CCharacter* lpCharacter,
            unsigned char nXSize, unsigned char nYSize, unsigned char nTabNum);

        // ������ ������ �Լ�
        virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
        virtual bool RemoveItem(ItemPos itemPos);

        // ���� ���� �Լ�
        inline bool Login(const char* szPassword, size_t nPasswordLength, char bSavePassword);
        inline bool IsLogin() { return m_bLoginSuccess; }
        inline void Logout() { if(m_bLoginSuccess) { m_bLoginSuccess = false; } }
        
        inline bool IsPasswordSaved() { return (0 != (m_dwTabFlag & PktDepositUpdateDB::SAVED_PASSWORD)); }
        inline void SetPassword(const char* szPassword, size_t nPasswordLength);
        bool ChangePassword(const char* szCurrentPassword, size_t nCurrentPasswordLength,
            const char* szChangePassword, size_t nChangePasswordLength);

        // NULL-Terminated�� �ƴ�!
        inline const unsigned char* GetPassword() const { return m_szPassword; }
        
        // �� ���� �Լ�
        inline unsigned long GetGold() const { return m_dwGold; }
        bool AddGold(unsigned long dwGold);
        bool DeductGold(unsigned long dwGold);

        // Flag���� �Լ�. Tab1�� �׻� ��� ����.
        inline void SetTabFlag(unsigned long dwTabFlag);
        inline unsigned long GetTabFlag() const { return m_dwTabFlag; }

        // �� ����.
        inline bool BuyTab(unsigned char cTabNum);
       
        // �� ���� ������ �Լ�
        inline CItemContainer* GetTab(unsigned char nTabNum);
        inline unsigned char GetMaxTabNum() const { return m_nTabNum; }

        // Serialize In.
        virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In);

        // ���� ���
        virtual void DumpItemInfo();

        // â�� ������ ������Ʈ
        bool ClientUpdate(CSendStream& ClientSendStream);        
        bool DBUpdate(CSendStream& AgentSendStream);
        bool LogUpdate(char* szLogBuffer_Out, unsigned long& dwBufferSize_InOut);

    private:

        bool Update(CSendStream& SendStream);
        
        void DestroyDepositContainer();

        // Serialize Out�� �� ���� ��, �Ǻ��� SerializeOut�Ѵ�.
        virtual bool SerializeOut(char* szItemBuffer_Out, unsigned long& dwBufferSize_InOut) const { return false; }
                        
        CArrayContainer*    m_lpArrayContainer;
        CCharacter*         m_lpOwner;

        unsigned long       m_dwTabFlag;
        unsigned long       m_dwGold;

        unsigned char       m_szPassword[Deposit::PASSWORD_LENGTH];

        unsigned char       m_nXSize;
        unsigned char       m_nYSize;
        unsigned char       m_nTabNum;
        bool                m_bLoginSuccess;
    };
};

Item::CItemContainer* Item::CDepositContainer::GetTab(unsigned char nTabNum)
{
    return (nTabNum < m_nTabNum && NULL != m_lpArrayContainer) ? 
        &m_lpArrayContainer[nTabNum] : NULL;
}


inline void Item::CDepositContainer::SetTabFlag(unsigned long dwTabFlag)
{
    m_dwTabFlag = dwTabFlag | PktDepositUpdateDB::ITEM_TAB1;

    if(0 == memcmp(m_szPassword, "0000", Deposit::PASSWORD_LENGTH) && 0 == (m_dwTabFlag & PktDepositUpdateDB::USED_DEPOSIT))
    {
        m_dwTabFlag &= ~PktDepositUpdateDB::USED_DEPOSIT;
    }
    else
    {
        m_dwTabFlag |= PktDepositUpdateDB::USED_DEPOSIT;        
    }
}

inline void Item::CDepositContainer::SetPassword(const char* szPassword, size_t nPasswordLength)
{
    memcpy(m_szPassword, szPassword, 
        (Deposit::PASSWORD_LENGTH < nPasswordLength) ? Deposit::PASSWORD_LENGTH : nPasswordLength);
}

inline bool Item::CDepositContainer::Login(const char* szPassword, size_t nPasswordLength, char bSavePassword)
{
    if(0 == memcmp(m_szPassword, szPassword, 
        (Deposit::PASSWORD_LENGTH < nPasswordLength) ? Deposit::PASSWORD_LENGTH : nPasswordLength))
    {
        m_bLoginSuccess = true;
        if(0 != bSavePassword)
        {
            m_dwTabFlag |= PktDepositUpdateDB::SAVED_PASSWORD;
        }

        m_dwTabFlag |= PktDepositUpdateDB::USED_DEPOSIT;
    }

    return m_bLoginSuccess;
}

inline bool Item::CDepositContainer::BuyTab(unsigned char cTabNum)
{
    if(cTabNum < m_nTabNum) 
    {
        m_dwTabFlag |= (1 << cTabNum);
        return true;
    }
    
    return false;
}


#endif