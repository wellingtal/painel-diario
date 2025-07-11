#ifndef _REGULAR_AGENT_DISPATCH_H_
#define _REGULAR_AGENT_DISPATCH_H_

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>

#include <Creature/Character/TempCharacter.h>


class CRegularAgentDispatch : public CRylServerDispatch
{
public:

	enum Const
	{
		MAX_PACKET_DISPATCH_PER_PULSE   = 10,
	};

	static CMultiDispatch& GetDispatchTable(void);
	static CTempCharacterMgr& GetTempCharacterMgr(void);

	CRegularAgentDispatch(CSession& Session);
	virtual ~CRegularAgentDispatch();

	static bool Initialize(void);

	virtual void Connected(void);
	virtual void Disconnected(void);
	virtual bool DispatchPacket(PktBase* lpPktBase);

	bool ParseServerLogin(PktBase* lpPktBase);
	
	static RylServerInfo& GetAgentServerInfo(unsigned char cGroup)	{ return ms_AgentServerInfo[cGroup]; }

private:

	char			m_cGroup;

	static RylServerInfo	ms_AgentServerInfo[SERVER_ID::MAX_GROUP_NUM];
};

namespace RegularAgentPacketParse
{
	// Packet Dispatch Functions
	bool ParseGetCharSlot(PktBase* lpPktBase);
	bool ParseGetCharData(PktBase* lpPktBase, unsigned char cGroup);

	// Send Functions
	bool SendGetCharSlot(unsigned long dwCID, unsigned char cGroup);
	bool SendGetCharData(unsigned long dwUID, unsigned long dwSlotCID, 
        const char* szSlotName, unsigned long dwCID, unsigned char cGroup);
	bool SendSetCharData(unsigned long dwCID, unsigned long dwMileage, unsigned char cGroup);
}

#endif