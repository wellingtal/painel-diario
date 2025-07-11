#ifndef _CUID_AGENT_DISPATCH_H_
#define _CUID_AGENT_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>

// 패킷 전방 참조
struct PktSL;
struct PktUUT;
struct PktUK;
struct PktBase;

class CUserNode;

class CUIDAgentDispatch : public CRylServerDispatch
{
public:

	static CMultiDispatch& GetDispatchTable()
	{
		static CMultiDispatch multiDispatchTable;
		return multiDispatchTable;
	}

	CUIDAgentDispatch(CSession& Session);

	virtual void Connected();
    virtual void Disconnected();    

	IN_ADDR&	GetAddress(void)			{	return m_Address;		}
	char		GetGroup(void)				{	return m_Group;			}

	void InitUnitNum(void)		{	m_nUnitNum = 0;			}
	int	GetUnitNum(void) const	{	return m_nUnitNum;		}
	int	IncreaseUnitNum(void)	{	return ++m_nUnitNum;	}
	int	DecreaseUnitNum(void)	{	return --m_nUnitNum;	}

	void InitSucceedNum(void)		{	m_nSucceedNum = 0;			}	

	int	GetSucceedNum(void) const	{	return m_nSucceedNum;		}
	int	IncreaseSucceedNum(void)	{	return ++m_nSucceedNum;		}
	int	DecreaseSucceedNum(void)	{	return --m_nSucceedNum;		}

	void InitFailNum(void)		{	m_nFailNum = 0;			}	

	int	GetFailNum(void) const	{	return m_nFailNum;		}
	int	IncreaseFailNum(void)	{	return ++m_nFailNum;	}
	int	DecreaseFailNum(void)	{	return --m_nFailNum;	}

private:

    bool ProcessBillingHan(PktUUT* lpPktUUT);
    bool ProcessBillingYouxiLand(PktUUT* lpPktUUT);
//    bool ProcessBillingGammania(PktUUT* lpPktUUT);
    bool ProcessBillingROWGlobal(PktUUT* lpPktUUT);
    bool ProcessBillingJapan(PktUUT* lpPktUUT);
    bool ProcessBillingHanUnited(PktUUT* lpPktUUT);

    bool ProcessDisconnectHan();
    bool ProcessDisconnectYouxiLand();
//    bool ProcessDisconnectGammania();
    bool ProcessDisconnectROWGlobal();
    bool ProcessDisconnectJapan();
    bool ProcessDisconnectHanUnited();

    virtual bool DispatchPacket(PktBase* lpPktBase);

    // Packet Dispatch Functions
	bool ParseServerLogin(PktSL* lpPktSL);
    bool ParseServerLogout(PktBase* lpPktBase);
	bool ParseUpdateUIDTable(PktUUT* lpPktUUT);

    // Send Functions
	bool SendUserKill(char Group, unsigned long UserID);

	IN_ADDR		m_Address;
	char		m_Group;
    bool        m_bDisconnected;

	int         m_nUnitNum;
	int         m_nSucceedNum;
	int         m_nFailNum;
};

#endif