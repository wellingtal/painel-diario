#ifndef _PARSE_AUTH_SERVER_PACKET_H_
#define _PARSE_AUTH_SERVER_PACKET_H_

// ���� ����
struct PktBase;

#include <ctime>
#include <DB/DBDefine.h>

namespace ParsePacket
{
    // AuthServer
    unsigned long HandleAuthAccount(PktBase* lpPktBase, unsigned long& UserID_Out, 
        unsigned char& cAgentServerType_Out, unsigned char& cFirstLogin_Out);

	// WORK_LIST 2.1 ���� ���� �߰�
    unsigned long HandleUserLogin(PktBase* lpPktBase, unsigned long *SessionID_Out, unsigned long *UserID_Out, CHAR_VIEW* CharRecode_Out,
		sGuildData* CharGuildRecode_Out, unsigned char* cAccountNation, time_t* currentTime, int* nPlayTime_Out,
		unsigned short* usAdminLevel_Out, unsigned char* cBillingType_Out, unsigned char* cBillingUser_Out, unsigned char* cLoginType_Out,
		unsigned long* dwTabFlag);

	unsigned long HandleCharSelect(PktBase* lpPktBase, unsigned char *Zone_Out, unsigned short *lpChannelNum_Out);
    unsigned long HandleCharCreate(PktBase* lpPktBase, unsigned long *CharID_Out, unsigned long *SlotNum_Out, CHAR_VIEW* lpCharView_Out);
    unsigned long HandleCharDelete(PktBase* lpPktBase, unsigned long *UserID_Out, unsigned long *SlotNum_Out);

    // WORK_LIST 2.1 ���� ���� �߰�
    unsigned long HandleSelectAccountNation(PktBase* lpPktBase, unsigned long *UserID_Out, unsigned char *cType, unsigned char *cAccountNation_Out);

    // WORK_LIST 2.3 ���� ���� ���� ��� ����
	unsigned long HandleNationChangeResult(PktBase* lpPktBase, unsigned long *UserID_Out, unsigned long* aryGID_Out, unsigned long* aryFame_Out);

    // �������� ĳ���� ����
    unsigned long HandleUnifiedCharInfo(PktBase* lpPktBase, 
        unsigned char& cRestrictedPart1ToPart2Level, unsigned char& cRemainCharTransferCount,
        USER_INFO& userInfo,
        UnifiedStoreInfo** lppUnifiedStoreInfo, unsigned long& dwStoreInfoNum_Out,
        UnifiedCharData** lppUnifiedCharData, unsigned long& dwCharDataNum_Out);

    // �������� ĳ���� ���� Ack - ���� / ���� �ڵ尪�� �޴´�.
    unsigned long HandleUnifiedCharSelectAck(PktBase* lpPktBase);
};


#endif