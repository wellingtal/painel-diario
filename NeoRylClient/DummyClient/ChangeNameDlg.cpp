// ChangeNameDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DummyClient.h"
#include "ChangeNameDlg.h"
#include "CharacterData.h"
#include "MainFrm.h"
#include <Network/ClientSocket/Send/SendEtc.h>
#include "GMMemory.h"


// CChangeNameDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CChangeNameDlg, CDialog)
CChangeNameDlg::CChangeNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeNameDlg::IDD, pParent)
    , m_szOldName(_T(""))
    , m_szNewName(_T(""))
{
}

CChangeNameDlg::~CChangeNameDlg()
{
}

void CChangeNameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_OLD_NAME, m_szOldName);
    DDV_MaxChars(pDX, m_szOldName, 15);
    DDX_Text(pDX, IDC_NEW_NAME, m_szNewName);
	DDV_MaxChars(pDX, m_szNewName, 15);
}


BEGIN_MESSAGE_MAP(CChangeNameDlg, CDialog)
END_MESSAGE_MAP()


// CChangeNameDlg �޽��� ó�����Դϴ�.

void CChangeNameDlg::OnOK()
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
    UpdateData(true);
    
    CCharacterDataMgr& characterDataMgr = 
        static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

    CCharacterData* lpCharacterData = characterDataMgr.GetByCharName(m_szOldName);

    if(0 != lpCharacterData)
    {
        SendPacket::CharNameChange(
            &lpCharacterData->GetClientSocket().GetHandler(ClientSocket::GameEventHandler),  
            lpCharacterData->GetUID(), lpCharacterData->GetCID(), m_szNewName);
    }

    CDialog::OnOK();
}
