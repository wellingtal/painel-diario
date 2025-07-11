// DummyClientView.cpp : CDummyClientView Ŭ������ ����
//

#include "stdafx.h"
#include "DummyClient.h"

#include "DummyClientDoc.h"
#include "DummyClientView.h"
#include ".\DummyClientview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDummyClientView
IMPLEMENT_DYNCREATE(CDummyClientView, CFormView)

BEGIN_MESSAGE_MAP(CDummyClientView, CFormView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_CREST, OnBnClickedBtnCrest)
	ON_BN_CLICKED(IDC_BTN_DEPOSIT, OnBnClickedBtnDeposit)
	ON_BN_CLICKED(IDC_BTN_DETAILS, OnBnClickedBtnDetails)
	ON_BN_CLICKED(IDC_BTN_FIND, OnBnClickedBtnFind)
	ON_BN_CLICKED(IDC_BTN_LEV, OnBnClickedBtnLev)
	ON_BN_CLICKED(IDC_BTN_RELEASE, OnBnClickedBtnRelease)
	ON_BN_CLICKED(IDC_BTN_WITHDRAW, OnBnClickedBtnWithdraw)
	ON_BN_CLICKED(IDC_BTN_REFLUSH, OnBnClickedBtnReflush)
	ON_BN_CLICKED(IDC_BTN_CREATE, OnBnClickedBtnCreate)
END_MESSAGE_MAP()

// CDummyClientView ����/�Ҹ�

CDummyClientView::CDummyClientView()
	: CFormView(CDummyClientView::IDD)
	, m_strName(_T(""))
	, m_strCrest(_T(""))
	, m_strMaster(_T(""))
	, m_strRep(_T(""))
	, m_strTend(_T(""))
	, m_strPol(_T(""))
	, m_strLev(_T(""))
	, m_strBalance(_T(""))
	, m_strFame(_T(""))
	, m_strGold(_T(""))
	, m_strLocation(_T(""))
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CDummyClientView::~CDummyClientView()
{
}

void CDummyClientView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GUILDINFOSORT_COMBO, m_cbGuildInfoMethod);
	DDX_Control(pDX, IDC_GUILDMBRSORT_COMBO, m_cbMemberSortMethod);
	DDX_Control(pDX, IDC_GUILDINFO_LISTCTRL, m_ctrlGuildInfo);
	DDX_Control(pDX, IDC_GUILDMBR_LISTCTRL, m_ctrlMemberInfo);
	DDX_Text(pDX, IDC_EDT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDT_CREST, m_strCrest);
	DDX_Text(pDX, IDC_EDT_MASTER, m_strMaster);
	DDX_Text(pDX, IDC_EDT_REP, m_strRep);
	DDX_Text(pDX, IDC_EDT_TEND, m_strTend);
	DDX_Text(pDX, IDC_EDT_POL, m_strPol);
	DDX_Text(pDX, IDC_EDT_LEV, m_strLev);
	DDX_Text(pDX, IDC_EDT_BALANCE, m_strBalance);
	DDX_Text(pDX, IDC_EDT_FAME, m_strFame);
	DDX_Text(pDX, IDC_EDT_GOLD, m_strGold);
	DDX_Text(pDX, IDC_EDT_LOCATION, m_strLocation);
}

BOOL CDummyClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CDummyClientView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_cbGuildInfoMethod.AddString( "����(��)" ) ;
	m_cbGuildInfoMethod.AddString( "��ȣ���" ) ;
	m_cbGuildInfoMethod.AddString( "������" ) ;
	m_cbGuildInfoMethod.AddString( "���ȣ/��������" ) ;
	m_cbGuildInfoMethod.AddString( "����" ) ;
	m_cbGuildInfoMethod.AddString( "�ڽ��� ���" ) ;

	m_cbMemberSortMethod.AddString("��� ������") ;
	m_cbMemberSortMethod.AddString("�߰� ������") ;
	m_cbMemberSortMethod.AddString("�Ϲ� ����") ;
	m_cbMemberSortMethod.AddString("Ż�� �����") ;
	m_cbMemberSortMethod.AddString("���� �����") ;

	m_ctrlGuildInfo.InsertColumn( 0, _T( "����" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlGuildInfo.InsertColumn( 1, _T( "����" ), LVCFMT_CENTER, 60 ) ;
	m_ctrlGuildInfo.InsertColumn( 2, _T( "��帶����" ), LVCFMT_CENTER, 80 ) ;
	m_ctrlGuildInfo.InsertColumn( 3, _T( "����" ), LVCFMT_CENTER, 60 ) ;
	m_ctrlGuildInfo.InsertColumn( 4, _T( "��ũ" ), LVCFMT_CENTER, 40 ) ;

	m_ctrlMemberInfo.InsertColumn( 0, _T( "����" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlMemberInfo.InsertColumn( 1, _T( "Ŭ����" ), LVCFMT_CENTER, 60 ) ;
	m_ctrlMemberInfo.InsertColumn( 2, _T( "����" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlMemberInfo.InsertColumn( 3, _T( "��" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlMemberInfo.InsertColumn( 4, _T( "�̸�" ), LVCFMT_CENTER, 80 ) ;
}


// CDummyClientView ����

#ifdef _DEBUG
void CDummyClientView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDummyClientView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDummyClientDoc* CDummyClientView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDummyClientDoc)));
	return (CDummyClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CDummyClientView �޽��� ó����

int CDummyClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

BOOL CDummyClientView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CDummyClientView::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnCrest()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnDeposit()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnDetails()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnFind()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnLev()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnRelease()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnWithdraw()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnReflush()
{
	// TODO: Add your control notification handler code here
}

void CDummyClientView::OnBnClickedBtnCreate()
{
	// TODO: Add your control notification handler code here
}
