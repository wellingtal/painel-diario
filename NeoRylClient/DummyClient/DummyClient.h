// DummyClient.h : DummyClient ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CDummyClientApp:
// �� Ŭ������ ������ ���ؼ��� DummyClient.cpp�� �����Ͻʽÿ�.
//

class CDummyClientApp : public CWinApp
{
public:

    CDummyClientApp();

// ������
public:
	
    virtual BOOL    InitInstance();

// ����

    DECLARE_MESSAGE_MAP()
   
};

extern CDummyClientApp theApp;
