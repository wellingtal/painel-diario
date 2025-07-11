#include "CharSphereTree.h"
#include <cassert>

#include <Creature/Character/Character.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

CCharSphereTree&	CCharSphereTree::GetInstance()
{
	static CCharSphereTree	ms_this( SphereConst::MAX_SPHERE_NODE, 
									 SphereConst::DEFAULT_ROOT_SIZE,
									 SphereConst::DEFAULT_LEAF_SIZE,
									 SphereConst::DEFAULT_GRAVY );
	return ms_this;
}

CCharSphereNode::CCharSphereNode() :
	m_dwCID(0),
	m_pCharacter(NULL),
	m_pParent(NULL),
	m_pChildren(NULL),
	m_pPrevSibling(NULL),
	m_pNextSibling(NULL),
	m_ppRecomuteFifo(NULL),
	m_ppIntegrateFifo(NULL),
	m_iChildCount(0),
	m_dwFlag(0),
	m_fBindingDistance2(0),
	m_pLink(NULL),
	m_pTree(NULL)
{
}

CCharSphereNode::CCharSphereNode( CCharSphereTree* pTree, const Position& Pos, float fRadius, void* pLink ) :
	CSphere(Pos, fRadius),
	m_dwCID(0),
	m_pCharacter(NULL),
	m_pParent(NULL),
	m_pChildren(NULL),
	m_pPrevSibling(NULL),
	m_pNextSibling(NULL),
	m_ppRecomuteFifo(NULL),
	m_ppIntegrateFifo(NULL),
	m_iChildCount(0),
	m_dwFlag(0),
	m_fBindingDistance2(0),
	m_pLink(pLink),
	m_pTree(pTree)
{
}

void
CCharSphereNode::Init( CCharSphereTree* pTree, const Position& Pos, float fRadius, void* pLink )
{
	m_pTree = pTree;
	m_Center = Pos;
	SetRadius( fRadius );
	m_pLink = pLink;
}

void
CCharSphereNode::AddChild( CCharSphereNode* pChild )
{
	CCharSphereNode* pOldChild = m_pChildren;
	m_pChildren = pChild;

	pChild->SetNextSibling( pOldChild );
	pChild->SetPrevSibling( NULL );
	pChild->SetParent( this );

	if ( pOldChild )
	{
		pOldChild->SetPrevSibling( pChild );
	}

	++m_iChildCount;

	float fDist = Distance2( pChild );
	float fRadius = sqrtf( fDist ) + pChild->GetRadius();

	assert( fRadius <= GetRadius() );
}

void
CCharSphereNode::DeleteChild( CCharSphereNode* pChild )
{
	assert( m_iChildCount );
	assert( m_pChildren );

#ifdef _DEBUG
	// pChild �� ���� ����� �ڽ����� üũ
	CCharSphereNode* pChildren = m_pChildren;
	bool bFound = false;

	while ( pChildren )
	{
		if ( pChildren == pChild )
		{
			assert( !bFound );
			bFound = true;
			break;
		}

		pChildren = pChildren->GetNextSibling();
	}

	assert( bFound );
#endif

	// Prev �� Next Sibling �ٽ� ����
	CCharSphereNode* pPrev = pChild->GetPrevSibling();
	if ( pPrev )
	{
		CCharSphereNode* pNext = pChild->GetNextSibling();
		pPrev->SetNextSibling( pNext );
		if ( pNext ) pNext->SetPrevSibling( pPrev );
	}
	else
	{
		CCharSphereNode* pNext = pChild->GetNextSibling();
		m_pChildren = pNext;
		if ( m_pChildren ) m_pChildren->SetPrevSibling( NULL );
	}

	--m_iChildCount;

	if ( 0 == m_iChildCount && HasSphereNodeFlag( SNF_SUPERSPHERE ) )
	{
		m_pTree->DeleteSphere( this );
	}
}

void
CCharSphereNode::Unlink()
{
	// Recompute & Integrate Fifo ���� �����Ѵ�.
	if ( m_ppRecomuteFifo )
	{
		*m_ppRecomuteFifo = NULL;
		m_ppRecomuteFifo = NULL;
	}

	if ( m_ppIntegrateFifo )
	{
		*m_ppIntegrateFifo = NULL;
		m_ppIntegrateFifo = NULL;
	}

	if ( m_pParent )
	{
		m_pParent->DeleteChild( this );
	}

	assert( !m_pChildren );

	m_pParent = NULL;
}

void
CCharSphereNode::NewPos( const Position& Pos )
{
	// ���ο� ��ǥ ����
	m_Center = Pos;

	// �θ� �ְ�, �籸���Ǿ�� �� �÷��װ� ���ٸ�, �籸���� ������ �Ǵ��Ѵ�.
	if ( m_pParent && !HasSphereNodeFlag( SNF_INTEGRATE ) )
	{
		// �θ���� �Ÿ��� �������� ����Ѵ�.
		float fDistance2 = Distance2( m_pParent );

		// �Ÿ��� ���ε� �Ÿ��� ��� ���
		if ( fDistance2 >= m_fBindingDistance2 )
		{
			// ���� �÷��װ� ���ٸ�, �θ��� ũ�Ⱑ ����Ǿ�� �Ѵ�.
			if ( !m_pParent->HasSphereNodeFlag( SNF_RECOMPUTE ) )
			{
				m_pTree->AddRecompute( m_pParent );
			}

			// �θ���� ������ ����, �ش� Root ��忡 �����Ѵ�.
			Unlink();
			m_pTree->AddIntegrate( this );
		}
	}
}

void
CCharSphereNode::NewPos( float fX, float fY, float fZ )
{
	NewPos( Position(fX, fY, fZ) );
}

void
CCharSphereNode::NewPosRadius( const Position& Pos, float fRadius )
{
	// ���ο� ��ǥ ����
	m_Center = Pos;

	// �θ� �ְ�, �籸���Ǿ�� �� �÷��װ� ���ٸ�, �籸���� ������ �Ǵ��Ѵ�.
	if ( m_pParent && !HasSphereNodeFlag( SNF_INTEGRATE ) )
	{
		// �������� ���� �Ǿ������� üũ�ؼ� ����Ǿ��ٸ�,
		// �������� �ٽ� �����ϰ�, ���ε� �Ÿ��� �����Ѵ�.
		if ( fRadius != GetRadius() )
		{
			SetRadius( fRadius );
			ComputeBindingDistance( m_pParent );
		}

		// �θ���� �Ÿ��� �������� ����Ѵ�.
		float fDistance2 = Distance2( m_pParent );

		// �Ÿ��� ���ε� �Ÿ��� ��� ���
		if ( fDistance2 >= m_fBindingDistance2 )
		{
			// ���� �÷��װ� ���ٸ�, �θ��� ũ�Ⱑ ����Ǿ�� �Ѵ�.
			if ( !m_pParent->HasSphereNodeFlag( SNF_RECOMPUTE ) )
			{
				m_pTree->AddRecompute( m_pParent );
			}

			// �θ���� ������ ����, �ش� Root ��忡 �����Ѵ�.
			Unlink();
			m_pTree->AddIntegrate( this );
		}
		// �ڽ��� �θ��� ���ε� �Ÿ��ȿ� ���� ���
		else
		{
			// �θ��� ���ε� �Ÿ��� ���ϼ� �������� �ֱ� ������, ������ �Ѵ�.
			if ( !m_pParent->HasSphereNodeFlag( SNF_RECOMPUTE ) )
			{
				m_pTree->AddRecompute( m_pParent );
			}
		}
	}
}

void
CCharSphereNode::NewPosRadius( float fX, float fY, float fZ, float fRadius )
{
	NewPosRadius( Position(fX, fY, fZ), fRadius );
}

float
CCharSphereNode::Distance2( const CCharSphereNode* pNode )
{
	return static_cast<float>( m_Center.GetSquaredDistance( pNode->GetCenter() ) );
}

void
CCharSphereNode::ComputeBindingDistance( const CCharSphereNode* pParent )
{
	m_fBindingDistance2 = pParent->GetRadius() - GetRadius();
	if ( m_fBindingDistance2 <= 0 ) m_fBindingDistance2 = 0;
	else m_fBindingDistance2 *= m_fBindingDistance2;
}

bool
CCharSphereNode::Recompute( float fGravy )
{
	if ( !m_pChildren ) return true;						// ���� ���
	if ( HasSphereNodeFlag( SNF_ROOT_NODE ) ) return false;	// Root �� �������� �ʴ´�.

	Position sumPos;
	int iCount = 0;

	// �ڽĵ��� ��ġ�� ��� ���ؼ� ����� ���ؼ�,
	// ���ο� �߽� ��ǥ�� ����� ����.
	CCharSphereNode* pChild = m_pChildren;
	while ( pChild )
	{
		sumPos.m_fPointX += pChild->GetCenter().m_fPointX;
		sumPos.m_fPointY += pChild->GetCenter().m_fPointY;
		sumPos.m_fPointZ += pChild->GetCenter().m_fPointZ;

		++iCount;

		pChild = pChild->GetNextSibling();
	}

	if ( iCount )
	{
		float fRecip = 1.0f / float(iCount);
		sumPos.m_fPointX *= fRecip;
		sumPos.m_fPointY *= fRecip;
		sumPos.m_fPointZ *= fRecip;

		Position oldCenter = m_Center;
		m_Center = sumPos;				// ���� ������ �߽� ��ǥ�� ����
		float fMaxRadius = 0;

		// �ڽĵ��� ���ԵǴ°� Ȯ���Ѵ�.
		pChild = m_pChildren;
		while ( pChild )
		{
			float fDistance2 = Distance2( pChild );
			float fRadius = sqrtf( fDistance2 ) + pChild->GetRadius();

			if ( fRadius > fMaxRadius )
			{
				fMaxRadius = fRadius;
				if ( (fMaxRadius + fGravy) >= GetRadius() )
				{
					m_Center = oldCenter;
					ClearShpereNodeFlag( SNF_RECOMPUTE );
					return false;
				}
			}

			pChild = pChild->GetNextSibling();
		}

		// �� Ŀ�� ���������� �� ����
		fMaxRadius += fGravy;
		SetRadius( fMaxRadius );

		// �ڽĵ��� ���ε� �Ÿ��� �� ���
		pChild = m_pChildren;
		while ( pChild )
		{
			pChild->ComputeBindingDistance( this );
			pChild = pChild->GetNextSibling();
		}
	}

	ClearShpereNodeFlag( SNF_RECOMPUTE );
	return false;
}


CCharSphereTree::CCharSphereTree( int iMaxSphereNode, float fRootSize, float fLeafSize, float fGravy ) :
	m_fMaxRootSize(fRootSize), m_fMaxLeafSize(fLeafSize), m_fSuperSphereGravy(fGravy)
{
	iMaxSphereNode *= 4;
	m_Recompute = new CCharSphereNodeFifo( iMaxSphereNode );
	m_Integrate = new CCharSphereNodeFifo( iMaxSphereNode );

	// Root Tree ����
	Position rootPos;
	m_pRoot = new CCharSphereNode();
	m_pRoot->Init( this, rootPos, 65535, NULL );
	m_pRoot->SetSphereNodeFlag( eSphereNodeFlag(SNF_ROOT_NODE | SNF_SUPERSPHERE | SNF_ROOT_TREE) );

	// Leaf Tree ����
	m_pLeaf = new CCharSphereNode();
	m_pLeaf->Init( this, rootPos, 16384, NULL );
	m_pLeaf->SetSphereNodeFlag( eSphereNodeFlag(SNF_ROOT_NODE | SNF_SUPERSPHERE | SNF_LEAF_TREE) );
}

CCharSphereTree::~CCharSphereTree()
{
	DeleteAllChildSphere( m_pRoot );
	DeleteAllChildSphere( m_pLeaf );

	SAFE_DELETE( m_pRoot );
	SAFE_DELETE( m_pLeaf );

	SAFE_DELETE( m_Recompute );
	SAFE_DELETE( m_Integrate );

	m_CharSphereMap.clear();
}

void
CCharSphereTree::Process()
{
	// ���� �Ǿ�� �� ��Ͽ� �ִ� ��带 �����Ѵ�.
	// ���� leaf node �� �θ� ����ٸ�, �θ� �� �����ǰų�, �ڽ��� ���ٸ�, ���ŵǾ�� �Ѵ�.
	if ( m_Recompute->GetCount() > 0 )
	{
		bool bKill;
		CCharSphereNode* pTempNode = NULL;

		int iCount = m_Recompute->GetCount();
		for (int i=0; i<iCount; ++i)
		{
			pTempNode = m_Recompute->Pop();
			if ( !pTempNode ) continue;
			pTempNode->SetRecomputeFifo( NULL );

			bKill = pTempNode->Recompute( m_fSuperSphereGravy );
			if ( bKill )
			{
				DeleteSphere( pTempNode );
			}
		}
	}

	// �籸�� �Ǿ�� �� ��Ͽ� �ִ� ��带 �籸���Ѵ�.
	if ( m_Integrate->GetCount() > 0 )
	{
		CCharSphereNode* pTempNode = NULL;

		int iCount = m_Integrate->GetCount();
		for (int i=0; i<iCount; ++i)
		{
			pTempNode = m_Integrate->Pop();
			if ( !pTempNode ) continue;
			pTempNode->SetIntegrateFifo( NULL );

			if ( pTempNode->HasSphereNodeFlag( SNF_ROOT_TREE ) )
				Integrate( pTempNode, m_pRoot, m_fMaxRootSize ); // integrate this one single dude against the root node.
			else
				Integrate( pTempNode, m_pLeaf, m_fMaxLeafSize ); // integrate this one single dude against the root node.
		}
	}
}

bool
CCharSphereTree::AddCharacter( unsigned long dwCID, CCharacter* pCharacter, const Position& Pos )
{
	CCharSphereNode* pNode = AddSphere( Pos, SphereConst::CHAR_RADIUS, NULL );
	if ( pNode )
	{
		pNode->SetCID( dwCID );
		pNode->SetCharacter( pCharacter );
		pNode->SetSphereNodeFlag( SNF_ACTOR_NODE );

		return m_CharSphereMap.insert( std::make_pair(dwCID, pNode) ).second;
	}

	return false;
}

bool
CCharSphereTree::DeleteCharacter( unsigned long dwCID )
{
	CharSphereMap::iterator itr = m_CharSphereMap.find( dwCID );
	if ( itr != m_CharSphereMap.end() )
	{
		CCharSphereNode* pNode = itr->second;
		if ( pNode )
		{
			DeleteSphere( pNode );
		}

		m_CharSphereMap.erase( itr );
		return true;
	}

	return false;
}

void
CCharSphereTree::MoveTo( unsigned long dwCID, const Position& NewPos )
{
	CharSphereMap::iterator itr = m_CharSphereMap.find( dwCID );
	if ( itr != m_CharSphereMap.end() )
	{
		CCharSphereNode* pNode = itr->second;
		if ( pNode && pNode->GetCenter() != NewPos )
		{
			pNode->NewPos( NewPos );
		}
	}
}

CCharSphereNode*
CCharSphereTree::AddSphere( const Position& Pos, float fRadius, void* pLink, unsigned long dwFlag )
{
	// ���ο� ��带 �����.
	CCharSphereNode* pNode = new CCharSphereNode();
	assert( pNode );

	if ( pNode )
	{
		// Root �� Leaf �߿� ������ ����� �ٿ��ְ�, �� ���� ����Ʈ�� �ִ´�.
		if ( dwFlag & SNF_ROOT_TREE )
		{
			pNode->Init( this, Pos, fRadius, pLink );
			pNode->SetSphereNodeFlag( SNF_ROOT_TREE );
			AddIntegrate( pNode );
		}
		else
		{
			pNode->Init( this, Pos, fRadius, pLink );
			pNode->SetSphereNodeFlag( SNF_LEAF_TREE );
			AddIntegrate( pNode );
		}
	}

	return pNode;
}

void
CCharSphereTree::DeleteSphere( CCharSphereNode* pNode )
{
	// Root node �� ����ڰ� �����ش�.
	if ( pNode->HasSphereNodeFlag( SNF_ROOT_NODE ) ) return;

	// Leaf node tree �� supersphere �� ��ũ�� root node tree �� node �� �����ش�.
	if ( pNode->HasSphereNodeFlag( SNF_SUPERSPHERE ) && pNode->HasSphereNodeFlag( SNF_LEAF_TREE) )
	{
		CCharSphereNode* pLink = (CCharSphereNode*) pNode->GetLink();
		DeleteSphere( pLink );
	}

	pNode->Unlink();
	SAFE_DELETE( pNode );
}

void
CCharSphereTree::DeleteAllChildSphere( CCharSphereNode* pRootNode )
{
	CCharSphereNode* pTempNode = NULL;
	CCharSphereNode* pChild = pRootNode->GetChildren();
	while ( pChild )
	{
		if ( pChild->GetChildCount() )
		{
			DeleteAllChildSphere( pChild );
		}

		pTempNode = pChild;
		pChild = pChild->GetNextSibling();

		SAFE_DELETE( pTempNode );
	}
}

void
CCharSphereTree::AddRecompute( CCharSphereNode* pNode )
{
	if ( !pNode->HasSphereNodeFlag( SNF_RECOMPUTE ) )
	{
		if ( pNode->GetChildCount() )
		{
			pNode->SetSphereNodeFlag( SNF_RECOMPUTE );
			CCharSphereNode** ppFifo = m_Recompute->Push( pNode );
			pNode->SetRecomputeFifo( ppFifo );
		}
		else
		{
			DeleteSphere( pNode );
		}
	}
}

void
CCharSphereTree::AddIntegrate( CCharSphereNode* pNode )
{
	if ( pNode->HasSphereNodeFlag( SNF_ROOT_TREE ) )
	{
		m_pRoot->AddChild( pNode );
	}
	else
	{
		m_pLeaf->AddChild( pNode );
	}

	pNode->SetSphereNodeFlag( SNF_INTEGRATE );
	CCharSphereNode** ppFifo = m_Integrate->Push( pNode );
	pNode->SetIntegrateFifo( ppFifo );
}

void
CCharSphereTree::Integrate( CCharSphereNode* pNode, CCharSphereNode* pSuperSphere, float fNodeSize )
{
	// �籸�� �ܰ� ù��°�� pNode �� ���� ������ �ִ� SuperSphere �� ã�´�.
	CCharSphereNode* pSearch = pSuperSphere->GetChildren();

	CCharSphereNode* pNearestNode1st = NULL;	// pNode �� ������ �ѷ��Ҽ� �ִ� SuperSphere
	CCharSphereNode* pNearestNode2nd = NULL;	// pNode �� �߰��ϱ� ���� �ణ ũ�⸦ �����ؾ� �ϴ� SuperSphere
	float fNearDist1st2 = 1e9;
	float fNearDist2nd2 = 1e9;

	while ( pSearch )
	{
		if ( !pSearch->HasSphereNodeFlag( SNF_ROOT_NODE ) &&
			pSearch->HasSphereNodeFlag( SNF_SUPERSPHERE ) &&
			pSearch->GetChildCount() )
		{
			float fDistance2 = pNode->Distance2( pSearch );

			if ( pNearestNode1st )
			{
				// ã�� ���� ������ �����ϴ� SuperSphere ���� �� ����� �༮�� ã�� ���
				if ( fDistance2 < fNearDist1st2 )
				{
					float fDist = sqrtf( fDistance2 ) + pNode->GetRadius();
					if ( fDist <= pSearch->GetRadius() )
					{
						pNearestNode1st = pSearch;
						fNearDist1st2 = fDistance2;
					}
				}
			}
			else
			{
				float fDist = sqrtf( fDistance2 ) + pNode->GetRadius() - pSearch->GetRadius();
				if ( fDist < fNearDist2nd2 )
				{
					if ( fDist < 0 )
					{
						// ������ �����ϴ� SuperSphere �� ã�� ���
						pNearestNode1st = pSearch;
						fNearDist1st2 = fDistance2;
					}
					else
					{
						// ������ ���������� ��������, �������� Ű���� ������ �� �ִ�
						// ���ɼ��� �ִ� SuperSphere �� ã�� ���
						pNearestNode2nd = pSearch;
						fNearDist2nd2 = fDist;
					}
				}
			}
		}

		pSearch = pSearch->GetNextSibling();
	}

	// pNode �� ������ ������ �� �ִ� SuperSphere �� ã�� ���
	if ( pNearestNode1st )
	{
		// pNode �� pNearestNode1st �� �ڽ����� �ٿ��ָ� �ȴ�.
		pNode->Unlink();
		pNearestNode1st->AddChild( pNode );
		pNearestNode1st->Recompute( m_fSuperSphereGravy );
		pNode->ComputeBindingDistance( pNearestNode1st );

		// ���� leaf node �� supersphere ���, root tree �� link node ���� ��ǥ�� ũ�⸦ �������ش�.
		if ( pNearestNode1st->HasSphereNodeFlag( SNF_LEAF_TREE ) )
		{
			CCharSphereNode* pLink = (CCharSphereNode*) pNearestNode1st->GetLink();
			pLink->NewPosRadius( pNearestNode1st->GetCenter(), pNearestNode1st->GetRadius() );
		}
	}
	else
	{
		bool bCreateNewSphere = true;

		// ũ�⸦ �ణ Ű��� pNode �� ������ �� �������� SuperSphere �� ã�� ���
		if ( pNearestNode2nd )
		{
			float fNewSize = fNearDist2nd2 + pNearestNode2nd->GetRadius() + m_fSuperSphereGravy;

			if ( fNewSize <= fNodeSize )
			{
				pNode->Unlink();

				pNearestNode2nd->SetRadius( fNewSize );
				pNearestNode2nd->AddChild( pNode );
				pNearestNode2nd->Recompute( m_fSuperSphereGravy );
				pNode->ComputeBindingDistance( pNearestNode2nd );

				if ( pNearestNode2nd->HasSphereNodeFlag( SNF_LEAF_TREE ) )
				{
					CCharSphereNode* pLink = (CCharSphereNode*) pNearestNode2nd->GetLink();
					pLink->NewPosRadius( pNearestNode2nd->GetCenter(), pNearestNode2nd->GetRadius() );
				}

				bCreateNewSphere = false;
			}
		}

		// ���ο� SuperSphere �� ������ �ϴ� ���
		if ( bCreateNewSphere )
		{
			assert( pSuperSphere->HasSphereNodeFlag( SNF_ROOT_NODE ) );

			pNode->Unlink();

			CCharSphereNode* pParent = new CCharSphereNode();
			assert( pParent );
			pParent->Init( this, pNode->GetCenter(), pNode->GetRadius() + m_fSuperSphereGravy, NULL );

			if ( pSuperSphere->HasSphereNodeFlag( SNF_ROOT_TREE ) )
			{
				pParent->SetSphereNodeFlag( eSphereNodeFlag(SNF_SUPERSPHERE | SNF_ROOT_TREE) );
			}
			else
			{
				pParent->SetSphereNodeFlag( eSphereNodeFlag(SNF_SUPERSPHERE | SNF_LEAF_TREE) );
			}

			pParent->AddChild( pNode );
			pSuperSphere->AddChild( pParent );
			pParent->Recompute( m_fSuperSphereGravy );
			pNode->ComputeBindingDistance( pParent );

			if ( pParent->HasSphereNodeFlag( SNF_LEAF_TREE ) )
			{
				// root node �� link �� �����ؾ� �Ѵ�.
				CCharSphereNode* pLink = AddSphere( pParent->GetCenter(), pParent->GetRadius(), pParent, SNF_ROOT_TREE );
				pParent->SetLink( pLink );
			}
		}
	}

	pNode->ClearShpereNodeFlag( SNF_INTEGRATE );
}

void
CCharSphereNode::RangeTest( const Position& centerPos, float fDistance, ICharSphereTreeCallBack* pCallBack )
{
	float fDist = static_cast<float>( m_Center.GetDistance( centerPos ) );
	if ( (fDist - fDistance) > GetRadius() ) return;

	if ( HasSphereNodeFlag( SNF_SUPERSPHERE ) )
	{
		CCharSphereNode* pNode = m_pChildren;
		while ( pNode )
		{
			pNode->RangeTest( centerPos, fDistance, pCallBack );
			pNode = pNode->GetNextSibling();
		}
	}
	else
	{
		if ( HasSphereNodeFlag( SNF_ROOT_TREE ) )
		{
			CCharSphereNode* pLink = (CCharSphereNode*) GetLink();
			if ( pLink )
			{
				pLink->RangeTest( centerPos, fDistance, pCallBack ) ;
			}
		}
		else
		{
			pCallBack->RangeTestCallBack( centerPos, fDistance, this );
		}
	}
}

void
CCharSphereTree::RangeTest( const Position& centerPos, float fDistance, ICharSphereTreeCallBack* pCallBack )
{
	m_pRoot->RangeTest( centerPos, fDistance, pCallBack );
}

void
CCharSphereNode::SendToRange( const Position& centerPos, float fDistance, const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In )
{
	float fDist = static_cast<float>( m_Center.GetDistance( centerPos ) );
	if ( (fDist - fDistance) > GetRadius() ) return;

	if ( HasSphereNodeFlag( SNF_SUPERSPHERE ) )
	{
		CCharSphereNode* pNode = m_pChildren;
		while ( pNode )
		{
			pNode->SendToRange( centerPos, fDistance, szPacket, dwPacketSize, cCMD_In );
			pNode = pNode->GetNextSibling();
		}
	}
	else
	{
		if ( HasSphereNodeFlag( SNF_ROOT_TREE ) )
		{
			CCharSphereNode* pLink = (CCharSphereNode*) GetLink();
			if ( pLink )
			{
				pLink->SendToRange( centerPos, fDistance, szPacket, dwPacketSize, cCMD_In ) ;
			}
		}
		else
		{
			// Packet ����
			if (NULL != m_pCharacter)
			{
				CGameClientDispatch* lpDispatcher = m_pCharacter->GetDispatcher();
				if (NULL != lpDispatcher)
				{
					lpDispatcher->GetSendStream().PutBuffer( szPacket, dwPacketSize, cCMD_In );
				}
			}
		}
	}
}

void
CCharSphereTree::SendToRange( const Position& centerPos, float fDistance, const char* szPacket,	unsigned long dwPacketSize, unsigned char cCMD_In )
{
	m_pRoot->SendToRange( centerPos, fDistance, szPacket, dwPacketSize, cCMD_In );
}