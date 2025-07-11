#ifndef _HAN_UNITED_DISCONNECT_ID_H_
#define _HAN_UNITED_DISCONNECT_ID_H_

#include <map>

class CHanUnitedDisconnectID
{
public:

    struct DisconnectInfo
    {        
        int     m_nUID;             // 
        int     m_nServerID;        // 
        int     m_nRemainMin;       //          
        int     m_nEndTime;         // ����, ���� : 20030225, ���� : RemainMin
        int     m_nCRMIndex;        // ������ 0, PC���� 0�̻�. ���� ���θ� ���´�.
        char    m_cBillingType;     // E : ��������, T : ��������, F : ��������
    };

    static CHanUnitedDisconnectID& GetInstance();

    CHanUnitedDisconnectID();
    virtual ~CHanUnitedDisconnectID();
    
    void UpdateDisconnectInfo(const DisconnectInfo& disconnectInfo);
    bool RemoveDisconnectInfo(int nUID);

    DisconnectInfo* GetDisconnectInfo(int nUID);

    void CheckDisconnect();

private:

    typedef std::map<int, DisconnectInfo> DisconnectInfoMap;

    DisconnectInfoMap   m_DisconnectInfoMap;
};


#endif