// DummyClientDoc.cpp : CDummyClientDoc Ŭ������ ����
//

#include "stdafx.h"
#include "DummyClient.h"

#include "DummyClientDoc.h"
#include "CharacterData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDummyClientDoc
IMPLEMENT_DYNCREATE(CDummyClientDoc, CDocument)


// CDummyClientDoc ����/�Ҹ�

CDummyClientDoc::CDummyClientDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CDummyClientDoc::~CDummyClientDoc()
{
}

BOOL CDummyClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CDummyClientDoc serialization

void CDummyClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CDummyClientDoc ����

#ifdef _DEBUG
void CDummyClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDummyClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


