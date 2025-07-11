//================================================================
//
//	Name : RYLClientMain.h 
//	Desc : Client Main
//  Date : 2003. 11. 24
//
//================================================================
#ifndef __RYL_CLIENTMAIN_H__
#define __RYL_CLIENTMAIN_H__

//#define _WIN32_WINNT 0x403

#include <winsock2.h>

#include "InitValue.h"
#include "BaseGraphicsLayer.h"

#include "..\\CharStatus.h"

enum AdminLevelMode
{
	ADMIN_NONE = 0,
	SUPPORTER = 1,
	ADMIN_L1 = 2,
	ADMIN_L2 = 3,
	ADMIN_L3 = 4,
	ADMIN_DEV = 5,
};

enum NetworkError
{
	NE_NOERROR = 0,
	NE_VERSION,
	NE_USERID,
	NE_USERPW,
	NE_LOGINSERVER,				//커넥팅 에러
	NE_AGENTSERVER, 
	NE_LOGINCHRINFO,
	NE_SELECTCHR, 
	NE_CHRCREATE,
	NE_CHRDELETE,
	NE_CHRMOVE,
	NE_CHRSELECT,
	NE_CHRSHAPE,
	NE_CHRATT,
    NE_LOGOUT,
	NE_CHAT,
	NE_DEAD,
	NE_RESP,
	NE_LEVELUP,
	NE_TRADE,
	NE_MOTION,
        
	NE_TAKEOUT,
	NE_TAKEIN, 
	NE_ALREADY,					//이미 접속되어있음,
	NE_DBCONNECTION,
	NE_UNIDENTYUSER,
	NE_USERDATA,
	NE_LOCATION,
	NE_DBFAIL
};


#define WriteLog(s) WriteLogtoFile(s, __FILE__, __LINE__)

using namespace std;

class CRYLGameData ;
class CRYLNetworkData ;
class CRYLCommunityData ;
class CRYLIMEView ;
class CRYLSceneManager ;
class CRYLLoadingScene ;
class CRYLCastleManager ;
class CRYLCampManager ;
class CRYLSkillScriptMgr ;

class CClientMain
{
public :

	HINSTANCE				m_hInstance ;
	HWND					m_hWnd ;
	HINSTANCE				m_hnProtectLib ;
	DEVMODE					m_DeviceMode ;

	char					m_strClientPath[ MAX_PATH ] ;			// 클라이언트 위치
	CInitValue				m_InitValue ;							// Init Value Class  /Video,Adapter,Mode/

	BaseGraphicsLayer		m_BaseGraphicLayer ;					// BaseGraphics Layer Class
	CRYLGameData*			m_pRYLGameData ;						// Game관련 Data 클래스
	CRYLNetworkData*		m_pRYLNetworkData ;						// 네트웍관련 Data 클레스
	CRYLCommunityData*		m_pRYLCommunityData ;					// 커뮤니티 관련 클래스
	CRYLCastleManager*		m_pCastleManager ;						// 공성 관련 메니저 클레스
	CRYLCampManager*		m_pCampManager ;						// 진지 관련 메니저 클레스
	CRYLSkillScriptMgr*		m_pSkillScriptManager ;					// Lua 스킬 스크립트 매니저 클래스

	unsigned long			m_dwVersion ;							// 체크섬
	unsigned long			m_dwClientVersion ;						// 클라이언트 버전

	unsigned short			m_wClientLoginFlag ;					// 로긴시에 필요한 플래그(플래그는 서버에 줄 값)
	unsigned short			m_wAdminMode ;							// 운영자 모드 (5 = 개발자 1,2,3 = 운영자)
	bool					m_bUDPInfo ;							// UDP 정보 출력 여부 (디버깅용)

	unsigned long   m_dwServerTotal;//hz speed hack test check start
	unsigned long   m_dwServerLast;
	unsigned long   m_dwClientTotal;
	unsigned long   m_dwClinetLast;
	unsigned long   m_dwHzSpeedHack;//hz speed hack test check end

	INT						m_iScreenWidth ;
	INT						m_iScreenHeight ;
	INT						m_iBpp ;

	INT						m_iRefresh;								// 리프레쉬율 by Belldandy
	INT						m_iWindowMode ;
	INT						m_iLanguage;							// 랭귀지
	INT						m_iMultiSupport;						// 멀티 랭귀지 서포트
	
	unsigned long			m_dwUseJoystick ;						// 조이스틱 사용 여부

	CRYLSceneManager*		m_pRYLSceneManager ;
	CRYLIMEView*			m_pImeView ;
	LPDIRECT3DDEVICE8		m_lpD3DDevice ;							// D3D Device

	CRYLLoadingScene*		m_pLoadingScene ;						// Loading Scene [ Instance를 따로 받아와야함 ]
	//name check hack hz
	int						loop;

public :

	CClientMain(void);
	~CClientMain(void);

	unsigned long  CheckOCTREE();

	BOOL	Init(HINSTANCE hInstance, INT iWidth, INT iHeight );
	bool	InitCreateGameClass() ;
	void	Destroy(void);
	bool    CheckSpeedHack(unsigned long CurrentServer, unsigned long CurrentClient);//hz speed hack test check
	void	Update(void);
	void	Render(void);
	void	RenderMain(void);
	HRESULT		CreateScene() ;

	void	ProcessScreenshot(void);
	
	void	SetDisplay(void);
	void	RestoreDisplay(void);

	void	SetFocus(void) { ::SetFocus(m_hWnd); }
	
    VOID	GetServerInfo( HWND hWnd ) ;

//	void	PrintException(EXCEPTION_RECORD &SavedExceptRec, CONTEXT &SavedContext);
	void	WriteLogtoFile(const char *strLog, const char *strFileName, int nLine);
	
	void	SetDayTimer(void);	

	unsigned long ClientProc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam);

	unsigned long GetUseJoystick() { return m_dwUseJoystick; }

	// 해상도 값 얻기
	VOID	QueryRegScreenValue() ;
	
#ifdef JAPAN_CODE
	VOID	GetJapanServerInfo() ;
#endif

	// --------------------------------------------
	// IME 관련

	VOID		InitIMEView() ;
	VOID		FinalIMEView() ;

	// --------------------------------------------


private:

	void ViewFPS(void);
};

extern CClientMain g_ClientMain;
#endif // __RYL_CLIENTMAIN_H__