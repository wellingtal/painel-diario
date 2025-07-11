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
	// Flag 함수
	void	SetSphereNodeFlag( eSphereNodeFlag flag )		{ m_dwFlag |= flag;		}
	void	ClearShpereNodeFlag( eSphereNodeFlag flag )		{ m_dwFlag &= ~flag;	}
	bool	HasSphereNodeFlag( eSphereNodeFlag flag ) const
	{
		if ( m_dwFlag & flag ) return true;
		return false;
	}

	// ===================================================================
	// 캐릭터 관련 함수
	void					SetCID( unsigned long dwCID )						{ m_dwCID = dwCID;				}
	unsigned long			GetCID() const										{ return m_dwCID;				}
	void					SetCharacter(CCharacter* pCharacter)				{ m_pCharacter = pCharacter;	}
	CCharacter*				GetCharacter() const								{ return m_pCharacter;			}

	// ===================================================================
	// 부모 & 자식 함수
	void				SetParent( CCharSphereNode* pParent )		{ m_pParent = pParent;	}
	CCharSphereNode*	GetParent() const							{ return m_pParent;		}

	void				AddChild( CCharSphereNode* pChild );
	void				DeleteChild( CCharSphereNode* pChild );
	CCharSphereNode*	GetChildren() const							{ return m_pChildren;	}
	int					GetChildCount() const						{ return m_iChildCount;	}

	// ===================================================================
	// 같은 레벨의 형제 함수
	void				SetPrevSibling( CCharSphereNode* pNode )	{ m_pPrevSibling = pNode;	}
	void				SetNextSibling( CCharSphereNode* pNode )	{ m_pNextSibling = pNode;	}
	CCharSphereNode*	GetPrevSibling() const						{ return m_pPrevSibling;	}
	CCharSphereNode*	GetNextSibling() const						{ return m_pNextSibling;	}

	// ===================================================================
	// Fifo 포인터 관리 함수
	void			SetRecomputeFifo( CCharSphereNode** ppFifo )	{ m_ppRecomuteFifo = ppFifo;	}
	void			SetIntegrateFifo( CCharSphereNode** ppFifo )	{ m_ppIntegrateFifo = ppFifo;	}

	// ===================================================================
	// 부모와 자식 모두의 연결을 끊는 함수 (실제로는 부모와만 연결을 끊는다.)
	void			Unlink();

	// ===================================================================
	// Root tree node 의 Link ( Leaf tree 의 Root 가 아닌 SuperSphere node 가 가진다 )
	void			SetLink( void* pLink )					{ m_pLink = pLink;	}
	void*			GetLink() const							{ return m_pLink;	}

	// ===================================================================
	// 좌표 변경 함수
	void			NewPos( const Position& Pos );
	void			NewPos( float fX, float fY, float fZ );
	void			NewPosRadius( const Position& Pos, float fRadius );
	void			NewPosRadius( float fX, float fY, float fZ, float fRadius );

	// ===================================================================
	// 거리 및 바인딩거리 계산 함수
	float			Distance2( const CCharSphereNode* pNode );
	void			ComputeBindingDistance( const CCharSphereNode* pParent );

	// ===================================================================
	// 크기 재계산 함수 ( true 리턴시에는 제거해야함 )
	bool			Recompute( float fGravy );

	// ===================================================================
	// 범위 테스트용 함수
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

	CCharSphereNode**		m_ppRecomuteFifo;	// Recompute Fifo 내의 위치 포인터
	CCharSphereNode**		m_ppIntegrateFifo;	// Integrate Fifo 내의 위치 포인터

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
	// Tree 의 Node 를 재계산하고 재구성하는 처리 함수
	void	Process();

	// ===================================================================
	// 캐릭터 관련 함수
	bool	AddCharacter( unsigned long dwCID, CCharacter* pCharacter, const Position& Pos ) ;
	bool	DeleteCharacter( unsigned long dwCID );
	void	MoveTo( unsigned long dwCID, const Position& NewPos );

	// ===================================================================
	// CCharSphereNode 를 생성(캐릭터 제외)해서 재구성하는 함수 와 삭제하는 함수
	CCharSphereNode*	AddSphere( const Position& Pos, float fRadius, void* pLink, unsigned long dwFlag = SNF_LEAF_TREE );
	void				DeleteSphere( CCharSphereNode* pNode );

	// ===================================================================
	// 재계산, 재구성 리스트에 Node 추가 함수
	void	AddRecompute( CCharSphereNode* pNode );
	void	AddIntegrate( CCharSphereNode* pNode );

	// ===================================================================
	// Tree 를 재구성하는 함수
	void	Integrate( CCharSphereNode* pNode, CCharSphereNode* pSuperSphere, float fNodeSize );

	// ===================================================================
	// 범위 테스트용 함수
	void	RangeTest( const Position& centerPos, float fDistance, ICharSphereTreeCallBack* pCallBack );
	void	SendToRange( const Position& centerPos, float fDistance, const char* szPacket,
						 unsigned long dwPacketSize, unsigned char cCMD_In );

protected:
	// ===================================================================
	// 모든 자식 Node 를 메모리 해제하는 함수
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