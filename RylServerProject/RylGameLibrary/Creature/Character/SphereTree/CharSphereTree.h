#ifndef __CHARACTER_SPHERE_TREE_H__
#define __CHARACTER_SPHERE_TREE_H__

#include "Sphere.h"
#include "CharSphereTreeConstants.h"
using namespace SphereConst;

#include <map>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{	if ((p)) { delete (p); (p) = NULL; }	}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{	if ((p)) { delete [] (p); (p) = NULL; }	}
#endif

//======================================================================================
struct Position;
class CCharacter;
class CSphere;
class CCharSphereNode;
class CCharSphereTree;
class CCharSphereNodeFifo;
class ICharSphereTreeCallBack;

//======================================================================================
class ICharSphereTreeCallBack
{
public:
	virtual void RangeTestCallBack( const Position& centerPos, float fDistance, CCharSphereNode* pNode )
	{
	}

private:
};

//======================================================================================
class CCharSphereNode : public CSphere
{
public:
	CCharSphereNode();
	CCharSphereNode( CCharSphereTree* pTree, const Position& Pos, float fRadius, void* pLink );

	void	Init( CCharSphereTree* pTree, const Position& Pos, float fRadius, void* pLink );

	// ===================================================================
	// Flag �Լ�
	void	SetSphereNodeFlag( eSphereNodeFlag flag )		{ m_dwFlag |= flag;		}
	void	ClearShpereNodeFlag( eSphereNodeFlag flag )		{ m_dwFlag &= ~flag;	}
	bool	HasSphereNodeFlag( eSphereNodeFlag flag ) const
	{
		if ( m_dwFlag & flag ) return true;
		return false;
	}

	// ===================================================================
	// ĳ���� ���� �Լ�
	void					SetCID( unsigned long dwCID )						{ m_dwCID = dwCID;				}
	unsigned long			GetCID() const										{ return m_dwCID;				}
	void					SetCharacter(CCharacter* pCharacter)				{ m_pCharacter = pCharacter;	}
	CCharacter*				GetCharacter() const								{ return m_pCharacter;			}

	// ===================================================================
	// �θ� & �ڽ� �Լ�
	void				SetParent( CCharSphereNode* pParent )		{ m_pParent = pParent;	}
	CCharSphereNode*	GetParent() const							{ return m_pParent;		}

	void				AddChild( CCharSphereNode* pChild );
	void				DeleteChild( CCharSphereNode* pChild );
	CCharSphereNode*	GetChildren() const							{ return m_pChildren;	}
	int					GetChildCount() const						{ return m_iChildCount;	}

	// ===================================================================
	// ���� ������ ���� �Լ�
	void				SetPrevSibling( CCharSphereNode* pNode )	{ m_pPrevSibling = pNode;	}
	void				SetNextSibling( CCharSphereNode* pNode )	{ m_pNextSibling = pNode;	}
	CCharSphereNode*	GetPrevSibling() const						{ return m_pPrevSibling;	}
	CCharSphereNode*	GetNextSibling() const						{ return m_pNextSibling;	}

	// ===================================================================
	// Fifo ������ ���� �Լ�
	void			SetRecomputeFifo( CCharSphereNode** ppFifo )	{ m_ppRecomuteFifo = ppFifo;	}
	void			SetIntegrateFifo( CCharSphereNode** ppFifo )	{ m_ppIntegrateFifo = ppFifo;	}

	// ===================================================================
	// �θ�� �ڽ� ����� ������ ���� �Լ� (�����δ� �θ�͸� ������ ���´�.)
	void			Unlink();

	// ===================================================================
	// Root tree node �� Link ( Leaf tree �� Root �� �ƴ� SuperSphere node �� ������ )
	void			SetLink( void* pLink )					{ m_pLink = pLink;	}
	void*			GetLink() const							{ return m_pLink;	}

	// ===================================================================
	// ��ǥ ���� �Լ�
	void			NewPos( const Position& Pos );
	void			NewPos( float fX, float fY, float fZ );
	void			NewPosRadius( const Position& Pos, float fRadius );
	void			NewPosRadius( float fX, float fY, float fZ, float fRadius );

	// ===================================================================
	// �Ÿ� �� ���ε��Ÿ� ��� �Լ�
	float			Distance2( const CCharSphereNode* pNode );
	void			ComputeBindingDistance( const CCharSphereNode* pParent );

	// ===================================================================
	// ũ�� ���� �Լ� ( true ���Ͻÿ��� �����ؾ��� )
	bool			Recompute( float fGravy );

	// ===================================================================
	// ���� �׽�Ʈ�� �Լ�
	void			RangeTest( const Position& centerPos, float fDistance, ICharSphereTreeCallBack* pCallBack );
	void			SendToRange( const Position& centerPos, float fDistance, const char* szPacket,
								 unsigned long dwPacketSize, unsigned char cCMD_In );

protected:

private:

	unsigned long			m_dwCID;
	CCharacter*				m_pCharacter;

	CCharSphereNode*		m_pParent;
	CCharSphereNode*		m_pChildren;

	CCharSphereNode*		m_pPrevSibling;
	CCharSphereNode*		m_pNextSibling;

	CCharSphereNode**		m_ppRecomuteFifo;	// Recompute Fifo ���� ��ġ ������
	CCharSphereNode**		m_ppIntegrateFifo;	// Integrate Fifo ���� ��ġ ������

	int						m_iChildCount;
	unsigned long			m_dwFlag;
	float					m_fBindingDistance2;

	void*					m_pLink;
	CCharSphereTree*		m_pTree;
};


//======================================================================================
class CCharSphereTree
{
public:
	CCharSphereTree( int iMaxSphereNode, float fRootSize, float fLeafSize, float fGravy );
	virtual ~CCharSphereTree();

	typedef std::map< unsigned long, CCharSphereNode* >	CharSphereMap;

	static CCharSphereTree&	GetInstance();

	// ===================================================================
	// Tree �� Node �� �����ϰ� �籸���ϴ� ó�� �Լ�
	void	Process();

	// ===================================================================
	// ĳ���� ���� �Լ�
	bool	AddCharacter( unsigned long dwCID, CCharacter* pCharacter, const Position& Pos ) ;
	bool	DeleteCharacter( unsigned long dwCID );
	void	MoveTo( unsigned long dwCID, const Position& NewPos );

	// ===================================================================
	// CCharSphereNode �� ����(ĳ���� ����)�ؼ� �籸���ϴ� �Լ� �� �����ϴ� �Լ�
	CCharSphereNode*	AddSphere( const Position& Pos, float fRadius, void* pLink, unsigned long dwFlag = SNF_LEAF_TREE );
	void				DeleteSphere( CCharSphereNode* pNode );

	// ===================================================================
	// ����, �籸�� ����Ʈ�� Node �߰� �Լ�
	void	AddRecompute( CCharSphereNode* pNode );
	void	AddIntegrate( CCharSphereNode* pNode );

	// ===================================================================
	// Tree �� �籸���ϴ� �Լ�
	void	Integrate( CCharSphereNode* pNode, CCharSphereNode* pSuperSphere, float fNodeSize );

	// ===================================================================
	// ���� �׽�Ʈ�� �Լ�
	void	RangeTest( const Position& centerPos, float fDistance, ICharSphereTreeCallBack* pCallBack );
	void	SendToRange( const Position& centerPos, float fDistance, const char* szPacket,
						 unsigned long dwPacketSize, unsigned char cCMD_In );

protected:
	// ===================================================================
	// ��� �ڽ� Node �� �޸� �����ϴ� �Լ�
	void	DeleteAllChildSphere( CCharSphereNode* pRootNode );

private:
	CCharSphereNode*		m_pRoot;
	CCharSphereNode*		m_pLeaf;

	CCharSphereNodeFifo*	m_Recompute;
	CCharSphereNodeFifo*	m_Integrate;

	float					m_fMaxRootSize;
	float					m_fMaxLeafSize;
	float					m_fSuperSphereGravy;

	CharSphereMap			m_CharSphereMap;
};


//======================================================================================
class CCharSphereNodeFifo
{
public:
	CCharSphereNodeFifo( int iSize )
	{
		m_iCount = 0;
		m_iStackPointer = 0;
		m_iBottom = 0;
		m_iSize = iSize;
		m_ppFifo = new CCharSphereNode *[ m_iSize ];
	};

	~CCharSphereNodeFifo()
	{
		SAFE_DELETE_ARRAY( m_ppFifo );
	};

	CCharSphereNode** Push( CCharSphereNode* pNode )
	{
		m_iCount++;
		CCharSphereNode** ret = &m_ppFifo[ m_iStackPointer ];
		m_ppFifo[ m_iStackPointer ] = pNode;
		++m_iStackPointer;
		if ( m_iStackPointer == m_iSize ) m_iStackPointer = 0;
		return ret;
	};

	CCharSphereNode* Pop()
	{
		while ( m_iStackPointer != m_iBottom )
		{
			--m_iCount;
			CCharSphereNode* ret = m_ppFifo[ m_iBottom ];
			++m_iBottom;
			if ( m_iBottom == m_iSize ) m_iBottom = 0;
			if ( ret ) return ret;
		}
		return NULL;
	}

	bool Flush( CCharSphereNode* pNode )
	{
		if ( m_iStackPointer == m_iBottom ) return false;
		int i = m_iBottom;
		while ( i != m_iStackPointer )
		{
			if ( m_ppFifo[i] == pNode )
			{
				m_ppFifo[i] = NULL;
				return true;
			}
			i++;
			if ( i == m_iSize ) i = 0;
		}
		return false;
	};

	int GetCount(void) const	{ return m_iCount; }

private:

	int					m_iCount;
	int					m_iStackPointer;
	int					m_iBottom;
	int					m_iSize;
	CCharSphereNode**	m_ppFifo;
};


#endif	__CHARACTER_SPHERE_TREE_H__