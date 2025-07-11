#ifndef _CCLIENT_DISPATCH_H_
#define _CCLIENT_DISPATCH_H_

#include <mmsystem.h>
#include <RylServerLibrary/Network/Dispatch/CheckPing.h>
#include <RylServerLibrary/Network/Dispatch/RylServerDispatch.h>

// 패킷 전방 참조
struct PktBase;
struct PktSvL;
struct PktSG;
struct PktSPI;
struct PktAUO;

class CLoginClientDispatch : public CRylServerDispatch
{
public:

	CLoginClientDispatch(CSession& Session);
    ~CLoginClientDispatch();
    
    virtual bool Dispatch();

	CCheckPing& GetCheckPing() { return m_CheckPing; }

private:

    virtual bool DispatchPacket(PktBase* lpPktBase);

	bool ParseLoginServerList(PktSvL* lpPktSvLPt);	// 클라이언트 리스트, 버젼, 패치 주소	
	bool ParseSysPatchAddress(PktSPI* lpPktSPIPt);	// 구 런쳐 용

    CCheckPing  m_CheckPing;
};

#endif