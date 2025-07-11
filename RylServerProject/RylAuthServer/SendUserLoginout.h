#ifndef _SEND_USER_LOGINOUT_H_
#define _SEND_USER_LOGINOUT_H_

#include <winsock2.h>
#include <windows.h>

class CSession;
class CAuthClientDispatch;
struct CHAR_VIEW;
struct sGuildData;

class CSendStream;

namespace AuthSendPacket
{
    bool SendAuthAccount(CAuthClientDispatch* lpDispatch, 
        unsigned long UserID_In, unsigned short usErrorCode_In);

	// WORK_LIST 2.1 계정 국적 추가
    bool SendUserLogin(CSendStream& SendStream, unsigned long SessionID_In, unsigned long UserID_In, CHAR_VIEW* lpCharView_In, 
						sGuildData* lpGuildData_In, unsigned long dwFlag, unsigned char cAccountNation, int nPlayTime, unsigned short usAdminLevel,
						unsigned char cBillingType,	unsigned char cBillingUser, unsigned char cLoginType, unsigned short usError_In);

	bool SendUserLogout(unsigned long SessionID_In, unsigned long UserID_In);
	bool SendUserMove(unsigned long SessionID_In, unsigned long UserID_In);

	bool SendServerZone(CSendStream& SendStream, unsigned long ServerID_In, 
        const sockaddr_in& gameServerTCPAddress, unsigned short usError_In);

    bool SendUnifiedCharSelectAck(CSendStream& SendStream, 
        unsigned long dwUID, unsigned long dwRequestKey, unsigned short usError_In);
};

#endif
