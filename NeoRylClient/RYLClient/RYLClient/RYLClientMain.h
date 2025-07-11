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
	NE_LOGINSERVER,				//Ŀ���� ����
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
	NE_ALREADY,					//�̹� ���ӵǾ�����,
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

	char					m_strClientPath[ MAX_PATH ] ;			// Ŭ���̾�Ʈ ��ġ
	CInitValue				m_InitValue ;							// Init Value Class  /Video,Adapter,Mode/

	BaseGraphicsLayer		m_BaseGraphicLayer ;					// BaseGraphics Layer Class
	CRYLGameData*			m_pRYLGameData ;						// Game���� Data Ŭ����
	CRYLNetworkData*		m_pRYLNetworkData ;						// ��Ʈ������ Data Ŭ����
	CRYLCommunityData*		m_pRYLCommunityData ;					// Ŀ�´�Ƽ ���� Ŭ����
	CRYLCastleManager*		m_pCastleManager ;						// ���� ���� �޴��� Ŭ����
	CRYLCampManager*		m_pCampManager ;						// ���� ���� �޴��� Ŭ����
	CRYLSkillScriptMgr*		m_pSkillScriptManager ;					// Lua ��ų ��ũ��Ʈ �Ŵ��� Ŭ����

	unsigned long			m_dwVersion ;							// üũ��
	unsigned long			m_dwClientVersion ;						// Ŭ���̾�Ʈ ����

	unsigned short			m_wClientLoginFlag ;					// �α�ÿ� �ʿ��� �÷���(�÷��״� ������ �� ��)
	unsigned short			m_wAdminMode ;							// ��� ��� (5 = ������ 1,2,3 = ���)
	bool					m_bUDPInfo ;							// UDP ���� ��� ���� (������)

	unsigned long   m_dwServerTotal;//hz speed hack test check start
	unsigned long   m_dwServerLast;
	unsigned long   m_dwClientTotal;
	unsigned long   m_dwClinetLast;
	unsigned long   m_dwHzSpeedHack;//hz speed hack test check end

	INT						m_iScreenWidth ;
	INT						m_iScreenHeight ;
	INT						m_iBpp ;

	INT						m_iRefresh;								// ���������� by Belldandy
	INT						m_iWindowMode ;
	INT						m_iLanguage;							// ������
	INT						m_iMultiSupport;						// ��Ƽ ������ ����Ʈ
	
	unsigned long			m_dwUseJoystick ;						// ���̽�ƽ ��� ����

	CRYLSceneManager*		m_pRYLSceneManager ;
	CRYLIMEView*			m_pImeView ;
	LPDIRECT3DDEVICE8		m_lpD3DDevice ;							// D3D Device

	CRYLLoadingScene*		m_pLoadingScene ;						// Loading Scene [ Instance�� ���� �޾ƿ;��� ]
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

	// �ػ� �� ���
	VOID	QueryRegScreenValue() ;
	
#ifdef JAPAN_CODE
	VOID	GetJapanServerInfo() ;
#endif

	// --------------------------------------------
	// IME ����

	VOID		InitIMEView() ;
	VOID		FinalIMEView() ;

	// --------------------------------------------


private:

	void ViewFPS(void);
};

extern CClientMain g_ClientMain;
#endif // __RYL_CLIENTMAIN_H__