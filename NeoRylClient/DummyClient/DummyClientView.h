// DummyClientView.h : iCDummyClientView 클래스의 인터페이스
//


#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CDummyClientView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CDummyClientView();
	DECLARE_DYNCREATE(CDummyClientView)

public:
	enum{ IDD = IDD_DummyClient_FORM };

// 특성
public:
	CDummyClientDoc* GetDocument() const;

// 작업
public:

// 재정의
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현
public:
	virtual ~CDummyClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbGuildInfoMethod;
	CComboBox m_cbMemberSortMethod;
	CListCtrl m_ctrlGuildInfo;
	CListCtrl m_ctrlMemberInfo;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnCrest();
	afx_msg void OnBnClickedBtnDeposit();
	afx_msg void OnBnClickedBtnDetails();
	afx_msg void OnBnClickedBtnFind();
	afx_msg void OnBnClickedBtnLev();
	afx_msg void OnBnClickedBtnRelease();
	afx_msg void OnBnClickedBtnWithdraw();
	CString m_strName;
	CString m_strCrest;
	CString m_strMaster;
	CString m_strRep;
	CString m_strTend;
	CString m_strPol;
	CString m_strLev;
	CString m_strBalance;
	CString m_strFame;
	CString m_strGold;
	CString m_strLocation;
	afx_msg void OnBnClickedBtnReflush();
	afx_msg void OnBnClickedBtnCreate();
};

#ifndef _DEBUG  // DummyClientView.cpp의 디버그 버전
inline CDummyClientDoc* CDummyClientView::GetDocument() const
   { return reinterpret_cast<CDummyClientDoc*>(m_pDocument); }
#endif

