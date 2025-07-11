// DummyClientView.cpp : CDummyClientView 클래스의 구현
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

// CDummyClientView 생성/소멸

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
	// TODO: 여기에 생성 코드를 추가합니다.

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CDummyClientView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_cbGuildInfoMethod.AddString( "순위(명성)" ) ;
	m_cbGuildInfoMethod.AddString( "우호길드" ) ;
	m_cbGuildInfoMethod.AddString( "적대길드" ) ;
	m_cbGuildInfoMethod.AddString( "비우호/비적대길드" ) ;
	m_cbGuildInfoMethod.AddString( "길드명" ) ;
	m_cbGuildInfoMethod.AddString( "자신의 길드" ) ;

	m_cbMemberSortMethod.AddString("길드 마스터") ;
	m_cbMemberSortMethod.AddString("중간 관리자") ;
	m_cbMemberSortMethod.AddString("일반 길드원") ;
	m_cbMemberSortMethod.AddString("탈퇴 대기자") ;
	m_cbMemberSortMethod.AddString("가입 대기자") ;

	m_ctrlGuildInfo.InsertColumn( 0, _T( "순위" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlGuildInfo.InsertColumn( 1, _T( "길드명" ), LVCFMT_CENTER, 60 ) ;
	m_ctrlGuildInfo.InsertColumn( 2, _T( "길드마스터" ), LVCFMT_CENTER, 80 ) ;
	m_ctrlGuildInfo.InsertColumn( 3, _T( "길드명성" ), LVCFMT_CENTER, 60 ) ;
	m_ctrlGuildInfo.InsertColumn( 4, _T( "마크" ), LVCFMT_CENTER, 40 ) ;

	m_ctrlMemberInfo.InsertColumn( 0, _T( "서열" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlMemberInfo.InsertColumn( 1, _T( "클래스" ), LVCFMT_CENTER, 60 ) ;
	m_ctrlMemberInfo.InsertColumn( 2, _T( "레벨" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlMemberInfo.InsertColumn( 3, _T( "명성" ), LVCFMT_CENTER, 40 ) ;
	m_ctrlMemberInfo.InsertColumn( 4, _T( "이름" ), LVCFMT_CENTER, 80 ) ;
}


// CDummyClientView 진단

#ifdef _DEBUG
void CDummyClientView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDummyClientView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDummyClientDoc* CDummyClientView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDummyClientDoc)));
	return (CDummyClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CDummyClientView 메시지 처리기

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
