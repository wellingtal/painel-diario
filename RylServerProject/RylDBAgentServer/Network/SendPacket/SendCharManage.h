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
		// WORK_LIST 2.1 계정 국적 추가
        // 캐릭터 뷰 얻어 오기
        bool UserLogin(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID, 
                    const CHAR_VIEW* lpCharView, const sGuildData* lpGuildData, const STORE_INFO* pStoreInfo, unsigned long dwCharViewNum, 
					unsigned char cAccountNation, int nPlayTime, unsigned char cAdminLevel, unsigned char cBillingUser,
                    unsigned char cBillingType, unsigned char cLoginType, unsigned short usError);

        // 캐릭터 선택
        bool CharSelect(CSendStream& SendStream, unsigned long dwRequestKey, 
                        unsigned char cZone, unsigned short usError);

        // 캐릭터 생성
        bool CharCreate(CSendStream& SendStream, unsigned long dwRequestKey, 
            unsigned long dwCID, unsigned long dwSlot, const CHAR_VIEW& CharView, unsigned short usError);

        // 캐릭터 삭제
        bool CharDelete(CSendStream& SendStream, unsigned long dwRequestKey, 
            unsigned long dwUID, unsigned long dwSlot, unsigned long dwCID, unsigned short usError);

        // 아이템 생성
        bool CharCreateItem(CSendStream& SendStream, unsigned long dwRequestKey, 
                            unsigned char cZone, unsigned short usError);

		// WORK_LIST 2.1 계정 국적 추가
		// 계정 국적 설정 및 변경
		bool SelectAccountNation(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID,
								 unsigned char cType, unsigned char cAccountNation, unsigned short usError);

        // WORK_LIST 2.3 계정 국적 변경 기능 구현
		bool NationChangeResult(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID,
								unsigned long* aryGID, unsigned long* aryFame);

        // 서버통합 캐릭터 정보 전달. 모든 캐릭터를 선택할 수 있는 통합시에는 데이터를 전달하지 않는다.
        bool UnifiedCharInfo(CSendStream& SendStream, 
            unsigned long dwUID, unsigned char cTransferedCharCount, const USER_INFO& userInfo,
            const UnifiedStoreInfo* lpUnifiedStoreInfo, unsigned long dwUnifiedStoreNum,
            const UnifiedCharData* lpUnifiedCharData, unsigned long dwUnifiedCharNum);

        // Part1 DB중계서버로 데이터를 전송한다.
        bool UnifiedCharSelectReq(CSendStream& SendStream, unsigned long dwUID, 
            unsigned long dwRequestKey, unsigned long* lpdwCID, unsigned long dwCIDNum, 
            unsigned char cSelectedStoreServerGroup, unsigned char cSelectedNation);

        // 인증서버로 캐릭터 선택 성공/실패를 준다.
        bool UnifiedCharSelectAck(CSendStream& SendStream, unsigned long dwUID, 
            unsigned long dwRequestKey, unsigned short usError);

		// 캐쉬아이템을 사용
		bool CharUseCashItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
			Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError);

		// 엑스트라 이벤트 전송
		bool CharExtraEvent(CSendStream& SendStream, unsigned long dwUID, unsigned long dwCID,
			unsigned long dwEventID, unsigned long dwValue1, unsigned long dwValue2, unsigned short usError);
    }
}


#endif