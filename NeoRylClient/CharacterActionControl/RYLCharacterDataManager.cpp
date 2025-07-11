//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCharacterDataManager.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: contain and manage characters's data
//
// Date: 2004. 3. 26. ( Fri )
//--------------------------------------------------------------------------------------------------------------------------

#include "RYLCharacterDataManager.h"
#include <algorithm>
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// functors
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// ftDeleteCharDataMemory() : Container 원소 메모리 날리기
static void ftDeleteCharDataMemory( CharDataNode* pObject )
{
    delete pObject;
    pObject = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// FindFreedNode() : Cantainer의 원소들 중에 쓰이지 않는 노드 찾기
static bool ftFindFreeNode( const CharDataNode* node )
{
    if( !node->m_bIsUsingNode )
        return true;

    return false;
}


//--------------------------------------------------------------------------------------------------------------------------
// class ftFindCharDataByID : ID로 CharData list에서 node 찾기
class ftFindCharDataByID
{
private:
    unsigned long m_ulCharID;

public:
    ftFindCharDataByID( unsigned long ulCharID ) : m_ulCharID( ulCharID ) 
    {
    }

    bool operator()( const CharDataNode* node ) const
    {
        if( node->m_ulCharID == m_ulCharID )
            return true;

        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCharacterDataManager
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
RYLCharacterDataManager*   RYLCharacterDataManager::m_pInstance = NULL;   // initialize static member variable

//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
RYLCharacterDataManager::RYLCharacterDataManager( unsigned int unCharPoolMax )
{
    m_ulCharCount       = 0;
    
    InitCharDataManager( unCharPoolMax );
}

RYLCharacterDataManager::~RYLCharacterDataManager()
{
    DeleteAllCharData();
}

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::InitCharDataManager()
void RYLCharacterDataManager::InitCharDataManager( unsigned int unMaxCash )
{
    // clear pool
    if( !m_lstCharData.empty() )
    {
        for_each( m_lstCharData.begin(), m_lstCharData.end(), ftDeleteCharDataMemory );
    }

    // allocate pool
    m_unMaxCash = unMaxCash;
    /*for( unsigned int i = 0 ; i < m_unMaxCash ; ++i )
    {
        CharDataNode*   pNode = new CharDataNode;
        m_lstCharData.push_back( pNode );
    }*/
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::AddCharData()
CharDataNode* RYLCharacterDataManager::AddCharData( CharDataNode* pNode )
{
    // if added character is already exist in list, return it's node ID
    list<CharDataNode*>::iterator iter = find_if( m_lstCharData.begin(), m_lstCharData.end(), ftFindCharDataByID( pNode->m_ulCharID ) );

    if( iter != m_lstCharData.end() )
    {
        delete pNode;
        pNode = NULL;

        (*iter)->m_bIsUsingNode = true;

        return (*iter);
    }

    ++m_ulCharCount;
    pNode->m_ulNodeID       = m_ulCharCount;
    pNode->m_bIsUsingNode   = true;          // turn on 'using flag'

    // find free node
    iter = find_if( m_lstCharData.begin(), m_lstCharData.end(), ftFindFreeNode );

    if( iter != m_lstCharData.end() )
    {
        // if free node exist
        *(*iter) = *pNode;

		delete pNode ;
		pNode = NULL ;
		pNode = (*iter) ;
    }
    else
    {
        m_lstCharData.push_back( pNode );
    }

    return pNode;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::DeleteCharData()
void RYLCharacterDataManager::DeleteCharData( unsigned long ulCharID )
{
    list<CharDataNode*>::iterator iter = find_if( m_lstCharData.begin(), m_lstCharData.end(), ftFindCharDataByID( ulCharID ) );

    if( iter != m_lstCharData.end() )
    {
        if( m_lstCharData.size() > m_unMaxCash )
        {
            ftDeleteCharDataMemory( (*iter) );
            m_lstCharData.erase( iter );
        }
        else
        {
            (*iter)->m_bIsUsingNode = false;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::GetCharData()
CharDataNode* RYLCharacterDataManager::GetCharData( unsigned long ulCharID )
{
    list<CharDataNode*>::iterator iter = find_if( m_lstCharData.begin(), m_lstCharData.end(), ftFindCharDataByID( ulCharID ) );

    if( iter != m_lstCharData.end() && (*iter)->m_bIsUsingNode )
        return (*iter);

    return NULL;
}

CharDataNode* RYLCharacterDataManager::GetCharData( char* szName )
{
	list<CharDataNode*>::iterator pos = m_lstCharData.begin();
	list<CharDataNode*>::iterator end = m_lstCharData.end();

	for(; pos!=end; ++pos)
	{
		if(strcmp((*pos)->m_strName.c_str(), szName)==0)		
			return (*pos);
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::InitCharData()
CharDataNode* RYLCharacterDataManager::InitCharData( unsigned long ulCharID, char cRace )
{
	CharDataNode* pData = GetCharData( ulCharID ) ;

	if ( NULL == pData )
	{
		return NULL ;
	}

	pData->m_strName.clear() ;
	pData->m_cSex		= 1 ;
	pData->m_cHair		= 2 ;
	pData->m_cFace		= 1 ;
	pData->m_cNation	= 0 ;
	pData->m_cRace		= cRace ;
	pData->m_usClass	= 0 ;

	if ( 0 == cRace )
	{
        pData->m_cNation = 1 ;
		pData->m_usClass = 1 ;
	}
	else
	{
		pData->m_cNation = 2 ;
		pData->m_usClass = 17 ;
	}

	pData->m_ulFame		= 0 ;
	pData->m_ulMileage	= 0 ;
	pData->m_ulGuildID	= 0 ;
	pData->m_ulPartyID	= 0 ;
	pData->m_ulTeamID	= 0 ;

	pData->m_cLevel		= 0 ;
	pData->m_lCurrHP	= 0 ;
	pData->m_lCurrMP	= 0 ;
	pData->m_ulMaxHP	= 0 ;
	pData->m_ulMaxMP	= 0 ;

	pData->m_bPeaceMode			= false ;
	pData->m_nChrPattern		= 0 ;
	pData->m_strChatMessage.clear() ;

	pData->m_ulTargetCharID        = 0xFFFFFFFF ;
	pData->m_ulSummonMasterCharID  = 0xFFFFFFFF ;
	pData->m_strStreetStallName.clear() ;

	return pData ;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::DeleteAllCharData()
void RYLCharacterDataManager::DeleteAllCharData()
{
    for_each( m_lstCharData.begin(), m_lstCharData.end(), ftDeleteCharDataMemory );
    m_lstCharData.clear();

    m_pSelfData = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::SwapPartyID()
void RYLCharacterDataManager::SwapPartyID( unsigned long ulOldPartyID, unsigned long ulNewPartyID )
{
    for( list<CharDataNode*>::iterator iter = m_lstCharData.begin() ; iter != m_lstCharData.end() ; ++iter )
    {
        if( (*iter)->m_ulPartyID == ulOldPartyID )
            (*iter)->m_ulPartyID = ulNewPartyID;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCharacterDataManager::