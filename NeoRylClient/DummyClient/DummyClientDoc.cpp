// DummyClientDoc.cpp : CDummyClientDoc 클래스의 구현
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


// CDummyClientDoc 생성/소멸

CDummyClientDoc::CDummyClientDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CDummyClientDoc::~CDummyClientDoc()
{
}

BOOL CDummyClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CDummyClientDoc serialization

void CDummyClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CDummyClientDoc 진단

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


