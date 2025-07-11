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

        //! ������ �����Ѵ�.
        void StartBilling(int nPlayTime, 
            unsigned long dwCRMIndex1, unsigned char cBillingType);

        void ClearBilling();

        //! ��÷��� ����Ʈ�� �߰��Ѵ�
        void AddYouxiLandPoint(int nAdditionalPoint, unsigned char cBillingType);

        //! true : ���� ���� ���� / false : ���� ����� ��.
        bool CheckBilling(const CSessionData& SessionData, time_t nCurrentTime);

    private:

        time_t          m_nLoginTime;       // �α��� �ð�(midnight, January 1, 1970 ���� �帥 �ð�(sec))

        int             m_nPlayTime;		// ���� �ð� (��¥ Ȥ�� �ð�)
        unsigned long   m_dwCRMIndex1;  	// PC�� ID (0�̸� ��������)
        WarningType     m_eWarnMsgType;    // �ؿ� ���޽��� Ÿ�� ( 0 : 1������ ����� - �븸 ��, 1 : ��� ����, 2 : ��� ���� )

        char			m_cBillingType;		// ���� Ÿ�� (D : ���� ��¥, T : ���� �ð�)
        char            m_cFlags;           // ���� �÷���.
    };
}
}

#endif