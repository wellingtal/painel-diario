// DummyClientDoc.h : CDummyClientDoc Ŭ������ �������̽�
//


#pragma once

class CDummyClientDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CDummyClientDoc();
	DECLARE_DYNCREATE(CDummyClientDoc)

// Ư��
public:

// �۾�
public:

// ������
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CDummyClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};


