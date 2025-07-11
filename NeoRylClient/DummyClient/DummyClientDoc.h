// DummyClientDoc.h : CDummyClientDoc 클래스의 인터페이스
//


#pragma once

class CDummyClientDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CDummyClientDoc();
	DECLARE_DYNCREATE(CDummyClientDoc)

// 특성
public:

// 작업
public:

// 재정의
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현
public:
	virtual ~CDummyClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};


