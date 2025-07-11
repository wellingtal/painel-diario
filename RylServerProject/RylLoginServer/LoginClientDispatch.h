#ifndef _CCLIENT_DISPATCH_H_
#define _CCLIENT_DISPATCH_H_

#include <mmsystem.h>
#include <RylServerLibrary/Network/Dispatch/CheckPing.h>
#include <RylServerLibrary/Network/Dispatch/RylServerDispatch.h>

// ��Ŷ ���� ����
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

	bool ParseLoginServerList(PktSvL* lpPktSvLPt);	// Ŭ���̾�Ʈ ����Ʈ, ����, ��ġ �ּ�	
	bool ParseSysPatchAddress(PktSPI* lpPktSPIPt);	// �� ���� ��

    CCheckPing  m_CheckPing;
};

#endif