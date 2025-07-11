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
        int     m_nEndTime;         // 무료, 정액 : 20030225, 정량 : RemainMin
        int     m_nCRMIndex;        // 개인은 0, PC방은 0이상. 보통 개인만 들어온다.
        char    m_cBillingType;     // E : 개인정량, T : 개인정액, F : 무료사용자
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