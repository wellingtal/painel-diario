#ifndef _AUTH_AGENT_DISPATCH_H_
#define _AUTH_AGENT_DISPATCH_H_

#include <Network/Dispatch/MultiThreadDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>
#include "AuthClientDispatch.h"

// 전방 참조
struct PktSLAck;
struct PktSCInfo;
struct PktSVU;
struct PktDD;
struct PktUK;
struct PktBTN;
struct PktUnifiedCharInfo;
struct PktUnifiedCharSelectAck;


class CAuthAgentDispatch : public CRylServerDispatch
{
public:
	
	static CSingleDispatch& GetDispatchTable();

	CAuthAgentDispatch(CSession& Session);
	virtual ~CAuthAgentDispatch();	

	virtual void Connected();  
    virtual void Disconnected();
   
	static bool SendStartSession(CAuthClientDispatch* lpDispatch_In, 
        unsigned long dwSessionID_In, const char* AccountName_In, const char* Password_In, 
        unsigned long dwUserID_In, unsigned short usCnFlag_In);

private:

	// Send Functions
	bool SendUserKill(unsigned long dwUserID, unsigned short usError);
	bool SendSysServerLogin(unsigned long dwServer_In);
    bool SendCreateCharEquipItem(unsigned long CharID_In, unsigned short Shirt_In, 
		unsigned short Armour_In, unsigned short Boots_In, unsigned short Weapon_In);

    virtual bool DispatchPacket(PktBase* lpPktBase);

    // Packet Dispatch Functions;
    bool ParseSysServerLogin(PktSLAck* lpPktSLAck);
    bool ParseSysServerVerUpdate(PktSVU* lpPktSVU);
    bool ParseSysChannelUpdate(PktSCInfo* lpPktSCInfo);

    bool ParseDBGetData(PktDD* lpPktDD);
	bool ParseAgentZone(PktDD* lpPktDD);

	bool ParseUserKill(PktUK* lpPktUK);
    bool ParseBillingTimeoutNotify(PktBTN* lpPktBTN);
	bool ParseBillingTimeCheckNotify(PktBTN* lpPktBTN);

    bool ParseUnifiedCharInfo(PktUnifiedCharInfo* lpPktUnifiedCharInfo);
    bool ParseUnifiedCharSelectAck(PktUnifiedCharSelectAck* lpPktUnifiedCharSelectAck);
};


#endif