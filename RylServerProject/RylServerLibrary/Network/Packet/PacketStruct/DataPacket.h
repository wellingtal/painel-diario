#ifndef _DATA_PACKET_H_
#define _DATA_PACKET_H_

#include <Network/Packet/PacketBase.h>

#pragma pack(1)

typedef struct PktDD* LPPktDD;
struct PktDD : public PktBase 
{
	unsigned long   m_dwRequestKey;
	unsigned short	m_wCmd;

    enum SubCommands
    {
        SCmdUserLogin			= 0x0001,	// ���� �α���
        SCmdUserLogout			= 0x0002,	// ���� �α���
        SCmdUserMove			= 0x0003,	// ���� �� �̵�
		// WORK_LIST 2.1 ���� ���� �߰�
		SCmdSelectNation		= 0x0004,	// ���� ���� ����
		// WORK_LIST 2.3 ���� ���� ���� ��� ����
		SCmdNationChangeResult	= 0x0005,

        SCmdCharCreate			= 0x0021,	// ĳ���� ����
        SCmdCharDelete			= 0x0022,	// ĳ���� ����
        SCmdCharCreateItem		= 0x0023,	// ĳ���� ���� ������
        SCmdCharSelect			= 0x0024,	// ĳ���� ����
        
        SCmdCreateParty			= 0x0031,	// ��Ƽ ����
        SCmdDeleteParty			= 0x0032,	// ��Ƽ ����
        SCmdGetPartyInfo		= 0x0033,	// ��Ƽ ���� ���
        SCmdInsertPartyMem		= 0x0034,	// ��Ƽ ��� �߰�
        SCmdDeletePartyMem		= 0x0035,	// ��Ƽ ��� ����
        SCmdLoginPartyMem		= 0x0036,	// ��Ƽ ��� �α���
        SCmdLogoutPartyMem		= 0x0037,	// ��Ƽ ��� �α׾ƿ�
        SCmdTransferLeader		= 0x0038,	// ��Ƽ ���� �絵
		SCmdAutoRoutingOn		= 0x0039,	// ��Ƽ ������� �ѱ�
		SCmdAutoRoutingOff		= 0x003A,	// ��Ƽ ������� ����
		//SCmdPartyAddress		= 0x003B,	// ��Ƽ�� UDP �ּ� ������Ʈ
		SCmdDeadPartyMem		= 0x003C,	// ��Ƽ�� ���
        SCmdMoveZonePartyMem    = 0x003D,   // ��Ƽ�� ���̵�        : RefID = �̵��� �� ID
        SCmdLevelUpPartyMem     = 0x003E,   // ��Ƽ�� ������        : RefID = �������� ����
        SCmdChangeClassPartyMem = 0x003F,   // ��Ƽ�� Ŭ��������    : RefID = �ٲ� Ŭ����
		SCmdChangeGuildPartyMem	= 0x0040,	// ��Ƽ�� ��� �����	: RefID = �ٲ� ��� ID
		
        SCmdStartSession		= 0x0050,	// ���� ����
        SCmdServerZone			= 0x0051,	// ���� ��
        SCmdCharMoveZone		= 0x0052,	// ĳ���� ���̵�
        SCmdServerZoneEnd       = 0x0053	// ���� �� ��
    };

	inline void InitPtSubCmd(unsigned long dwRequestKey, unsigned short Cmd_In);
};

inline void PktDD::InitPtSubCmd(unsigned long dwRequestKey, unsigned short Cmd_In)
{
	m_dwRequestKey  = dwRequestKey;
	m_wCmd          = Cmd_In;
}

#pragma pack()

#endif