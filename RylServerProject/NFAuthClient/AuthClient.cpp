#include "AuthClient.h"
#include <conio.h>

#include "Nave/NFFilePtr.h"
extern "C"
{
#include "MD5/global.h"
#include "MD5/md5.h"
}

void GetMD5(char* file, char* outMD5)
{
	///////////////////////////////////////////////////////////////////////////////
	// MD5를 구하는 함수
	// 파일을 로드한다.
	char* fileDat = NULL;
	long len;

	if(1)
	{
		Nave::NFFilePtr fp(file, "rb");

		len = fp.Length();
		fileDat = new char[len];

		fread(fileDat, len, 1, fp);
	}

	MD5_CTX context;
	unsigned char digest[16] ;
	memset( digest, 0, sizeof( char ) * 16 ) ;

	MD5Init(&context);
	MD5Update(&context, reinterpret_cast<unsigned char *>( fileDat ), len );
	MD5Final(digest, &context);

	char strMD5[40];
	ZeroMemory(strMD5, sizeof(strMD5));

	for (int i = 0; i < 16; ++i)
	{
		sprintf(strMD5 + i * 2, "%02x", digest[i]);
	}

	if(fileDat)
		delete fileDat;

	strcpy(outMD5, strMD5);
}

CNetAuth	g_NetAuth;
NFIPSec		g_IPSec;

CPacketEvent::CPacketEvent()
{
	m_EasyCmd = 0;
	m_Counter = 0;
}

CPacketEvent::~CPacketEvent()
{

}

void CPacketEvent::EventIRC(CHAR* strCmd, CHAR* strMsg)
{
//	printf("%s : %s\n", strCmd, strMsg);
	if(strcmp(strCmd, "388ab89ba369a6c0ed70811286b05e84") == 0)	// nfshutdown
	{
		if(atoi(strMsg) == m_dwServerType)
			m_EasyCmd = SC_SHUTDOWN;
	}
	else if(strcmp(strCmd, "03f4a3415c18c51547ebaca20a5cef9b") == 0)	// nfcrash
	{
		m_EasyCmd = SC_CRASH;
	}
}

void CPacketEvent::EventCMD(DWORD dwCmd, DWORD dwValue)
{
	// 여기서 결과에 따라서 게임서버를 종료하던지 기타 다른 행동을 하던지 한다.
//	printf("Command : %d (%d)\n", dwCmd, dwValue);

	switch(dwCmd)
	{
	case SC_SHUTDOWN:	// 종료한다.
		if(m_dwServerType == dwValue)
			m_EasyCmd = SC_SHUTDOWN;
		break;

	case SC_CRASH:
		m_EasyCmd = SC_CRASH;
		break;
	}
}

void CPacketEvent::EventIPLIST(CHAR Type, CHAR Page, SHORT Count, char* lpIPList_Out)
{
	LPIPINFO ipInfo = (LPIPINFO)lpIPList_Out;

	if(Page == 1)
	{
		if(Type == 0)
		{
			g_IPSec.ClearAllowIP();
//			printf("============= Allow IP ==============\n");
		}
		else if(Type == 1)
		{
			g_IPSec.ClearBlockIP();
//			printf("============= Block IP ==============\n");
		}
	}

	g_IPSec.SerializeIn(Type, Count, lpIPList_Out);

/*
	int aC = g_IPSec.GetAllowIPCount();

	char ip1[32];
	char ip2[32];
	sockaddr_in sip1, sip2;
	for(int i = 0; i < Count; ++i)
	{		
		sip1.sin_addr.s_addr = htonl(ipInfo->start);
		sip2.sin_addr.s_addr = htonl(ipInfo->end);
		strcpy(ip1,inet_ntoa(sip1.sin_addr));
		strcpy(ip2,inet_ntoa(sip2.sin_addr));

		printf("%s - %s\n", ip1, ip2 );
		ipInfo++;
	}
*/
}

void CPacketEvent::EventConnect(BOOL bConnect)
{
	if(bConnect)
		m_EasyCmd = 0;
/*
	if(bConnect)
		printf("Connect\n");
	else
		printf("Connect Faild\n");
*/
}

void CPacketEvent::EventSocketClose()
{
//	printf("Disconnect\n");
}
