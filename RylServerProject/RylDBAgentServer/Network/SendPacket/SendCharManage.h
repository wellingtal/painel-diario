#ifndef _DBAGENT_SERVER_SEND_CHAR_MANAGE_H_
#define _DBAGENT_SERVER_SEND_CHAR_MANAGE_H_

#include <DB/DBdefine.h>
#include <Item/Item.h>

// forward decl.
class CSendStream;
struct USER_INFO;
struct CHAR_VIEW;
struct sGuildData;
struct UnifiedStoreInfo;
struct UnifiedCharData;

namespace DBAgent
{
    namespace DataStorage
    {
        // forward delc.
        class CSessionData;
    }

    namespace SendPacket
    {
		// WORK_LIST 2.1 ���� ���� �߰�
        // ĳ���� �� ��� ����
        bool UserLogin(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID, 
                    const CHAR_VIEW* lpCharView, const sGuildData* lpGuildData, const STORE_INFO* pStoreInfo, unsigned long dwCharViewNum, 
					unsigned char cAccountNation, int nPlayTime, unsigned char cAdminLevel, unsigned char cBillingUser,
                    unsigned char cBillingType, unsigned char cLoginType, unsigned short usError);

        // ĳ���� ����
        bool CharSelect(CSendStream& SendStream, unsigned long dwRequestKey, 
                        unsigned char cZone, unsigned short usError);

        // ĳ���� ����
        bool CharCreate(CSendStream& SendStream, unsigned long dwRequestKey, 
            unsigned long dwCID, unsigned long dwSlot, const CHAR_VIEW& CharView, unsigned short usError);

        // ĳ���� ����
        bool CharDelete(CSendStream& SendStream, unsigned long dwRequestKey, 
            unsigned long dwUID, unsigned long dwSlot, unsigned long dwCID, unsigned short usError);

        // ������ ����
        bool CharCreateItem(CSendStream& SendStream, unsigned long dwRequestKey, 
                            unsigned char cZone, unsigned short usError);

		// WORK_LIST 2.1 ���� ���� �߰�
		// ���� ���� ���� �� ����
		bool SelectAccountNation(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID,
								 unsigned char cType, unsigned char cAccountNation, unsigned short usError);

        // WORK_LIST 2.3 ���� ���� ���� ��� ����
		bool NationChangeResult(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID,
								unsigned long* aryGID, unsigned long* aryFame);

        // �������� ĳ���� ���� ����. ��� ĳ���͸� ������ �� �ִ� ���սÿ��� �����͸� �������� �ʴ´�.
        bool UnifiedCharInfo(CSendStream& SendStream, 
            unsigned long dwUID, unsigned char cTransferedCharCount, const USER_INFO& userInfo,
            const UnifiedStoreInfo* lpUnifiedStoreInfo, unsigned long dwUnifiedStoreNum,
            const UnifiedCharData* lpUnifiedCharData, unsigned long dwUnifiedCharNum);

        // Part1 DB�߰輭���� �����͸� �����Ѵ�.
        bool UnifiedCharSelectReq(CSendStream& SendStream, unsigned long dwUID, 
            unsigned long dwRequestKey, unsigned long* lpdwCID, unsigned long dwCIDNum, 
            unsigned char cSelectedStoreServerGroup, unsigned char cSelectedNation);

        // ���������� ĳ���� ���� ����/���и� �ش�.
        bool UnifiedCharSelectAck(CSendStream& SendStream, unsigned long dwUID, 
            unsigned long dwRequestKey, unsigned short usError);

		// ĳ���������� ���
		bool CharUseCashItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
			Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError);

		// ����Ʈ�� �̺�Ʈ ����
		bool CharExtraEvent(CSendStream& SendStream, unsigned long dwUID, unsigned long dwCID,
			unsigned long dwEventID, unsigned long dwValue1, unsigned long dwValue2, unsigned short usError);
    }
}


#endif