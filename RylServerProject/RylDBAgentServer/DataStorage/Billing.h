#ifndef _RYL_DBAGENT_BILLING_DATA_H_
#define _RYL_DBAGENT_BILLING_DATA_H_

#include <ctime>

// forward decl;
class CSendStream;

namespace DBAgent
{
namespace DataStorage
{
    // forward decl;
    class CSessionData;

    class CBilling
    {
    public:

        CBilling();
        ~CBilling();

        enum WarningType
        {
            WARN_BEFORE_1SEC = 0,
            NO_WARNING = 1,
            WARN_EVERY_MINUTE = 2
        };


        time_t          GetLoginTime() const   { return m_nLoginTime;    }
        int             GetPlayTime() const    { return m_nPlayTime;     }
        unsigned long   GetCRMData() const     { return m_dwCRMIndex1;   }
        WarningType     GetWarnMsgType() const { return m_eWarnMsgType; }
        char            GetBillingType() const { return m_cBillingType;  }

        void SetWarnMsgType(WarningType eWarnMsgType) { m_eWarnMsgType = eWarnMsgType; }

        //! 빌링을 시작한다.
        void StartBilling(int nPlayTime, 
            unsigned long dwCRMIndex1, unsigned char cBillingType);

        void ClearBilling();

        //! 요시랜드 포인트를 추가한다
        void AddYouxiLandPoint(int nAdditionalPoint, unsigned char cBillingType);

        //! true : 아직 접속 가능 / false : 접속 끊어야 함.
        bool CheckBilling(const CSessionData& SessionData, time_t nCurrentTime);

    private:

        time_t          m_nLoginTime;       // 로그인 시간(midnight, January 1, 1970 부터 흐른 시간(sec))

        int             m_nPlayTime;		// 빌링 시간 (날짜 혹은 시간)
        unsigned long   m_dwCRMIndex1;  	// PC방 ID (0이면 개인유저)
        WarningType     m_eWarnMsgType;    // 해외 경고메시지 타입 ( 0 : 1분전에 경고보냄 - 대만 등, 1 : 경고 없음, 2 : 경고 있음 )

        char			m_cBillingType;		// 빌링 타입 (D : 남은 날짜, T : 남은 시간)
        char            m_cFlags;           // 내부 플래그.
    };
}
}

#endif