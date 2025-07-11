#ifndef _CAUTH_DISPATCH_H_
#define _CAUTH_DISPATCH_H_

#include <RylServerLibrary/Network/Dispatch/RylServerDispatch.h>

// 패킷 전방 참조
struct PktSL;
struct PktSVU;
struct PktSCInfo;
  

class CLoginAgentDispatch : public CRylServerDispatch
{
public:

    CLoginAgentDispatch(CSession& Session);
	~CLoginAgentDispatch();
    
    virtual void Disconnected();    

private:

    virtual bool DispatchPacket(PktBase* lpPktBase);

    // Packet Dispatch Functions
	bool ParseSysServerLogin(PktSL* lpPktSLPt);
	bool ParseSysServerVerUpdate(PktSVU* lpSVUPt);
	bool ParseSysChannelUpdate(PktSCInfo* lpPktSCInfoPt);    

    unsigned long m_dwServerID;
};

#endif