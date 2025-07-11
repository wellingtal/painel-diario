// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DummyClient.h"
#include "LoginDlg.h"
#include ".\logindlg.h"
#include "MainFrm.h"
#include "CharacterSelectDlg.h"
#include "GMMemory.h"

// CLoginDlg dialog

const char* szSetupFileName = "./DummyClient.ini";

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)
CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
	, m_szAccount(_T(""))
	, m_szPassword(_T(""))
    , m_szCheckSum(_T(""))
    , m_dwClientVersion(0)
    , m_nLoopCount(0)
{
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDT_LOGIN, m_szAccount);
    DDX_Text(pDX, IDC_EDT_PASSWORD, m_szPassword);
    DDX_Text(pDX, IDC_IPADDRESS1, m_AuthServerIP);
    DDX_Text(pDX, IDC_EDT_LOGIN2, m_szCheckSum);
    DDX_Text(pDX, IDC_EDT_LOGIN3, m_dwClientVersion);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_EXIT, OnBnClickedBtnExit)
	ON_BN_CLICKED(ID_BTN_LOGIN, OnBnClickedBtnLogin)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLoginDlg message handlers


//----------------------------------------------------------------------
// Desc : �α��� ó��
//----------------------------------------------------------------------
void CLoginDlg::OnBnClickedBtnExit()
{
	// TODO: Add your control notification handler code here

	
}

//----------------------------------------------------------------------
// Desc : ����
//----------------------------------------------------------------------
void CLoginDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here

    // �� ĳ���� ������ �ν��Ͻ� �����Ŀ�, ���� ȣ��

	
}

void CLoginDlg::MoveToCenter()
{
    CRect rcDlg, rcMain;

    GetClientRect(&rcDlg);    
    AfxGetMainWnd()->GetClientRect(&rcMain);

    int x = ( rcMain.Width() - rcDlg.Width() ) / 2 ;
    int y = ( rcMain.Height() - rcDlg.Height() ) / 2 ;
    MoveWindow( x, y, rcDlg.Width(), rcDlg.Height() + 20 ) ;
}

BOOL CLoginDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

    // ������Ʈ�� ���Ͽ��� �����ּ�, ����, üũ���� �о�´�.
    
    m_dwClientVersion = GetPrivateProfileInt(
        "DummyLoginSetup", "ClientVersion", 0, szSetupFileName);

    char szTemp[MAX_PATH];

    GetPrivateProfileString("DummyLoginSetup", "CheckSum", 
        "", szTemp, MAX_PATH - 1, szSetupFileName);

    szTemp[MAX_PATH - 1] = 0;
    m_szCheckSum = szTemp;

    GetPrivateProfileString("DummyLoginSetup", "AuthServerAddress", 
        "", szTemp, MAX_PATH - 1, szSetupFileName);

    szTemp[MAX_PATH - 1] = 0;
    m_AuthServerIP = szTemp;
    
    GetPrivateProfileString("DummyLoginSetup", "Account", 
        "", szTemp, MAX_PATH - 1, szSetupFileName);

    szTemp[MAX_PATH - 1] = 0;
    m_szAccount = szTemp;

    GetPrivateProfileString("DummyLoginSetup", "Password", 
        "", szTemp, MAX_PATH - 1, szSetupFileName);

    szTemp[MAX_PATH - 1] = 0;
    m_szPassword = szTemp;

    MoveToCenter();
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CLoginDlg::OnDestroy()
{
    UpdateData(TRUE);

    char szClientVersion[MAX_PATH];
    itoa(m_dwClientVersion, szClientVersion, 10);

    WritePrivateProfileString("DummyLoginSetup", "ClientVersion", szClientVersion, szSetupFileName);
    WritePrivateProfileString("DummyLoginSetup", "CheckSum", m_szCheckSum, szSetupFileName);

    WritePrivateProfileString("DummyLoginSetup", "AuthServerAddress", 
        m_AuthServerIP.GetBuffer(0), szSetupFileName);

    WritePrivateProfileString("DummyLoginSetup", "Account", 
        m_szAccount.GetBuffer(0), szSetupFileName);

    WritePrivateProfileString("DummyLoginSetup", "Password", 
        m_szPassword.GetBuffer(0), szSetupFileName);

    CDialog::OnDestroy();

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
