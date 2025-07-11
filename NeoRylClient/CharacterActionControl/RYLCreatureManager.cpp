//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreatureManager.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: Creature Managerment Class
//
// Date: 2004. 4. 7. ( Wen )
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------

#include <winsock2.h>
#include "WinInput.h"

#include "RYLObjectControl.h"

#include "SceneManager.h"
#include "BaseGraphicsLayer.h"

#include "RYLCreatureCommon.h"
#include "RYLCharacterDataManager.h"
#include "RYLCreatureManager.h"

#include "..\\Effect\\CEffscript.h"
#include "RYLGameData.h"

#include <Creature/Siege/SiegeConstants.h>
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// stataic memeber function pointer initilaiRYLe

char*           (*RYLCreatureManager::GetMotionSheet)( char* pstrWeaponName, char* pstrShieldName, unsigned short usClass ) = NULL;
unsigned long   (*RYLCreatureManager::GetWeaponType)( char* strWeaponName ) = NULL;
unsigned long   (*RYLCreatureManager::GetClientIdentity)( unsigned long dwChrID ) = NULL;
unsigned long   (*RYLCreatureManager::GetClientEnemyType)( unsigned long dwChrID) = NULL;


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreatureManager
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
RYLCreatureManager* RYLCreatureManager::m_pInstance = NULL; // initialize static member variable

//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
RYLCreatureManager::RYLCreatureManager( unsigned int unMaxCash )
{
    m_pSelfCreature         = NULL;
    m_mapCreature.clear();
    m_lstCharModel.clear();    
	m_ArmsMap.clear();

    // alloc cash
    m_unMaxCash             = unMaxCash;

    m_pObjControl           = NULL;

    m_bGoodChantEffectShow  = true;
    m_bBadChantEffectShow   = true;
}

RYLCreatureManager::~RYLCreatureManager()
{
    ClearCreatureManager();
}

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::AddCreature()
unsigned long RYLCreatureManager::AddCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName,
											   vector3 vecPosition, float fDirection, bool bCollision, unsigned long dwNation,
											   int iValue, CZ3DGeneralChrModel **ppOld, bool bHouse, int iHouseType, D3DXVECTOR3 vecPos )
{
    // 같은 ID의 creature가 이미 있으면 실패
    RYLCreature* pTempCreature = GetCreature( ulCharID );
	if( pTempCreature )
	{
		return 0;
	}

	// 사용 가능 NPC인지를 체크한다.
	if (Creature::CT_NPC == GetClientIdentity(ulCharID))
	{
		if (NULL == g_NPCList.GetNPCNode(ulCharID))
		{
			return 0;
		}
	}

    // search usable creature
    map<unsigned long,RYLCreature*>::iterator iterCreature;
    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->GetMemoryReusable() )
            break;
    }

    // 사용 가능한 creature가 있다.
    if( iterCreature != m_mapCreature.end() )
    {    
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( pSelfCreature->GetCharID() == iterCreature->second->GetCharID() )
        {
            int i = 0;
            i = 1;
        }

        pTempCreature = iterCreature->second;

        // creature 초기화
        pTempCreature->InitCreature();

        FreeModel( pTempCreature->GetCharModel() );
        pTempCreature->SetCharModel( NULL );

        pTempCreature->m_ulCharID       = ulCharID;

        pTempCreature->m_strCharName    = pstrName;
        pTempCreature->m_strGCMDSName   = pstrGCMDSName;

        pTempCreature->m_vecPosition    = vecPosition+vector3(0, 1.0f, 0);
        pTempCreature->m_fDirection     = fDirection;

        pTempCreature->m_strFaceType    = "FACE01";
        pTempCreature->m_strHairType    = "HAIR01";

        // 기존에 map 상에 있던 원소는 제거해준다.( 새로 집어 넣을꺼니까 )
        m_mapCreature.erase( iterCreature );
    }
    // 사용 가능한 creature가 없다.
    else 
    {
        // creature 생성
        pTempCreature = new RYLCreature( ulCharID, pstrName, pstrGCMDSName, vecPosition, fDirection );
    }

    pTempCreature->SetIsUsed( true );
    pTempCreature->SetMemoryReusable( false );

    // create character model
    CZ3DGeneralChrModel* pTempModel = AllocModel( pTempCreature, bHouse, iHouseType, vecPos );
	pTempModel->SetPosition( vecPosition );
	pTempModel->SetDirection( fDirection );
	pTempModel->SetLODLevel( 0 );
	
    switch( GetClientIdentity( ulCharID ) )
	{
		case Creature::CT_PC:				
            {
                CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
			    pTempCreature->SetCDTType( CDT_NONE );
            }
			break;
                                                    
		case Creature::CT_NPC:				
			{
				CCharacterLightShadowManager::CCharacterDataNode CheckNode = CSceneManager::GetCharacterDataNode( pTempModel );					
				CheckNode.m_CollisionType = CT_NONE;					 
				CSceneManager::SetCharacterDataNode( CheckNode );				
				CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FIRSTBOTTOM );

                vecPosition.y -= 30.0f;
			    pTempModel->SetPosition( vecPosition );

			    pTempCreature->SetCDTType( CDT_FIRSTBOTTOM );
			}
			break;

		case Creature::CT_MONSTER:		
		case Creature::CT_STRUCT:
            {
			    if ( bCollision )
			    {
				    CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FIRSTBOTTOM );
				    vecPosition.y -= 30.0f;
				    pTempModel->SetPosition( vecPosition );
				
                    pTempCreature->SetCDTType( CDT_FIRSTBOTTOM );
			    }
			    else
			    {
				    CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_ONLYTERRAIN );
				    
                    pTempCreature->SetCDTType( CDT_ONLYTERRAIN );
			    }
            }
			break;
		case Creature::CT_SIEGE_OBJECT:
			{
				if ( bCollision )
				{
					CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FULL );
					vecPosition.y -= 30.0f;
					pTempModel->SetPosition( vecPosition );

					pTempCreature->SetCDTType( CDT_FULL );
				}
				else
				{
					CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FULL );

					pTempCreature->SetCDTType( CDT_FULL );
				}
			}
			break;
	}		

    // 빈 데이터를 data manager에 삽입
    CharDataNode* pTempData     = new CharDataNode;
    pTempData->m_strName        = pstrName;
    pTempData->m_ulCharID       = pTempCreature->m_ulCharID;
	pTempData->m_cNation		= static_cast<char>(dwNation);

    pTempData = RYLCharacterDataManager::Instance()->AddCharData( pTempData );
    m_mapCreature.insert( make_pair( pTempCreature->GetCharID(), pTempCreature ) );

    return pTempCreature->m_ulAddedID;
}

unsigned long RYLCreatureManager::AddCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName, 
                                               const char* pstrFaceType, const char* pstrHairType, 
											   const char** pstrShape, const Broadcast2nd::CNetworkEquipGrade& equipGrade, 
                                               unsigned char ucRace, vector3 vecPosition, float fDirection, bool bCollision,
                                               int iValue, CZ3DGeneralChrModel **ppOld, bool bHouse, int iHouseType, D3DXVECTOR3 vecPos )
{
    // 같은 ID의 creature가 이미 있으면 실패
    RYLCreature* pTempCreature = GetCreature( ulCharID );
    if( pTempCreature )
        return 0;

	// 사용 가능 NPC인지를 체크한다.
	if (Creature::CT_NPC == GetClientIdentity(ulCharID))
	{
		if (NULL == g_NPCList.GetNPCNode(ulCharID))
		{
			return 0;
		}
	}

    // search usable creature
    map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.begin();
    for( ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->GetMemoryReusable() )
            break;
    }

    if( iterCreature != m_mapCreature.end() )
    {
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( pSelfCreature->GetCharID() == iterCreature->second->GetCharID() )
        {
            int i = 0;
            i = 1;
        }

        pTempCreature = iterCreature->second;

        pTempCreature->InitCreature();

        FreeModel( pTempCreature->GetCharModel() );
        pTempCreature->SetCharModel( NULL );

        pTempCreature->m_ulCharID     = ulCharID;

        pTempCreature->m_strCharName  = pstrName;
        pTempCreature->m_strGCMDSName = pstrGCMDSName;
        pTempCreature->m_strFaceType  = pstrFaceType;
        pTempCreature->m_strHairType  = pstrHairType;

		//std::copy(pstrShape, pstrShape + Item::EquipmentPos::MAX_EQUPMENT_POS, pTempCreature->m_strShape);
		for(int i = 0 ; i < Item::EquipmentPos::MAX_EQUPMENT_POS ; ++i)
			pTempCreature->m_strShape[i] = NULL;

        pTempCreature->m_ucRace       = ucRace;
        pTempCreature->m_vecPosition  = vecPosition+vector3(0, 1.0f, 0);
        pTempCreature->m_fDirection   = fDirection;

        // 기존에 map 상에 있던 원소는 제거해준다.( 새로 집어 넣을꺼니까 )
        m_mapCreature.erase( iterCreature );
    }
    else
    {
        pTempCreature = new RYLCreature( ulCharID, pstrName, pstrGCMDSName, pstrFaceType, pstrHairType, pstrShape, 
			ucRace, vecPosition, fDirection );
    }
/*
	char ucSex = 1;
	if(strcmp("PC_MAN.GCMDS",pstrGCMDSName) == 0){ 
		ucSex = 1;
	}
	else if( strcmp("PC_Akhan_A.GCMDS",pstrGCMDSName) == 0){ 
		ucSex = 3;
	}
	else if ( strcmp("PC_Akhan_B.GCMDS",pstrGCMDSName) == 0){
		ucSex = 4;
	}
	else{
		ucSex = 2;
	}
*/



		char ucSex = 1;
	if(strcmp("PC_MAN.GCMDS",pstrGCMDSName) == 0 || strcmp("PC_Akhan_A.GCMDS",pstrGCMDSName) == 0)
		ucSex = 1;
	else 
		ucSex = 2;




    pTempCreature->SetIsUsed( false );
    pTempCreature->SetMemoryReusable( false );

    // create character model
    CZ3DGeneralChrModel* pTempModel = AllocModel( pTempCreature, bHouse, iHouseType, vecPos );
	pTempModel->SetPosition( vecPosition );
	pTempModel->SetDirection( fDirection );
	pTempModel->SetLODLevel( 0 );

    switch( GetClientIdentity( ulCharID ) )
	{
		case Creature::CT_PC:				
            {
                CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
			    pTempCreature->SetCDTType( CDT_NONE );
            }
			break;

		case Creature::CT_NPC:				
			{
				CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FIRSTBOTTOM );
                //vecPosition.y -= 50.0f;
			    pTempModel->SetPosition( vecPosition );

			    pTempCreature->SetCDTType( CDT_FIRSTBOTTOM );
			}
			break;

		case Creature::CT_MONSTER:
		case Creature::CT_STRUCT:
            {
			    if ( bCollision )
			    {
				    CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FIRSTBOTTOM );
				    pTempModel->SetPosition( vecPosition );
				
                    pTempCreature->SetCDTType( CDT_FIRSTBOTTOM );
			    }
			    else
			    {
				    CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_ONLYTERRAIN );
				    
                    pTempCreature->SetCDTType( CDT_ONLYTERRAIN );
			    }
            }
			break;

		case Creature::CT_SIEGE_OBJECT:
			{
				CSceneManager::CharacterCollisionDetectType( pTempModel,CDT_FULL );
				pTempCreature->SetCDTType( CDT_FULL );
			}
			break;
	}		

    // 빈 데이터를 data manager에 삽입
    CharDataNode* pTempData = new CharDataNode;
    pTempData->m_strName    = pstrName;
    pTempData->m_ulCharID   = pTempCreature->m_ulCharID;
    pTempData->m_cRace      = ucRace;                              
	pTempData->m_cSex		= ucSex;

    pTempData = RYLCharacterDataManager::Instance()->AddCharData( pTempData );

    pTempCreature->SetShape( pstrShape, equipGrade, true );

    m_mapCreature.insert( make_pair( pTempCreature->GetCharID(), pTempCreature ) );

    return pTempCreature->m_ulAddedID;
}

unsigned long RYLCreatureManager::AddCreature( unsigned long ulCharID, vector3 vecPosition,
                                               int iValue, CZ3DGeneralChrModel **ppOld, bool bHouse, int iHouseType, D3DXVECTOR3 vecPos )
{
    // 같은 ID의 creature가 이미 있으면 실패
    RYLCreature* pTempCreature = GetCreature( ulCharID );
    if( pTempCreature )
        return 0;

	// 사용 가능 NPC인지를 체크한다.
	if (Creature::CT_NPC == GetClientIdentity(ulCharID))
	{
		if (NULL == g_NPCList.GetNPCNode(ulCharID))
		{
			return 0;
		}
	}

    map<unsigned long,RYLCreature*>::iterator iterCreature;// = m_mapCreature.end();
    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->GetMemoryReusable() )
            break;
    }

    if( iterCreature != m_mapCreature.end() )
    {
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( pSelfCreature->GetCharID() == iterCreature->second->GetCharID() )
        {
            int i = 0;
            i = 1;
        }

        pTempCreature = iterCreature->second;

        pTempCreature->InitCreature();

		vector3 vpos = vecPosition+vector3(0, 1.0f, 0);
        pTempCreature->SetPosition( vpos );

        FreeModel( pTempCreature->GetCharModel() );
        pTempCreature->SetCharModel( NULL );

        // 기존에 map 상에 있던 원소는 제거해준다.( 새로 집어 넣을꺼니까 )
        m_mapCreature.erase( iterCreature );
    }
    else
    {
        pTempCreature = new RYLCreature( ulCharID, vecPosition );
    }

    pTempCreature->SetIsUsed( false );
    pTempCreature->SetMemoryReusable( false );

    pTempCreature->m_pCharModel = NULL;

    
    // 빈 데이터를 data manager에 삽입
    CharDataNode* pTempData = new CharDataNode;
    pTempData->m_ulCharID = pTempCreature->m_ulCharID;
    
    pTempData = RYLCharacterDataManager::Instance()->AddCharData( pTempData );

    m_mapCreature.insert( make_pair( pTempCreature->GetCharID(), pTempCreature ) );

    return pTempCreature->m_ulAddedID;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::AddSiegeCreature()
RYLCreature* RYLCreatureManager::AddSiegeCreature( RYLCreature* pSiegeCreature, 
                                                   unsigned long ulCharID, 
                                                   const char* pstrName, 
                                                   vector3 vecPosition, 
                                                   float fDirection, 
                                                   const char* pstrType )
{
    //if( pSiegeCreature )
    //    return pSiegeCreature;

    string strGCMDSName;
    unsigned short usObjectType;
    if ( !strcmp( pstrType, "LongSeigeWeapon" ) )
	{
        strGCMDSName = "SIEGE_Catapult.GCMDS";
        usObjectType = Siege::LONG_RANGE_SIEGE_ARMS;
	}
	else if ( !strcmp( pstrType, "ShortSiegeWeapon" ) )
	{
        strGCMDSName = "Siege_WallBreaker.GCMDS";
        usObjectType = Siege::SHORT_RANGE_SIEGE_ARMS;
	}
	else if ( !strcmp( pstrType, "LongCastleWeapon" ) )
	{
        strGCMDSName = "SIEGE_Catapult.GCMDS";
        usObjectType = Siege::LONG_RANGE_CASTLE_ARMS;
	}
	else if ( !strcmp( pstrType, "ShortCastleWeapon" ) )
	{
        strGCMDSName = "Siege_Guard.GCMDS";
        usObjectType = Siege::SHORT_RANGE_CASTLE_ARMS;
	}
	else if ( !strcmp( pstrType, "Dropship" ) )
	{
        strGCMDSName = "SIEGE_Dropship.GCMDS";
        usObjectType = Siege::AIRSHIP;
	}
	else if ( !strcmp( pstrType, "Character" ) )
	{
		return NULL;
	}

    // search usable creature
    map<unsigned long,RYLCreature*>::iterator iterCreature;// = m_mapCreature.end();
    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->GetMemoryReusable() )
            break;
    }

    // 사용 가능한 creature가 있다.
    if( iterCreature != m_mapCreature.end() )
    {
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( pSelfCreature->GetCharID() == iterCreature->second->GetCharID() )
        {
            int i = 0;
            i = 1;
        }

        pSiegeCreature = iterCreature->second;

        // creature 초기화
        pSiegeCreature->InitCreature();

        pSiegeCreature->m_ulCharID       = ulCharID;

        pSiegeCreature->m_strCharName    = pstrName;
        pSiegeCreature->m_strGCMDSName   = strGCMDSName;

        pSiegeCreature->m_vecPosition    = vecPosition;
        pSiegeCreature->m_fDirection     = fDirection;

        pSiegeCreature->m_strFaceType    = "FACE01";
        pSiegeCreature->m_strHairType    = "HAIR01";

        // 기존에 map 상에 있던 원소는 제거해준다.( 왜냐? 새로 집어 넣을꺼니까! )
        m_mapCreature.erase( iterCreature );
    }
    // 사용 가능한 creature가 없다.
    else 
    {
        // creature 생성
        pSiegeCreature = new RYLCreature( ulCharID, pstrName, strGCMDSName.c_str(), vecPosition, fDirection );
    }

    //
    pSiegeCreature->SetIsUsed( true );
    pSiegeCreature->SetMemoryReusable( false );

    // create character model
    CZ3DGeneralChrModel* pSiegeModel = AllocModel( pSiegeCreature );
	pSiegeModel->SetPosition( vecPosition );
	pSiegeModel->SetDirection( fDirection );
	pSiegeModel->SetLODLevel( 0 );
    
    pSiegeCreature->SetOBBEnable( pSiegeModel->HasBoundingVolumeInfo() );

    CSceneManager::CharacterCollisionDetectType( pSiegeModel, CDT_NONE );
	pSiegeCreature->SetCDTType( CDT_NONE );

    pSiegeCreature->SetObjectType( usObjectType );

    m_mapCreature.insert( make_pair( pSiegeCreature->m_ulCharID, pSiegeCreature ) );

    // 빈 데이터를 data manager에 삽입
    CharDataNode* pSiegeData = new CharDataNode;
    pSiegeData->m_strName    = pstrName;
    pSiegeData->m_ulCharID   = pSiegeCreature->m_ulCharID;
    
    pSiegeData = RYLCharacterDataManager::Instance()->AddCharData( pSiegeData );

    return pSiegeCreature;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::DeleteCreature()
void RYLCreatureManager::DeleteCreature( unsigned long ulCharID, BOOL bForceDelete )
{
    map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.find( ulCharID );

	RYLCreature* pTempChar;

    if( iterCreature != m_mapCreature.end() )
    {	
		pTempChar = iterCreature->second;
        RYLCharacterDataManager::Instance()->DeleteCharData( pTempChar->GetCharID() );

        CZ3DGeneralChrModel* pTempModel = pTempChar->GetCharModel();

		if( pTempModel )
        {
            pTempChar->Respawn();
            FreeModel( pTempModel, bForceDelete );
            pTempChar->m_pCharModel = NULL;
        }

        //if( m_mapCreature.size() > m_unMaxCash )
        {
            pTempChar->ClearCreature();

            delete pTempChar;
            iterCreature->second = NULL;

            m_mapCreature.erase( iterCreature );
        }
        /*else
        {
            iterCreature->second->InitCreature();
            iterCreature->second->SetMemoryReusable( true );
        }*/
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::AddCreatureList()
void RYLCreatureManager::AddCreatureList( RYLCreature* pCreature )
{
    if( !pCreature )
        return;

    map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.find( pCreature->GetCharID() );

    if( iterCreature != m_mapCreature.end() )
    {
        m_mapCreature.insert( make_pair( pCreature->GetCharID(), pCreature ) );
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::DeleteCreatureList()
RYLCreature* RYLCreatureManager::DeleteCreatureList( unsigned long ulCharID )
{
    map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.find( ulCharID );

    if( iterCreature != m_mapCreature.end() )
    {
        RYLCreature* pTempCreature = iterCreature->second;

        m_mapCreature.erase( iterCreature );

        return pTempCreature;
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetCreature()
RYLCreature* RYLCreatureManager::GetCreature( CZ3DGeneralChrModel* pModel )
{               
    map<unsigned long,RYLCreature*>::iterator iterCreature;

    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( !iterCreature->second )
        {
            m_mapCreature.erase( iterCreature );
            return NULL;
        }

        if( !iterCreature->second->GetMemoryReusable() )
        {
            if( iterCreature->second->m_pCharModel == pModel )
            {
                return iterCreature->second;
            }
        }
    }

    return NULL;
}

RYLCreature* RYLCreatureManager::GetCreature( unsigned long ulCharID )
{
    map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.find( ulCharID );

    if( iterCreature != m_mapCreature.end() )
    {
        if( !iterCreature->second )
        {
            m_mapCreature.erase( iterCreature );
            return NULL;
        }

        if( !iterCreature->second->GetMemoryReusable() )
        {
            return iterCreature->second;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetCreatureByAddedOrder()
RYLCreature* RYLCreatureManager::GetCreatureByAddedOrder( unsigned long ulAddedOrder )
{
    map<unsigned long,RYLCreature*>::iterator iterCreature;

    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->m_ulAddedID == ulAddedOrder )
            return iterCreature->second;
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetCharIDArray()
void RYLCreatureManager::GetCharIDArray( vector<unsigned long>& vtCharID )
{
    map<unsigned long,RYLCreature*>::iterator iterCreature;

    vtCharID.clear();
	vtCharID.reserve( m_mapCreature.size() );

    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        vtCharID.push_back( iterCreature->second->GetCharID() );
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetCreatureName()
const char* RYLCreatureManager::GetCreatureName( unsigned long ulCharID )
{
    RYLCreature* pTempCreature = GetCreature( ulCharID );

    if( pTempCreature )
    {
        return pTempCreature->m_strCharName.c_str();
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetCreatureListHead()
RYLCreature* RYLCreatureManager::GetCreatureListHead()
{
    if( !m_mapCreature.empty() )
    {
        m_iterBeforeCreature = m_mapCreature.begin();

        return m_iterBeforeCreature->second;
    }
    
    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetCreatureListNext()
RYLCreature* RYLCreatureManager::GetCreatureListNext()
{
    if( m_iterBeforeCreature != m_mapCreature.end() )
    {
        ++m_iterBeforeCreature;

        if( m_iterBeforeCreature != m_mapCreature.end() )
        {
            return m_iterBeforeCreature->second;
        }
        else
        {
            return NULL;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetModelListHead()
CZ3DGeneralChrModel* RYLCreatureManager::GetModelListHead()
{
    if( !m_lstCharModel.empty() )
    {
        m_iterBeforeModel = m_lstCharModel.begin();
        return (*m_iterBeforeModel)->m_pCharModel;
    }
    
    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetModelListNext()
CZ3DGeneralChrModel* RYLCreatureManager::GetModelListNext()
{
    if( m_iterBeforeModel != m_lstCharModel.end() )
    {
        ++m_iterBeforeModel;

        if( m_iterBeforeModel != m_lstCharModel.end() )
        {
            return (*m_iterBeforeModel)->m_pCharModel;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::AllocModel()
CZ3DGeneralChrModel* RYLCreatureManager::AllocModel( RYLCreature* pCreature, bool bHouse, int iHouseType, D3DXVECTOR3 vecPos )
{   
    // 모델 풀을 체크하여 사용가능한 모델이 있는지 체크
    CharModelNode* pTempModelNode = CheckUsableModelNode( pCreature->GetModelFileName()->c_str() );

    if( pTempModelNode )
    {
        pTempModelNode->m_ulUsedCharID = pCreature->GetCharID();

        CCharacterLightShadowManager::CCharacterDataNode CheckNode = CSceneManager::GetCharacterDataNode( pTempModelNode->m_pCharModel );
		CheckNode.m_bRender = true;
		CSceneManager::SetCharacterDataNode( CheckNode );
        CSceneManager::CharacterMovementVector( pTempModelNode->m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );

        pTempModelNode->m_pCharModel->SetPosition( *( pCreature->GetPosition() ) );

        pCreature->SetCharModel( pTempModelNode->m_pCharModel );
    }
    else
    {
        // create model data
        pTempModelNode   = new CharModelNode;
        pTempModelNode->m_strGCMDSName  = pCreature->GetModelFileName()->c_str();
        pTempModelNode->m_ulUsedCharID  = pCreature->GetCharID();
        pTempModelNode->m_pCharModel    = CSceneManager::AddCharacter( pCreature->GetModelFileName()->c_str(), 
                                                                       pCreature->GetFaceType()->c_str(), 
                                                                       pCreature->GetHairType()->c_str(), -1, 0, bHouse, iHouseType, vecPos );

        if( !pTempModelNode->m_pCharModel )
        {
            char    strError[ MAX_PATH ] = { 0, };
            sprintf( strError, "%s %s %s", pCreature->GetModelFileName()->c_str(), pCreature->GetFaceType()->c_str(), pCreature->GetHairType()->c_str() );
            MessageBox( NULL, strError, "Caution", MB_OK );

            return NULL;
        }

        pTempModelNode->m_pCharModel->SetAnimationSpeed( 1.0f );
        CSceneManager::CharacterMovementVector( pTempModelNode->m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );

        // save to model list                  
        m_lstCharModel.push_back( pTempModelNode );

        pCreature->SetCharModel( pTempModelNode->m_pCharModel );
    }

	const char **ppstrEffect = NULL;
	int nEffectNum = pTempModelNode->m_pCharModel->GetEffectInfoList( ppstrEffect );

	if( ( nEffectNum > 0 ) && ( ppstrEffect != NULL ) )
	{
		for( int cEff = 0; cEff < nEffectNum ; ++cEff )
		{
			pCreature->GetEffect()->MonsterEffectProcess( (char*)ppstrEffect[ cEff ], 1 );
		}
	}

	return pTempModelNode->m_pCharModel;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::AllocTransformModel()
CZ3DGeneralChrModel* RYLCreatureManager::AllocTransformModel( unsigned long ulCharID, const char* pstrGCMDSName, bool bHouse, int iHouseType, D3DXVECTOR3 vecPos )
{
    // 모델 풀을 체크하여 사용가능한 모델이 있는지 체크
    CharModelNode* pTempModelNode = CheckUsableModelNode( pstrGCMDSName );

    if( pTempModelNode )
    {
        pTempModelNode->m_ulUsedCharID = ulCharID;

        CCharacterLightShadowManager::CCharacterDataNode CheckNode = CSceneManager::GetCharacterDataNode( pTempModelNode->m_pCharModel );
		CheckNode.m_bRender = true;
		CSceneManager::SetCharacterDataNode( CheckNode );
        CSceneManager::CharacterMovementVector( pTempModelNode->m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );

        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );
        if( !pTempCreature )
            return NULL;

        pTempModelNode->m_pCharModel->SetPosition( *( pTempCreature->GetPosition() ) );

        return pTempModelNode->m_pCharModel;
    }
    else
    {
        // create model data
        CharModelNode* pTempModelNode   = new CharModelNode;
        pTempModelNode->m_strGCMDSName  = pstrGCMDSName;
        pTempModelNode->m_ulUsedCharID  = ulCharID;
        pTempModelNode->m_pCharModel    = CSceneManager::AddCharacter( pstrGCMDSName, "FACE01", "HAIR01", -1, 0, bHouse, iHouseType, vecPos );

        if( !pTempModelNode->m_pCharModel )
        {
            char    strError[ MAX_PATH ] = { 0, };
            sprintf( strError, "%s %s %s", pstrGCMDSName, "FACE01", "HAIR01" );
            MessageBox( NULL, strError, "Caution", MB_OK );

            delete pTempModelNode;

            return NULL;
        }

        pTempModelNode->m_pCharModel->SetAnimationSpeed( 1.0f );
        CSceneManager::CharacterMovementVector( pTempModelNode->m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );

        // save to model list                  
        m_lstCharModel.push_back( pTempModelNode );

        return pTempModelNode->m_pCharModel;
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::FreeModel()
void RYLCreatureManager::FreeModel( CZ3DGeneralChrModel* pModel, BOOL bForceDelete )
{
    if( !pModel )
        return;

    for( list<CharModelNode*>::iterator iterModel = m_lstCharModel.begin() ; 
         iterModel != m_lstCharModel.end() ;
         ++iterModel )
    {
        if( (*iterModel)->m_pCharModel == pModel )
        {
            (*iterModel)->m_ulUsedCharID = 0xFFFFFFFF;

            if( m_lstCharModel.size() < m_unMaxCash && !bForceDelete )
            {
                CCharacterLightShadowManager::CCharacterDataNode CheckNode = CSceneManager::GetCharacterDataNode( (*iterModel)->m_pCharModel );
			    CheckNode.m_bRender = false;
			    CSceneManager::SetCharacterDataNode( CheckNode );
            }
            else
            {
                CSceneManager::DeleteCharacter( (*iterModel)->m_pCharModel );
                (*iterModel)->m_pCharModel = NULL;

				delete (*iterModel) ;
				(*iterModel) = NULL ;

                m_lstCharModel.erase( iterModel );
            }

            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::CheckUsableModelNode()
CharModelNode* RYLCreatureManager::CheckUsableModelNode( const char* pstrGCMDSName )
{
    // 사용가능한 모델 데이터가 있으면 그 주소를 리턴.
    int nLoopCount = 0;
    list<CharModelNode*>::iterator iterModel;

    for( iterModel = m_lstCharModel.begin() ; iterModel != m_lstCharModel.end() ; ++iterModel )
    {
        // 현재 순회중인 모델의 순서
        ++nLoopCount;

        if( !(*iterModel) )
            continue;

        // 이 모델을 사용하는 creature가 없으면
        if( (*iterModel)->m_ulUsedCharID == 0xFFFFFFFF )
        {
            // 이름을 비교해서 같은 모델이면 pCreature에 해당하는 모델로 데이터를 변경한 후 리턴.
            if( !( (*iterModel)->m_strGCMDSName.compare( pstrGCMDSName ) ) )
            {
                return (*iterModel);
            }

            // 사용하고 있는 creature가 없고 최대 갯수를 초과한 모델은 지워준다.
            if( nLoopCount > static_cast<int>( m_unMaxCash ) )
            {
                CSceneManager::DeleteCharacter( (*iterModel)->m_pCharModel );
                (*iterModel)->m_pCharModel = NULL;

				delete (*iterModel) ;
				(*iterModel) = NULL ;

                m_lstCharModel.erase( iterModel );
            }
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::CheckExistModel()
bool RYLCreatureManager::CheckExistModel( CZ3DGeneralChrModel* pModel )
{
    if( !pModel )
        return false;

    for( list<CharModelNode*>::iterator iterModel = m_lstCharModel.begin() ;
         iterModel != m_lstCharModel.end() ;
         ++iterModel )
    {
        if( (*iterModel)->m_pCharModel == pModel )
            return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::ReplaceModel()
CZ3DGeneralChrModel* RYLCreatureManager::ReplaceModel( RYLCreature* pCreature, const char* pstrGCMDSName )
{
	// pCreature 의 CharacterModel에 해당하는 m_lstCharModel 내의 노드를 찾아둠
	CharModelNode* pModelNode = NULL;
	std::list<CharModelNode*>::iterator it = m_lstCharModel.begin();
	for(;it != m_lstCharModel.end(); ++it )
	{
		if( pCreature->GetCharModel() == (*it)->m_pCharModel )
		{
			pModelNode = *it;
			break;
		}
	}
	// m_lstCharModel 에서 찾기 실패
	if( it == m_lstCharModel.end() )
	{
		MessageBox( NULL, "CharacterModel of RYLCreature is not registered in CharModel list", "Caution", MB_OK );
		return NULL;
	}

	// scene manager 에 Character Model 대체 요청
	CZ3DGeneralChrModel* pTempChrModel = CSceneManager::ReplaceCharacter( pCreature->GetCharModel(), pstrGCMDSName, *pCreature->GetPosition() );

	if( NULL == pTempChrModel )
	{
		char    strError[ MAX_PATH ] = { 0, };
		sprintf( strError, "Fail to replace model to %s", pstrGCMDSName );
		MessageBox( NULL, strError, "Caution", MB_OK );

		return NULL;
	}

	pTempChrModel->SetAnimationSpeed( 1.0f );
	CSceneManager::CharacterMovementVector( pTempChrModel, vector3( 0.0f, 0.0f, 0.0f ) );
	pTempChrModel->SetStaticSlot( "FACE", pCreature->GetFaceType()->c_str() );
	pTempChrModel->SetStaticSlot( "HAIR", pCreature->GetHairType()->c_str() );

	// modify parameters of node in model list
	pModelNode->m_pCharModel = pTempChrModel;
	pModelNode->m_strGCMDSName = pstrGCMDSName;


	pCreature->SetCharModel( pTempChrModel );

	return pTempChrModel;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetSendCharacterChatList()
void RYLCreatureManager::GetSendCharacterChatList( list<unsigned long>& lstChar )
{
    vector3 vecSelfCharPos, vecCharPos, vecLens;

    CZ3DGeneralChrModel* pSelfModel = m_pSelfCreature->GetCharModel();
    pSelfModel->GetPosition( vecSelfCharPos );

    m_pSelfCreature->m_fSelfCharLens = 0.0f;

    map<unsigned long,RYLCreature*>::iterator iterCreature;
    for( iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->m_ulCharID == m_pSelfCreature->m_ulCharID )
            continue;

        CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
        if( !pSelfData )
            return;

        if( pSelfData->m_cNation != pSelfData->m_cNation )
        {
            CharDataNode* pCharData = RYLCharacterDataManager::Instance()->GetCharData( iterCreature->second->m_ulCharID );

            if( !pCharData->m_bPeaceMode )
                continue;
        }

        CZ3DGeneralChrModel* pModel = iterCreature->second->GetCharModel();
        if( pModel )
        {
            pModel->GetPosition( vecCharPos );
        }
        else
        {
            vecCharPos = iterCreature->second->m_vecPosition;
        }

        vecLens     = vecSelfCharPos - vecCharPos;
        vecLens.y   = 0.0f;

        iterCreature->second->m_fSelfCharLens = vecLens.GetLens();

        if( GetClientIdentity( iterCreature->second->m_ulCharID ) == Creature::CT_PC )
        {
            if( iterCreature->second->m_fSelfCharLens <= 6400.0f )
            {
                lstChar.push_back( iterCreature->second->GetCharID() );
            }
            else if( !iterCreature->second->m_bFirstPositionSend )
            {
                lstChar.push_back( iterCreature->second->GetCharID() );

                iterCreature->second->m_bFirstPositionSend = true;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetSendCharacterAroundList()
void RYLCreatureManager::GetSendCharacterAroundList( list<unsigned long>& lstChar, unsigned long ulSummonID, bool bSummonAttacked )
{
    vector3 vecSelfCharPos, vecCharPos, vecLens;
    
    CZ3DGeneralChrModel* pSelfModel = m_pSelfCreature->GetCharModel();

    pSelfModel->GetPosition( vecSelfCharPos );
    m_pSelfCreature->m_fSelfCharLens = 0.0f;

    RYLCreature* pSummonCreature = NULL;

    if( bSummonAttacked )
        pSummonCreature = GetCreature( ulSummonID );

    for( map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( ( iterCreature->second->GetCharID() == m_pSelfCreature->GetCharID() )
         || ( bSummonAttacked && pSummonCreature && iterCreature->second->GetCharID() == pSummonCreature->GetCharID() ) )
            continue;

        CZ3DGeneralChrModel* pModel = iterCreature->second->GetCharModel();

        if( pModel )
        {
            pModel->GetPosition( vecCharPos );
        }
        else
        {
            vecCharPos = iterCreature->second->m_vecPosition;
        }

        vecLens     = vecSelfCharPos - vecCharPos;
        vecLens.y   = 0.0f;

        iterCreature->second->m_fSelfCharLens = vecLens.GetLens();

        if( GetClientIdentity( iterCreature->second->GetCharID() ) == Creature::CT_PC )
        {
            if( iterCreature->second->m_fSelfCharLens <= 6400.0f )
            {
                lstChar.push_back( iterCreature->second->GetCharID() );
            }
            else if( !iterCreature->second->m_bFirstPositionSend )
            {
                lstChar.push_back( iterCreature->second->GetCharID() );

                iterCreature->second->m_bFirstPositionSend = true;
            }
        }
    }
}

void RYLCreatureManager::GetSendCharacterAroundList( list<unsigned long>& lstChar, unsigned long ulSummonID[ 10 ] )
{
    vector3 vecSelfCharPos, vecCharPos, vecLens;

    CZ3DGeneralChrModel* pModel = m_pSelfCreature->GetCharModel();

    pModel->GetPosition( vecSelfCharPos );
    m_pSelfCreature->m_fSelfCharLens = 0.0f;

    RYLCreature*    pSummonCreature = NULL;
    unsigned long   ulMasterID[ 10 ], ulMasterCount = 0;

    for( unsigned int i = 0 ; i < 10 ; ++i )
    {
		if( GetClientIdentity( ulSummonID[ i ] ) == Creature::CT_SUMMON )
        {
            pSummonCreature = GetCreature( ulSummonID[ i ] );

            if( pSummonCreature )
            {
                CharDataNode* pCharData = RYLCharacterDataManager::Instance()->GetCharData( pSummonCreature->GetCharID() );

                if( !pCharData )
                    continue;

                ulMasterID[ ulMasterCount ] = pCharData->m_ulSummonMasterCharID;
                ++ulMasterCount;
            }
        }
    }

    bool bMasterPass;

    for( map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second->GetCharID() == m_pSelfCreature->GetCharID() )
            continue;

        bMasterPass = false;
        for(unsigned long i = 0 ; i < ulMasterCount ; ++i )
        {
            if( iterCreature->second->m_ulCharID == ulMasterID[ i ] )
            {
                bMasterPass = true;
                break;
            }
        }

        if( bMasterPass )
            continue;

        CZ3DGeneralChrModel* pModel = iterCreature->second->GetCharModel();

        if( pModel )
        {
            pModel->GetPosition( vecCharPos );
        }
        else
        {
            vecCharPos = iterCreature->second->m_vecPosition;
        }

        vecLens     = vecSelfCharPos - vecCharPos;
        vecLens.y   = 0.0f;

        iterCreature->second->m_fSelfCharLens = vecLens.GetLens();

        if( GetClientIdentity( iterCreature->second->GetCharID() ) == Creature::CT_PC )
        {
            if( iterCreature->second->m_fSelfCharLens <= 6400.0f )
            {
                lstChar.push_back( iterCreature->second->GetCharID() );
            }
            else if( !iterCreature->second->m_bFirstPositionSend )
            {
                lstChar.push_back( iterCreature->second->GetCharID() );

                iterCreature->second->m_bFirstPositionSend = true;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::GetSendCharacterList()
void RYLCreatureManager::GetSendCharacterList( list<unsigned long>& lstChar )
{
    static unsigned int unTick = 0 ;
	unTick ++ ;

	if ( unTick >= 4 ) 
		unTick = 0 ;
	
    if( unTick && ( m_pSelfCreature->m_ulLowerCharActionID == CA_WAIT || m_pSelfCreature->m_ulLowerCharActionID == CA_REST ) )
        return;

    int nSendCharacter = 0;

    list<unsigned long>     lstPlayer;
    list<unsigned long>     lstParty;
    bool                    bInsert;

    for( map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.begin() ; iterCreature != m_mapCreature.end() ; ++iterCreature )
    {
        if( iterCreature->second == m_pSelfCreature )
            continue;

        CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( m_pSelfCreature->m_ulCharID );
        CharDataNode* pCharData = RYLCharacterDataManager::Instance()->GetCharData( iterCreature->second->m_ulCharID );

        if( GetClientIdentity( iterCreature->second->GetCharID() ) == Creature::CT_PC )
        {
            bInsert = false;

            if( !iterCreature->second->m_bFirstPositionSend )
            {
                bInsert = true;
                lstChar.push_back( iterCreature->second->m_ulCharID );
                iterCreature->second->m_bFirstPositionSend = true;
            }
            else
            {
                lstPlayer.push_back( iterCreature->second->m_ulCharID );
            }

            if( !unTick && pSelfData->m_ulPartyID == pCharData->m_ulPartyID && !bInsert )
                lstParty.push_back( iterCreature->second->m_ulCharID );
        }           
    }

    unsigned int X, Y, y;

	if( m_unMaxCash < static_cast<int>( lstPlayer.size() ) )
	{
		if ( m_unMaxCash < static_cast<int>( lstPlayer.size() ) && lstPlayer.size() <= 100 )
		{
			X = ( ( lstPlayer.size() - 21 ) / 10 ) * 3 ;
			Y = m_unMaxCash - X ;
			y = ( lstPlayer.size() - Y ) / X ;
		}
		else
		{
			X = 21 ;
			Y = 9 ;
			y = 4 ;
		}

        list<unsigned long>::iterator iterPlayer = lstPlayer.begin();
        unsigned int unCount = 0;

        while( iterPlayer != lstPlayer.end() )
        {
            if( unCount < Y )
            {
                lstChar.push_back( (*iterPlayer) );

                list<unsigned long>::iterator iterDelete = find( lstParty.begin(), lstParty.end(), (*iterPlayer) );

                if( iterDelete != lstParty.end() )
                    lstParty.erase( iterDelete );
            }
            else
            {
                break;
            }

            ++iterPlayer;
            ++unCount;
        }

		unsigned int A = Y + ( unTick % y ) ;

        iterPlayer = lstPlayer.begin();

        while( iterPlayer != lstPlayer.end() )
        {
            if( unCount < X && lstPlayer.size() < A )
            {
                lstChar.push_back( (*iterPlayer) );

                list<unsigned long>::iterator iterDelete = find( lstParty.begin(), lstParty.end(), (*iterPlayer) );

                if( iterDelete != lstParty.end() )
                    lstParty.erase( iterDelete );
            }
            else
            {
                break;
            }

            ++iterPlayer;
            ++unCount;
            A += y;
        }
	} 
	else
	{
		list<unsigned long>::iterator iterPlayer = lstPlayer.begin();

        while( iterPlayer != lstPlayer.end() )
        {
            lstChar.push_back( (*iterPlayer) );

            list<unsigned long>::iterator iterDelete = find( lstParty.begin(), lstParty.end(), (*iterPlayer) );

            if( iterDelete != lstParty.end() )
                lstParty.erase( iterDelete );

            ++iterPlayer;
        }
	}

    for( list<unsigned long>::iterator iterParty = lstParty.begin() ; iterParty != lstParty.end() ; ++iterParty )
    {
        lstChar.push_back( (*iterParty) );
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::ClearCreatureManager()
void RYLCreatureManager::ClearCreatureManager()
{
    for( list<CharModelNode*>::iterator iterModel = m_lstCharModel.begin() ;
         iterModel != m_lstCharModel.end() ;
         ++iterModel )
    {
        CSceneManager::DeleteCharacter( (*iterModel)->m_pCharModel );
		(*iterModel)->m_pCharModel = NULL ;

        delete (*iterModel);
        (*iterModel) = NULL;
    }
    m_lstCharModel.clear();

    for( map<unsigned long,RYLCreature*>::iterator iterCreature = m_mapCreature.begin() ; 
         iterCreature != m_mapCreature.end() ; 
         ++iterCreature ) 
    {
        delete iterCreature->second;
        iterCreature->second = NULL;
    }
    m_mapCreature.clear();                                                                                                                              

    RYLCharacterDataManager::Instance()->DeleteAllCharData();

    // clear add order
    RYLCreature::m_ulAddedOrder = 0;
    // clear self creature
    m_pSelfCreature = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::AddAction()
void RYLCreatureManager::AddAction( unsigned long ulCharID, unsigned long ulUpperAni, unsigned long ulLowerAni,
                                    float fDirection, vector3& vecNextPosition,
                                    float fVec, unsigned short usAniNum, unsigned long ulFrame )
{   
    if( !m_pSelfCreature )
        return;
    
    if( m_pSelfCreature->GetCharID() == ulCharID && ulLowerAni != CA_RESPAWN )  // 테스트할때 막는다.
        return;             

    RYLCreature* pTempCreature = GetCreature( ulCharID );

    if( pTempCreature )
    {
        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

        if( pTempModel )
        {
            switch( GetClientIdentity( ulCharID ) )
            {
            case Creature::CT_PC:
                {
                    if ( m_mapCreature.size() == 0 )
                    {
						CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_FULL );
                    }
					else
                    {
						CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
                    }
                }
                break;

            case Creature::CT_NPC:
                {
                    CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_FIRSTBOTTOM ) ;
                }
                break;

            case Creature::CT_MONSTER:
                {
                    CSceneManager::CharacterCollisionDetectType( pTempModel,  pTempCreature->GetCDTType() ) ;
					
                    if( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
					{
						ulUpperAni = CA_STUN ;
						ulLowerAni = CA_STUN ;
					}
                }
                break;
            }
        }

        CharActionNode* pTempAction     = new CharActionNode;
        pTempAction->m_ulLowerChrAction = ulLowerAni;
        pTempAction->m_ulUpperChrAction = ulUpperAni;
        pTempAction->m_fDirection       = fDirection;
        pTempAction->m_vecNextPosition  = vecNextPosition;
        pTempAction->m_fVec             = fVec;
        pTempAction->m_usAniNum         = usAniNum;
        pTempAction->m_usAniNumCount    = usAniNum;
        pTempAction->m_ulFrame          = ulFrame;

        if( GetClientIdentity( ulCharID ) == Creature::CT_MONSTER )			
        {
            if( pTempModel )
            {
                vector3 vecChrPos;
				pTempModel->GetPosition( vecChrPos );
				pTempAction->m_vecNextPosition.y = vecChrPos.y;
				pTempModel->SetPosition( pTempAction->m_vecNextPosition );

                CSceneManager::CharacterCollisionDetectType( pTempModel, pTempCreature->GetCDTType() );
			}
			else
			{
				vector3 vecMove( cosf( ( -pTempAction->m_fDirection ) - FLOAT_PHI / 2.0f ),
                                 0.0f,
                                 sinf( ( -pTempAction->m_fDirection ) - FLOAT_PHI / 2.0f ) );
				vecMove *= pTempAction->m_fVec;
				vecMove *= pTempAction->m_ulFrame;
				vecMove *= 100.0f;
				vecMove *= pTempAction->m_usAniNum;

                vector3 TempPos = pTempAction->m_vecNextPosition + vecMove;
                pTempCreature->SetPosition( TempPos );
                pTempCreature->SetDirection( pTempAction->m_fDirection );
                pTempCreature->SetAction( pTempAction->m_ulUpperChrAction, pTempAction->m_ulLowerChrAction );

				return;
			}
		}
		else if( GetClientIdentity( ulCharID ) == Creature::CT_SIEGE_OBJECT )			
		{			
			if( pTempModel )
			{
				pTempCreature->SetPosition( pTempAction->m_vecNextPosition );
				pTempCreature->SetDirection( pTempAction->m_fDirection );
				pTempCreature->SetAction( pTempAction->m_ulUpperChrAction, pTempAction->m_ulLowerChrAction );
			}
		}
		else
		{
			if ( !pTempModel )
			{
				pTempCreature->SetPosition( pTempAction->m_vecNextPosition );
                pTempCreature->SetDirection( pTempAction->m_fDirection );
                pTempCreature->SetAction( pTempAction->m_ulUpperChrAction, pTempAction->m_ulLowerChrAction );

				return;
			}
		}

        pTempCreature->AddActionData( pTempAction );
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureManager::Transformation()
void RYLCreatureManager::Transformation( unsigned long ulCharID, const char* pstrGCMDSName )
{
    /*CZ3DGeneralChrModel* pstrTransformModel = NULL;
    CCharacterLightShadowManager::CCharacterDataNode TransformData;

    if( pstrGCMDSName )
    {
        pstrTransformModel  = CSceneManager::AddCharacter( pstrGCMDSName, "FACE01", "HAIR01" );
        TransformData       = m_TransformModelData  = CSceneManager::GetCharacterDataNode( pstrTransformModel );
    }
    else
    {
        pstrTransformModel  = CSceneManager::AddCharacter( m_strGCMDSName.c_str(), m_strFaceType.c_str(), m_strHairType.c_str() );
        TransformData       = m_OriginalModelData   = CSceneManager::GetCharacterDataNode( pstrTransformModel );
    }

    pstrTransformModel->SetDirection( m_pCharModel->GetDirection() );
    pstrTransformModel->SetPosition( m_pCharModel->GetPosition() );

    CSceneManager::m_CharacterManager.UnallocListChr( TransformData, TRUE );
    CSceneManager::m_CharacterManager.DeleteElement( m_pCharModel ) ;

    m_pCharModel = pstrTransformModel;

    return S_FALSE;*/
}

void RYLCreatureManager::AddArmsCreature(unsigned long dwCID, unsigned long dwArmsCID)
{
	m_ArmsMap.insert(make_pair(dwCID, dwArmsCID));
}

void RYLCreatureManager::DeleteAllArmsCreature()
{
	m_ArmsMap.clear();
}

void RYLCreatureManager::DeleteArmsCreature(unsigned long dwCID)
{
	ArmsMap::iterator pos = m_ArmsMap.find(dwCID);
	
	if(pos!=m_ArmsMap.end())
	{
		m_ArmsMap.erase(pos);
	}
}

unsigned long RYLCreatureManager::IsArmsCreature(unsigned long dwCID)
{
	ArmsMap::iterator pos = m_ArmsMap.find(dwCID);

	if(pos!=m_ArmsMap.end())
	{
		return pos->second;
	}

	return 0;
}