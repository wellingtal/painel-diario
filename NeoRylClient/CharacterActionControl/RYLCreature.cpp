//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreature.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: base class for Creature
//
// Date: 2004. 3. 25. ( Tue )
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include <Winsock2.h>

#include <Creature/CreatureStructure.h>
#include <Creature/Monster/MonsterMgr.h>
#include <Creature/Character/CharacterClass.h>
#include <Network/ClientSocket/Parse/ParseCastle.h>

#include "FrameTimer.h"
#include "SoundMgr.h"
#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "CameraControl.h"
#include "FullSceneEffect.h"
#include "BgmManager.h"

#include "GMFontTexture.h"
#include "GMImageDib.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include "RYLObjectControl.h"
#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "RYLLabel.h"
#include "RYLPicture.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"

#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"

#include "RYLClientMain.h"

//--------------------------------------------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------------------------------------------
#define KeyPressed( key )    HIBYTE( GetAsyncKeyState( key ) )	

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreature
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long RYLCreature::m_ulAddedOrder = 0;
unsigned long RYLCreature::m_ulRunFactorTimer = CFrameTimer::Regist( 200.0f );

//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
RYLCreature::RYLCreature()
{                  
    m_pCharModel = NULL;
	m_pZ3DOBB = NULL ;

	m_pCreatureEffect   = NULL;
	m_pDamageEffect     = NULL;

	m_pCharStreetStallNameLabel	= NULL ;
	m_pCharNameLabel			= NULL ;
	m_pCharTempGuildNameLabel	= NULL ;
	m_pCharJobNameLabel			= NULL ;
	m_pCharChatMsgLabel			= NULL ; 
	m_pSignpostImg				= NULL ;

    InitCreature();

	m_cGuildWarFlag		= 0 ;
	m_cRealmWarFlag		= 0 ;
	m_cRealmPoint		= 0;
	m_cGMModelFlag		= 0;
	m_wObjectType		= 0;
}

RYLCreature::RYLCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName, 
                          vector3 vecCharPos, float fDirection )
{
    m_pCharModel = NULL;
	m_pZ3DOBB = NULL ;

    m_pCreatureEffect   = NULL;
    m_pDamageEffect     = NULL;

	m_pCharStreetStallNameLabel	= NULL ;
	m_pCharNameLabel			= NULL ;
	m_pCharTempGuildNameLabel	= NULL ;
	m_pCharJobNameLabel			= NULL ;
	m_pCharChatMsgLabel			= NULL ; 
	m_pSignpostImg				= NULL ;
    
    InitCreature();

    m_ulCharID          = ulCharID;

    m_strCharName       = pstrName;
    m_strGCMDSName      = pstrGCMDSName;

    m_vecPosition       = vecCharPos;
	m_vecNamePosition	= vector3( 0.0f, 0.0f, 0.0f );       
    m_fDirection        = fDirection;

    m_strFaceType       = "FACE01";
    m_strHairType       = "HAIR01";

	std::fill_n(m_strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

	m_bRideEnable		= false;
	m_ucEnableRide		= 0;
	m_bOBBEnable		= false;

	m_cGuildWarFlag		= 0 ;
	m_cRealmWarFlag		= 0 ;
	m_cGMModelFlag		= 0;
	m_cRealmPoint		= 0;
	m_wObjectType		= 0;
}

void	RYLCreature::MakeSignpostImg( char* pszText, bool bNPC ) 
{
	char szName[MAX_PATH] = { 0, };
	char szJob[MAX_PATH] = { 0, };
	char szTemp[MAX_PATH] = { 0, };

	unsigned short wWidth = 0, wHeight = 0, wTmpWidth = 0 ;
	strcpy( szTemp, pszText );

	// 이름과 직업을 토큰으로 나눈다.
//	if( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
	if( g_ClientMain.m_iLanguage == 2)
	{
		WCHAR	strWideString[MAX_PATH];
		char	strBreak[MAX_PATH];

		MultiByteToWideChar(CP_ACP, 0, szTemp, strlen(szTemp) + 1, strWideString, MAX_PATH);
		WCHAR*   wtoken = wcstok( strWideString, L"\\" ) ;

		if ( NULL != wtoken )
		{
			int nLen = wcslen( wtoken ) ;
			WideCharToMultiByte(CP_ACP, 0, wtoken, nLen + 1, strBreak, MAX_PATH, NULL, NULL);
			memcpy( szName, strBreak, nLen * 2 + 1 ) ;
			wtoken = wcstok( NULL, L"\\" ) ;

			if ( NULL != wtoken )
			{
				nLen = wcslen( wtoken ) ;
				WideCharToMultiByte(CP_ACP, 0, wtoken, nLen + 1, strBreak, MAX_PATH, NULL, NULL);
				memcpy( szJob, strBreak, nLen * 2 + 1 ) ;
			}
		}
	}
	else
	{
		char* lpToken = strtok( szTemp, "\\" ) ;
		if ( NULL != lpToken )
		{
			strcpy( szName, lpToken ) ;
			lpToken = strtok( NULL, "\\" ) ;

			if ( NULL != lpToken )
			{
				strcpy( szJob, lpToken ) ;
			}
		}
	}

	// 각 토큰의 길이를 이용해서 이름 박스의 최종 크기를 결정
	if (0 != strcmp(szJob, ""))
	{
		unsigned short wNameLen = CRYLStringTable::GetStringWidth( szName ) ;
		unsigned short wJobNameLen = CRYLStringTable::GetStringWidth( szJob ) ;
		wTmpWidth  = ( wNameLen > wJobNameLen ) ? wNameLen : wJobNameLen + 10 ;
		wWidth = ( wTmpWidth / 10 ) * 10 + 42 ;
		wHeight = 47 ;
	}
	else
	{
		wTmpWidth = CRYLStringTable::GetStringWidth( pszText ) + 10 ;
		wWidth = ( wTmpWidth / 10 ) * 10 + 42 ;
		wHeight = 32 ;
	}
	

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;
	
	if ( NULL == m_pSignpostImg )
	{
		m_pSignpostImg = new CRYLPicture( NULL ) ;
	}

	m_pSignpostImg->SetLeft( 179 ) ;
	m_pSignpostImg->SetTop( 36 ) ;
	// Bounus Str Image
	CRYLSpriteList::Instance()->CreateGMImage( wWidth, wHeight + 7, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	pImageDIB->DrawFrameRect( 0, 0, wWidth, wHeight, 0x80000000 ) ;

	// left, top
	if ( bNPC )		SetRect( &rt, 108, 138, 115, 151 ) ;
	else			SetRect( &rt, 130, 138, 137, 151 ) ;

	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;

	// top, bottom horizon
	int iHorzNum = ( wWidth / 10 ) - 1 ;
	int i = 0;
	for (; i < iHorzNum ; i ++ )
	{
		if ( bNPC )		SetRect( &rt, 115, 138, 125, 151 ) ;
		else			SetRect( &rt, 137, 138, 147, 151 ) ;
		pImageDIB->DrawRect( &rt, 7 + ( i * 10 ), 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;

		if ( bNPC )		SetRect( &rt, 115, 156, 125, 160 ) ;
		else			SetRect( &rt, 137, 156, 147, 160 ) ;
		pImageDIB->DrawRect( &rt, 7 + ( i * 10 ), wHeight - 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	}

	// right, top
	if ( bNPC )		SetRect( &rt, 125, 138, 130, 151 ) ;
	else			SetRect( &rt, 147, 138, 152, 151 ) ;
	pImageDIB->DrawRect( &rt,  7 + ( i * 10 ), 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	
	// top, bottom vertical	
	int iVertNum = ( ( wHeight - 17 ) / 5 ) ;
	for ( i = 0 ; i < iVertNum ; i ++ )
	{
		if ( bNPC )		SetRect( &rt, 108, 151, 115, 156 ) ;
		else			SetRect( &rt, 130, 151, 137, 156 ) ;
		pImageDIB->DrawRect( &rt, 0, 13 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;

		if ( bNPC )		SetRect( &rt, 123, 151, 130, 156 ) ;
		else			SetRect( &rt, 145, 151, 152, 156 ) ;
		pImageDIB->DrawRect( &rt, wWidth - 7, 13 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	}
	
	// left, bottom
	if ( bNPC )		SetRect( &rt, 108, 156, 115, 160 ) ;
	else			SetRect( &rt, 130, 156, 137, 160 ) ;
	pImageDIB->DrawRect( &rt, 0, 13 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;	
	
	// left, bottom
	if ( bNPC )		SetRect( &rt, 125, 156, 130, 160 ) ;
	else			SetRect( &rt, 147, 156, 152, 160 ) ;
	pImageDIB->DrawRect( &rt, wWidth - 5, 13 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;	

	if ( bNPC )		SetRect( &rt, 108, 160, 122, 169 ) ;
	else			SetRect( &rt, 130, 160, 144, 169 ) ;
	pImageDIB->DrawRect( &rt, ( wWidth / 2 ) - 7, wHeight - 2, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;	
	
    pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pSignpostImg->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
}

void	RYLCreature::DeleteSignpostImg()	
{
	if ( m_pSignpostImg )
	{
		delete m_pSignpostImg ;
		m_pSignpostImg = NULL ;
	}
}

RYLCreature::RYLCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName, 
                          const char* pstrFaceType, const char* pstrHairType, const char** pstrShape, 
                          unsigned char ucRace, vector3 vecCharPos, float fDirection )
{   
    m_pCharModel = NULL;
	m_pZ3DOBB = NULL ;

    m_pCreatureEffect   = NULL;
    m_pDamageEffect     = NULL;

	m_pCharStreetStallNameLabel	= NULL ;
	m_pCharNameLabel			= NULL ;
	m_pCharTempGuildNameLabel	= NULL ;
	m_pCharJobNameLabel			= NULL ;
	m_pCharChatMsgLabel			= NULL ; 
	m_pSignpostImg				= NULL ;

    InitCreature();

    m_ulCharID          = ulCharID;

    m_strCharName       = pstrName;
    m_strGCMDSName      = pstrGCMDSName;
    m_strFaceType       = pstrFaceType;
    m_strHairType       = pstrHairType;

//	std::copy(pstrShape, pstrShape + Item::EquipmentPos::MAX_EQUPMENT_POS, m_strShape);
	for(int i = 0 ; i < Item::EquipmentPos::MAX_EQUPMENT_POS ; ++i)
		m_strShape[i] = NULL;

    m_ucRace            = ucRace;
    m_vecPosition       = vecCharPos;
	m_vecNamePosition	= vector3( 0.0f, 0.0f, 0.0f );       
    m_fDirection        = fDirection;

    m_strFaceType       = pstrFaceType;
    m_strHairType       = pstrHairType;

	m_bRideEnable		= false;

	m_cGuildWarFlag		= 0 ;
	m_cRealmWarFlag		= 0 ;

	m_ucEnableRide		= 0;
}

RYLCreature::RYLCreature( unsigned long ulCharID, vector3 vecPosition )
{   
    m_pCharModel = NULL;
	m_pZ3DOBB = NULL ;

    m_pCreatureEffect   = NULL;
    m_pDamageEffect     = NULL;

	m_pCharStreetStallNameLabel	= NULL ;
	m_pCharNameLabel			= NULL ;
	m_pCharTempGuildNameLabel	= NULL ;
	m_pCharJobNameLabel			= NULL ;
	m_pCharChatMsgLabel			= NULL ; 
	m_pSignpostImg				= NULL ;
    
    InitCreature();
    
    m_ulCharID          = ulCharID;

    m_strCharName.clear();
    m_strGCMDSName.clear();

	std::fill_n(m_strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

    m_bFirstPositionSend= false;

    m_vecPosition       = vecPosition;
	m_vecNamePosition	= vector3( 0.0f, 0.0f, 0.0f );       
    m_fDirection        = 0.0f;
    
    m_strFaceType       = "FACE01";
    m_strHairType       = "HAIR01";

	m_bRideEnable		= false;

	m_cGuildWarFlag		= 0 ;
	m_cRealmWarFlag		= 0 ;

	m_ucEnableRide		= 0;
}

RYLCreature::~RYLCreature()
{
    ClearCreature();
}

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::InitCreature()
void RYLCreature::InitCreature( bool bRespawn )
{
    //m_ulCharID              = 0xFFFFFFFF;

    m_ulAddedID             = m_ulAddedOrder;
    ++m_ulAddedOrder;

	m_ulCreatureType        = Creature::CT_NONE_TYPE;

    //m_bUsed                 = false;
    m_bFirstPositionSend    = false;
    m_bMemoryReusable       = false;

	memset(m_dwRiderID, 0, sizeof(DWORD) * Siege::AIRSHIP_RIDER_NUM);

    m_strCharName.c_str();
    if ( m_pCharModel )
	{
		InitWeapon( 0 );
		m_pCharModel->SetAnimationSpeed( 1.0f );
	}

    if ( !bRespawn )
    {
		for ( int i = 0; i < Item::EquipmentPos::MAX_EQUPMENT_POS; ++i )
        {
            m_usShape[ i ] = 0; 
            m_strShape[ i ]	= NULL;
        }
    }

    m_vecPosition           = vector3( 0.0f, 0.0f, 0.0f );       
	m_vecNamePosition		= vector3( 0.0f, 0.0f, 0.0f );       
    m_fDirection            = 0.0f;   
    
	if ( !bRespawn )
	{
		m_ucRideFlag			= 0;
		m_ucEnableRide			= 0;
		m_ulWeaponPos           = 0;
		m_ucRace                = 0;
		m_usClass               = 0;
	}

    m_fScale                = 1.0f;
    m_fFixAngle             = 0.0f;
    
    m_fRunFactor            = 0.0f;
    m_vecJumpAttackVector   = vector3( 0.0f, 0.0f, 0.0f );

    m_fSelfCharLens         = 0.0f;    
	m_bViewName	            = false;
    
    m_ulUpperCharActionID   = CA_WAIT;
    m_ulLowerCharActionID   = CA_WAIT;
    m_bUpperActing          = false;
    m_bLowerActing          = false;
    m_bUpperAble            = true;        
    m_bLowerAble            = true;

    m_Action.m_usAniNum     = 0;
    m_Action.m_usAniNumCount= 0;
    
    while( !m_queActionData.empty() )
    {
        CharActionNode* pTempAction = m_queActionData.front();

        delete pTempAction;

        m_queActionData.pop();
    }

    m_fDeadTimer            = -1.0f;
    m_bFirstDead            = false;
    m_bCharDead             = false;
    
    m_ulAttackCombo         = 0;
    m_ulSkillComboCount     = 0;
    m_bNoneComboAttackMode  = false;
    m_bSkillCombo           = false;
    
    m_bStillCasting         = false;

    m_fChatTimer            = 0.0f;

	m_cGMModelFlag			= 0;

    m_bFlying               = false;
	m_dwFlyingTick			= 0;

    m_bSitMode              = false;
    m_bFlyFirstCheck        = false;

    m_bSlide                = false;

    m_ucCDTType             = CDT_ONLYTERRAIN;
    
    if ( m_pCreatureEffect )
    {
        delete m_pCreatureEffect;
        m_pCreatureEffect = NULL;
    }
    m_pCreatureEffect       = new RYLCreatureEffect( this );
    
    if ( m_pDamageEffect )
    {
        delete m_pDamageEffect;
        m_pDamageEffect = NULL;
    }
    m_pDamageEffect         = new RYLDamageEffect;

	if ( m_pSignpostImg )
	{
		delete m_pSignpostImg ;
	}
	m_pSignpostImg			= NULL ;

	if ( NULL == m_pCharStreetStallNameLabel )
	{
		m_pCharStreetStallNameLabel = new CRYLLabel( NULL ) ;		
		m_pCharStreetStallNameLabel->SetFontColor (0xffffffff);
		m_pCharStreetStallNameLabel->SetAutoSize( TRUE ) ;
		m_pCharStreetStallNameLabel->GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	}

	if ( NULL == m_pCharNameLabel )
	{
		m_pCharNameLabel = new CRYLLabel( NULL ) ;		
		m_pCharNameLabel->SetFontColor (0xffffffff);
		m_pCharNameLabel->SetAutoSize( TRUE ) ;
		m_pCharNameLabel->GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	}

	if ( NULL == m_pCharTempGuildNameLabel )
	{
		m_pCharTempGuildNameLabel = new CRYLLabel( NULL ) ;		
		m_pCharTempGuildNameLabel->SetFontColor (0xffffffff);
		m_pCharTempGuildNameLabel->SetAutoSize( TRUE ) ;
		m_pCharTempGuildNameLabel->GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	}

	if ( NULL == m_pCharJobNameLabel )
	{
		m_pCharJobNameLabel = new CRYLLabel( NULL ) ;		
		m_pCharJobNameLabel->SetFontColor (0xffffffff);
		m_pCharJobNameLabel->SetAutoSize( TRUE ) ;
		m_pCharJobNameLabel->GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	}

	if ( NULL == m_pCharChatMsgLabel )
	{
		m_pCharChatMsgLabel = new CRYLLabel[ 3 ] ;
		m_pCharChatMsgLabel[ 0 ].SetFontColor (0xffffffff);
		m_pCharChatMsgLabel[ 0 ].SetAutoSize( TRUE ) ;
		m_pCharChatMsgLabel[ 0 ].GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
		m_pCharChatMsgLabel[ 1 ].SetFontColor (0xffffffff);
		m_pCharChatMsgLabel[ 1 ].SetAutoSize( TRUE ) ;
		m_pCharChatMsgLabel[ 1 ].GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
		m_pCharChatMsgLabel[ 2 ].SetFontColor (0xffffffff);
		m_pCharChatMsgLabel[ 2 ].SetAutoSize( TRUE ) ;
		m_pCharChatMsgLabel[ 2 ].GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	}

	m_bRepairFullHP = TRUE ;
}

/*
//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetPosition()
void RYLCreature::SetPosition( float fPosX, float fPosY, float fPosZ )
{
    m_vecPosition.x = fPosX;	
    m_vecPosition.y = fPosY;	
    m_vecPosition.z = fPosZ;

	if ( m_pCharModel )
		m_pCharModel->SetPosition( m_vecPosition.x, m_vecPosition.y, m_vecPosition.z );
}
*/

void RYLCreature::SetPosition( vector3& vecPos )
{
    m_vecPosition = vecPos;

	if ( m_pCharModel )
	{
		// 병기이면
		/*if( RYLCreatureManager::GetClientIdentity(GetCharID()) == Creature::CT_SIEGE_OBJECT )			
		{
			vector3 vecCharPos;	
			CollisionType CT;
			vector3 vecOldPos;
			vecOldPos = vecPos;
			vector3 vecBeforePos = m_pCharModel->GetPrevPosition();

			vecBeforePos.y += 85.0f;
			vecOldPos.y+=85.0f;

			vecCharPos=vecOldPos;
			vecOldPos.y -= 40.0f;
			vector3 vecDir=vecOldPos-vecCharPos;
			vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,vecBeforePos,CT,true);			
			m_pCharModel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);
			m_vecPosition.x = vecCharPos.x;
			m_vecPosition.y = vecCharPos.y-85.0f;
			m_vecPosition.z = vecCharPos.z;
		}
		else
		{*/
			m_pCharModel->SetPosition( m_vecPosition.x, m_vecPosition.y, m_vecPosition.z );
//		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetPosition()
vector3* RYLCreature::GetPosition()
{
    if ( m_pCharModel )
	    m_pCharModel->GetPosition( m_vecPosition );

    return &m_vecPosition;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetNamePosition()
vector3* RYLCreature::GetNamePosition()
{ 
	if ( m_pCharModel )
		m_pCharModel->GetNameTagPosition( m_vecNamePosition );

	return &m_vecNamePosition;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetDirection()
void RYLCreature::SetDirection( float fDirection )
{
    m_fDirection = fDirection;

	if ( m_pCharModel )
		m_pCharModel->SetDirection( fDirection ); 
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetDirection()
float RYLCreature::GetDirection()
{
    if ( m_pCharModel )
		m_fDirection = m_pCharModel->GetDirection();

	return m_fDirection;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetActionID()
void RYLCreature::SetActionID( unsigned long* pulActionID, const char* pstrAction )
{
    string strAction = pstrAction;

	if ( !strAction.compare( "BASH" ) )
	{
		*pulActionID = CA_BASH;
	} 
    else if ( !strAction.compare( "FIRE1" ) )
	{
		*pulActionID = CA_FIRE1;
	} 
    else if ( !strAction.compare( "FIRE2" ) )
	{
		*pulActionID = CA_FIRE2;
	} 
    else if ( !strAction.compare( "GATHER01" ) )
	{
		*pulActionID = CA_GATHER01;
	} 
    else if ( !strAction.compare( "GATHER02" ) )
	{
		*pulActionID = CA_GATHER02;
	} 
    else if ( !strAction.compare( "GATHER03" ) )
	{
		*pulActionID = CA_GATHER03;
	} 
    else if ( !strAction.compare( "GATHER04" ) )
	{
		*pulActionID = CA_GATHER04;
	} 
    else if ( !strAction.compare( "GATHER05" ) )
	{
		*pulActionID = CA_GATHER05;
	} 
    else if ( !strAction.compare( "GATHER06" ) )
	{
		*pulActionID = CA_GATHER06;
	} 
    else if ( !strAction.compare( "GATHER07" ) )
	{
		*pulActionID = CA_GATHER07;
	} 
    else if ( !strAction.compare( "GATHER08" ) )
	{
		*pulActionID = CA_GATHER08;
	} 
    else if ( !strAction.compare( "GATHER09" ) )
	{
		*pulActionID = CA_GATHER09;
	} 
    else if ( !strAction.compare( "GATHER10" ) )
	{
		*pulActionID = CA_GATHER10;
	} 
	else if ( !strAction.compare( "SOCIAL_LAUGH" ) )
	{
		*pulActionID = CA_SOCIAL_LAUGH;
	} 
    else if ( !strAction.compare( "SOCIAL_DESPAIR" ) )
	{
		*pulActionID = CA_SOCIAL_DESPAIR;
	} 
    else if ( !strAction.compare( "SOCIAL_SHY" ) )
	{
		*pulActionID = CA_SOCIAL_SHY;
	} 
    else if ( !strAction.compare( "SOCIAL_SURPRISE" ) )
	{
		*pulActionID = CA_SOCIAL_SURPRISE;
	} 
    else if ( !strAction.compare( "SOCIAL_ROAR" ) )
	{
		*pulActionID = CA_SOCIAL_ROAR;
	} 
    else if ( !strAction.compare( "SOCIAL_WAVE" ) )
	{
		*pulActionID = CA_SOCIAL_WAVE;
	} 
    else if ( !strAction.compare( "SOCIAL_THANK" ) )
	{
		*pulActionID = CA_SOCIAL_THANK;
	} 
    else if ( !strAction.compare( "SOCIAL_SORRY" ) )
	{
		*pulActionID = CA_SOCIAL_SORRY;
	} 
    else if ( !strAction.compare( "SOCIAL_SALUTE" ) )
	{
		*pulActionID = CA_SOCIAL_SALUTE;
	} 
    else if ( !strAction.compare( "SOCIAL_CHEER" ) )
	{
		*pulActionID = CA_SOCIAL_CHEER;
	} 
    else if ( !strAction.compare( "SOCIAL_NOD" ) )
	{
		*pulActionID = CA_SOCIAL_NOD;
	} 
    else if ( !strAction.compare( "SOCIAL_NO" ) )
	{
		*pulActionID = CA_SOCIAL_NO;
	} 
    else if ( !strAction.compare( "SOCIAL_CHARGE" ) )
	{
		*pulActionID = CA_SOCIAL_CHARGE;
	} 
    else if ( !strAction.compare( "SOCIAL_COMFORT" ) )
	{
		*pulActionID = CA_SOCIAL_COMFORT;
	} 
    else if ( !strAction.compare( "SOCIAL_TALK" ) )
	{
		*pulActionID = CA_SOCIAL_TALK;
	} 
	else if ( !strAction.compare( "COMBINATION_BLOW" ) )
	{
		*pulActionID = CA_COMBINATIONBLOW;
	} 
    else if ( !strAction.compare( "BACKSTAB" ) )
	{
		*pulActionID = CA_BACKSTAB;
	} 
    else if ( !strAction.compare( "AIMED_SHOT1" ) )
	{
		*pulActionID = CA_AIMEDSHOT1;
	} 
    else if ( !strAction.compare( "AIMED_SHOT2" ) )
	{
		*pulActionID = CA_AIMEDSHOT2;
	} 
    else if ( !strAction.compare( "AIMED_SHOT3" ) )
	{
		*pulActionID = CA_AIMEDSHOT3;
	} 
    else if ( !strAction.compare( "CASTING" ) )
	{
		*pulActionID = CA_CASTING;
	} 
    else if ( !strAction.compare( "JUMP_ATTACK" ) )
	{
		*pulActionID = CA_JUMPATTACK;
	} 
    else if ( !strAction.compare( "JUMP_ATTACK2" ) )
	{
		*pulActionID = CA_JUMPATTACK2;
	} 
    else if ( !strAction.compare( "SHOT" ) )
	{
		*pulActionID = CA_SHOT;
	} 
    else if ( !strAction.compare( "JUMP_SHOT" ) )
	{
		*pulActionID = CA_JUMPSHOT;
	} 
    else if ( !strAction.compare( "OVER_BASH1" ) )
	{
		*pulActionID = CA_OVERBASH1;
	} 
    else if ( !strAction.compare( "OVER_BASH2" ) )
	{
		*pulActionID = CA_OVERBASH2;
	} 
    else if ( !strAction.compare( "OVER_BASH3" ) )
	{
		*pulActionID = CA_OVERBASH3;
	} 
    else if ( !strAction.compare( "OVER_BASH4" ) )
	{
		*pulActionID = CA_OVERBASH4;
	} 
    else if ( !strAction.compare( "OVER_BASH5" ) )
	{
		*pulActionID = CA_OVERBASH5;
	} 
    else if ( !strAction.compare( "POWER_DRAIN" ) )
	{
		*pulActionID = CA_POWERDRAIN;
	} 
    else if ( !strAction.compare( "TURN_PUNCH3" ) )
	{
		*pulActionID = CA_TURNPUNCH3;
	} 
    else if ( !strAction.compare( "JUMP_ATTACL3" ) )
	{
		*pulActionID = CA_JUMPATTACK3;
	}
    else if ( !strAction.compare( "STUN" ) )
	{
		*pulActionID = CA_STUN;
	} 
    else if ( !strAction.compare( "ROUND_SWING" ) )
	{
		*pulActionID = CA_ROUNDSWING;
	} 
    else if ( !strAction.compare( "SUMMON" ) )
	{
		*pulActionID = CA_SUMMON;
	} 
    else if ( !strAction.compare( "HIT1" ) )
	{
		*pulActionID = CA_HIT1;
	} 
    else if ( !strAction.compare( "WAIT_H1" ) )
	{
		*pulActionID = CA_WAITH1;
	} 
    else if ( !strAction.compare( "HIT2" ) )
	{
		*pulActionID = CA_HIT2;
	} 
    else if ( !strAction.compare( "WAIT_H2" ) )
	{
		*pulActionID = CA_WAITH2;
	}
	else if ( !strAction.compare( "SA_ADVANCE" ) )
	{
		*pulActionID = CA_ADVANCE;
	}
	else if ( !strAction.compare( "MOVE_FORWARD" ) )
	{
		*pulActionID = CA_FORWARD;
	}
	else if ( !strAction.compare( "MOVE_BACKWARD" ) )
	{
		*pulActionID = CA_BACKWARD;
	}
}

void RYLCreature::SetActionString( const unsigned long ulActionID, char* pstrAction )
{
    CollisionType CT = CSceneManager::GetCharacterCollisionType( m_pCharModel );

	if ( CT == CT_WATER )
	{
		strcpy( pstrAction, "SWIM" );
		if (m_pCharModel)
			m_pCharModel->SetFixHeight(true);
		return;
	}
	else
	{
		m_pCharModel->SetFixHeight(false);
	}

	switch ( ulActionID )
	{
		case CA_WAIT:
			strcpy( pstrAction, "WAIT" );
			break;

		case CA_WALK:
			strcpy( pstrAction, "WALK" );
			break;

		case CA_WALKRIGHT:
			strcpy( pstrAction, "WALK_RIGHT" );
			break; 

		case CA_WALKLEFT:
			strcpy( pstrAction, "WALK_LEFT" );
			break; 

		case CA_WALKBACK:
			strcpy( pstrAction, "WALK_BACK" );
			break; 

		case CA_RUN:
			strcpy( pstrAction, "RUN" );
			break; 

		case CA_REST:
			strcpy( pstrAction, "REST" );
			break; 
            
		case CA_SITDOWN:
			strcpy( pstrAction, "SIT_DOWN" );
			break;

		case CA_STANDUP:
			strcpy( pstrAction, "STAND_UP" );
			break; 

		case CA_ATTACK:
			strcpy( pstrAction, "ATTACK" );
			break; 

		case CA_ATTACKADVANCE:
			strcpy( pstrAction, "ATTACK_ADVANCE" );
			break; 

		case CA_ATTACKLEFT:
			strcpy( pstrAction, "ATTACK_LEFT" );
			break; 

		case CA_ATTACKRIGHT:
			strcpy( pstrAction, "ATTACK_RIGHT" );
			break; 

		case CA_ATTACKRETREAT:
			strcpy( pstrAction, "ATTACK_RETREAT" );
			break; 

        
        case CA_ATTACK2:
			strcpy( pstrAction, "ATTACK2" );
			break;
		
        case CA_ATTACK3:
			strcpy( pstrAction, "ATTACK3" );
			break;
		
        case CA_ATTACK4:
			strcpy( pstrAction, "ATTACK4" );
			break;
		
        case CA_ATTACK5:
			strcpy( pstrAction, "ATTACK5" );
			break;
		
        case CA_ATTACK6:
			strcpy( pstrAction, "ATTACK6" );
			break;
		
        case CA_FALLDOWN:
			strcpy( pstrAction, "FALLDOWN" );
			break;
		
        case CA_BASH:
			strcpy( pstrAction, "BASH" );
			break;
		
        case CA_COMBINATIONBLOW:
			strcpy( pstrAction, "COMBINATION_BLOW" );
			break;
		
        case CA_BACKSTAB:
			strcpy( pstrAction, "BACKSTAB" );
			break;
		
        case CA_FIRE1:
			strcpy( pstrAction, "FIRE1" );
			break;
		
        case CA_FIRE2:
			strcpy( pstrAction, "FIRE2" );
			break;

        case CA_GATHER01		:	strcpy( pstrAction, "GATHER01" );	break;
        case CA_GATHER02		:	strcpy( pstrAction, "GATHER02" );	break;
        case CA_GATHER03		:	strcpy( pstrAction, "GATHER03" );	break;
        case CA_GATHER04		:	strcpy( pstrAction, "GATHER04" );	break;
        case CA_GATHER05		:	strcpy( pstrAction, "GATHER05" );	break;
        case CA_GATHER06		:	strcpy( pstrAction, "GATHER06" );	break;
        case CA_GATHER07		:	strcpy( pstrAction, "GATHER07" );	break;
        case CA_GATHER08		:	strcpy( pstrAction, "GATHER08" );	break;
        case CA_GATHER09		:	strcpy( pstrAction, "GATHER09" );	break;
        case CA_GATHER10		:	strcpy( pstrAction, "GATHER10" );	break;

        case CA_SOCIAL_LAUGH	:	strcpy( pstrAction, "SOCIAL_LAUGH" );	break;
        case CA_SOCIAL_DESPAIR	:	strcpy( pstrAction, "SOCIAL_DESPAIR" );	break;
        case CA_SOCIAL_SHY		:	strcpy( pstrAction, "SOCIAL_SHY" );		break;
        case CA_SOCIAL_SURPRISE	:	strcpy( pstrAction, "SOCIAL_SURPRISE" );break;
        case CA_SOCIAL_ROAR		:	strcpy( pstrAction, "SOCIAL_ROAR" );	break;
        case CA_SOCIAL_WAVE		:	strcpy( pstrAction, "SOCIAL_WAVE" );	break;
        case CA_SOCIAL_THANK	:	strcpy( pstrAction, "SOCIAL_THANK" );	break;
        case CA_SOCIAL_SORRY	:	strcpy( pstrAction, "SOCIAL_SORRY" );	break;
        case CA_SOCIAL_SALUTE	:	strcpy( pstrAction, "SOCIAL_SALUTE" );	break;
        case CA_SOCIAL_CHEER	:	strcpy( pstrAction, "SOCIAL_CHEER" );	break;
        case CA_SOCIAL_NOD		:	strcpy( pstrAction, "SOCIAL_NOD" );		break;
        case CA_SOCIAL_NO		:	strcpy( pstrAction, "SOCIAL_NO" );		break;
        case CA_SOCIAL_CHARGE	:	strcpy( pstrAction, "SOCIAL_CHARGE" );	break;
        case CA_SOCIAL_COMFORT	:	strcpy( pstrAction, "SOCIAL_COMFORT" );	break;
        case CA_SOCIAL_TALK		:	strcpy( pstrAction, "SOCIAL_TALK" );	break;

		case CA_SHOT:
			strcpy( pstrAction, "SHOT" );
			break;
		
        case CA_JUMPSHOT:
			strcpy( pstrAction, "JUMP_SHOT" );
			break;
		
        case CA_JUMP:
			strcpy( pstrAction, "JUMP" );
			break;
		
        case CA_FORWARDJUMP:
			strcpy( pstrAction, "JUMP" );
			break;
		
        case CA_BACKJUMP:
			strcpy( pstrAction, "JUMP" );
			break;
		
        case CA_FRONTDASH:
			strcpy( pstrAction, "DODGE_FRONT" );
			break;
		
        case CA_LEFTDASH:
			strcpy( pstrAction, "DODGE_LEFT" );
			break;
		
        case CA_RIGHTDASH:
			strcpy( pstrAction, "DODGE_RIGHT" );
			break;
		
        case CA_AIMEDSHOT1:
			strcpy( pstrAction, "AIMED_SHOT1" );
			break;
		
        case CA_AIMEDSHOT2:
			strcpy( pstrAction, "AIMED_SHOT2" );
			break;
		
        case CA_AIMEDSHOT3:
			strcpy( pstrAction, "AIMED_SHOT3" );
			break;
		
        case CA_SWIM:
			strcpy( pstrAction, "SWIM" );
			break;
		
        case CA_CASTING:
			strcpy( pstrAction, "CASTING" );
			break;
		
        case CA_BACKDASH:
			strcpy( pstrAction, "DODGE_BACK" );
			break;
		
        case CA_JUMPATTACK:
			strcpy( pstrAction, "JUMP_ATTACK" );
			break;
		
        case CA_JUMPATTACK2:
			strcpy( pstrAction, "JUMP_ATTACK2" );
			break;
		
        case CA_LAND:
			strcpy( pstrAction, "LAND" );
			break; 
		
        case CA_OVERBASH1:
			strcpy( pstrAction, "OVER_BASH1" );
			break;
		
        case CA_OVERBASH2:
			strcpy( pstrAction, "OVER_BASH2" );
			break;
		
        case CA_OVERBASH3:
			strcpy( pstrAction, "OVER_BASH3" );
			break;
		
        case CA_OVERBASH4:
			strcpy( pstrAction, "OVER_BASH4" );
			break;
		
        case CA_OVERBASH5:
			strcpy( pstrAction, "OVER_BASH5" );
			break; 
		
        case CA_POWERDRAIN:
			strcpy( pstrAction, "POWER_DRAIN" );
			break; 
		
        case CA_TURNPUNCH3:
			strcpy( pstrAction, "TURN_PUNCH3" );
			break; 
		
        case CA_JUMPATTACK3:
			strcpy( pstrAction, "JUMP_ATTACK3" );
			break; 
		
        case CA_STUN:
			strcpy( pstrAction, "STUN" );
			break; 
		
        case CA_ROUNDSWING:
			strcpy( pstrAction, "ROUND_SWING" );
			break; 
		
        case CA_SUMMON:
			strcpy( pstrAction, "SUMMON" );
			break; 
		
        case CA_HIT1:
			strcpy( pstrAction, "HIT1" );
			break; 
		
        case CA_WAITH1:
			strcpy( pstrAction, "WAIT_H1" );
			break; 
		
        case CA_HIT2:
			strcpy( pstrAction, "HIT2" );
			break; 
		
        case CA_WAITH2:
			strcpy( pstrAction, "WAIT_H2" );
			break; 

		case CA_ADVANCE:
			strcpy( pstrAction, "SA_ADVANCE" );
			break;

		case CA_FORWARD:
			strcpy( pstrAction, "MOVE_FORWARD" );
			break;

		case CA_BACKWARD:
			strcpy( pstrAction, "MOVE_BACKWARD" );
			break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetAction()
bool RYLCreature::SetAction( unsigned long ulUpperAction, unsigned long ulLowerAction )
{
    char strUpperAction[ 128 ];
    char strLowerAction[ 128 ];
	bool bAction = false;

	if ( m_pCharModel )
	{
        SetActionString( ulUpperAction, strUpperAction );
        SetActionString( ulLowerAction, strLowerAction );

        bAction = SetActionSub( ulUpperAction, strUpperAction, ulLowerAction, strLowerAction );
	}
    
    return bAction;
}

bool RYLCreature::SetAction( const char* pstrUpperAction, const char* pstrLowerAction )
{
    unsigned long ulUpperAction, ulLowerAction;
    bool bAction = false;

    CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( m_ulCharID );
    if ( pData && !pData->m_strName.compare( "공성병기" ) )
    {
        int i = 0;
        i = 1;
    }

	if ( m_pCharModel )
	{
        SetActionID( &ulUpperAction, pstrUpperAction );
	    SetActionID( &ulLowerAction, pstrLowerAction );

        bAction = SetActionSub( ulUpperAction, pstrUpperAction, ulLowerAction, pstrLowerAction );
	} 
    
	return bAction;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::UpdatePlayerAction()
void RYLCreature::UpdatePlayerAction()
{
    if ( m_pCharModel )
    {
        InitCharAction();

        if ( m_ulLowerCharActionID != CA_FORWARDJUMP
		 && m_ulLowerCharActionID != CA_BACKJUMP
		 && m_ulLowerCharActionID != CA_JUMP
		 && m_ulLowerCharActionID != CA_FRONTDASH
		 && m_ulLowerCharActionID != CA_JUMPATTACK
		 && m_ulLowerCharActionID != CA_JUMPATTACK2
		 && m_ulLowerCharActionID != CA_FALLDOWN )
        {
            if ( m_pCharModel->IsActionFinished( 0 ) )
            {
                CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );
            }

            if ( GetObjectType() == Siege::GATE )
                return;

            SetAction( m_ulUpperCharActionID, m_ulLowerCharActionID );
        } 

		if( m_ulUpperCharActionID == CA_ATTACK )
		{
			//====================================================
			// EventEffect				
			Z3DTOK TokEffect;
			if( m_pCharModel->CheckEvent( "EFFECT", &TokEffect ) )
			{
				const char* pstrEffectName = g_TokEventName.GetString( TokEffect );

				m_pCreatureEffect->MonsterEffectProcess( pstrEffectName, 0 );
			}
			//====================================================
		}
		else
		{
			//====================================================				
			// Event Effect 처리 부분 				
			Z3DTOK TokEffect;
			if( m_pCharModel->CheckEvent( "EFFECT",&TokEffect ) )
			{
				const char* pstrEffectName = g_TokEventName.GetString( TokEffect );
				m_pCreatureEffect->MonsterEffectProcess( pstrEffectName, 1 );
			}
			//====================================================	
		}

        if ( !m_queActionData.empty() )
        {
            CharActionNode* pTempAction = m_queActionData.front();
            m_queActionData.pop();
            m_Action = (*pTempAction);
            delete pTempAction;

            SetDirection( m_Action.m_fDirection );

            if ( m_Action.m_ulUpperChrAction == CA_REST )
            {
                SetSitMode( true );
            }
            else if ( m_Action.m_ulUpperChrAction != CA_STANDUP && m_Action.m_ulUpperChrAction != CA_SITDOWN )
            {
                SetSitMode( false );
            }

            if ( m_Action.m_ulUpperChrAction != CA_CASTING 
             && m_Action.m_ulUpperChrAction != CA_AIMEDSHOT1
             && m_Action.m_ulUpperChrAction != CA_AIMEDSHOT2 )
            {
                SetStillCasting( false );
            }

            if ( m_Action.m_ulLowerChrAction == CA_RUN )
            {
                if ( m_fRunFactor < 255.0f )
                {
                    m_fRunFactor = CFrameTimer::GetUpdateTimer( m_ulRunFactorTimer );
                }
                else
                {
                    m_fRunFactor = 0.0f;
                }
            }

            if ( m_Action.m_ulLowerChrAction == CA_RESPAWN )
            {
                Respawn();
            }

            vector3 vecDistance = m_Action.m_vecNextPosition - m_vecPosition;

            if ( vecDistance.GetLens() > 1000.0f )
            {
                m_vecPosition = m_Action.m_vecNextPosition;
                if ( m_pCharModel )
                {
                    m_pCharModel->SetPosition( m_Action.m_vecNextPosition );
                    CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );
                }
            }
            else
            {
                if ( m_pCharModel )
                {
                    vector3 vecMove = vecDistance / ( 35.0f / 4.0f );

                    switch ( m_Action.m_ulLowerChrAction )
                    {
                    case CA_SWIM:
                    case CA_JUMPATTACK2:
				    case CA_JUMPATTACK:
				    case CA_FORWARDJUMP:
				    case CA_FRONTDASH:
				    case CA_RUN:
                    case CA_BACKJUMP:
				    case CA_WALK:
				    case CA_WALKBACK:
				    case CA_WALKLEFT: 
				    case CA_WALKRIGHT:
                        {
					        CSceneManager::CharacterMovementVector( m_pCharModel, vecMove );
                        }
					    break;

				    case CA_LEFTDASH:
				    case CA_RIGHTDASH:
				    case CA_BACKDASH:
                        {
					        CSceneManager::CharacterMovementVector( m_pCharModel, vecMove ,-1.1f );
                        }
					    break;

				    default:
                        {                                                                                       
					        m_pCharModel->SetPosition( m_Action.m_vecNextPosition );
					        CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );
                        }
					    break;
                    }
                }
            }

            if ( m_Action.m_ulUpperChrAction != CA_FALLDOWN )
            {
                m_bCharDead = false;
            }

            if ( !m_bCharDead )
            {
                if ( m_ulUpperCharActionID == CA_SWIM && m_ulLowerCharActionID != CA_SWIM )
                {
                    SetAction( CA_WAIT, CA_WAIT );
                    m_bUpperAble = true;
                    m_bLowerAble = true;
                    m_bUpperActing = false;
                    m_bLowerActing = false;
                }
                else if ( m_Action.m_ulUpperChrAction == CA_ATTACK || m_Action.m_ulUpperChrAction == CA_ATTACK2 ||
						 m_Action.m_ulUpperChrAction == CA_ATTACK3 || m_Action.m_ulUpperChrAction == CA_ATTACK4 ||
						 m_Action.m_ulUpperChrAction == CA_ATTACK5 || m_Action.m_ulUpperChrAction == CA_ATTACK6 ||
						 m_Action.m_ulUpperChrAction == CA_ATTACKADVANCE || m_Action.m_ulUpperChrAction == CA_ATTACKLEFT ||
						 m_Action.m_ulUpperChrAction == CA_ATTACKRIGHT || m_Action.m_ulUpperChrAction == CA_ATTACKRETREAT )
                {
                    if ( m_Action.m_ulUpperChrAction == CA_ATTACK || m_Action.m_ulUpperChrAction == CA_ATTACK2 ||
						m_Action.m_ulUpperChrAction == CA_ATTACK3 || m_Action.m_ulUpperChrAction == CA_ATTACK4 ||
						m_Action.m_ulUpperChrAction == CA_ATTACK5 || m_Action.m_ulUpperChrAction == CA_ATTACK6 )
                    {
                        SetAction( CA_WAIT, CA_WAIT );
                        m_bUpperAble = true;
                        m_bLowerAble = true;
                        m_bUpperActing = false;
                        m_bLowerActing = false;
                    }

                    Attack( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
                }
                else if ( m_Action.m_ulUpperChrAction == CA_STANDUP || m_Action.m_ulUpperChrAction == CA_SITDOWN )
				{
                    ToggleCrouch();
				} 
                else if ( m_Action.m_ulUpperChrAction == CA_FALLDOWN )
				{
					Dead();
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_LEFTDASH )
				{
					Dash( 0 );
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_RIGHTDASH )
				{
					Dash( 1 );
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_BACKDASH )
				{
					Dash( 2 );
				}
				else if ( m_Action.m_ulLowerChrAction == CA_CASTING ||
					     m_Action.m_ulLowerChrAction == CA_AIMEDSHOT1 || 
						 m_Action.m_ulLowerChrAction == CA_AIMEDSHOT2 )
				{
					Casting( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_JUMPATTACK )
				{
					if ( GetLowerActionID() != CA_JUMPATTACK )
					{
						m_bUpperAble = true;
                        m_bLowerAble = true;
                        m_bUpperActing = false;
                        m_bLowerActing = false;
						SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
					}
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_JUMPATTACK2 )
				{
					if ( GetLowerActionID() != CA_JUMPATTACK2 )
					{
						m_bUpperAble = true;
                        m_bLowerAble = true;
                        m_bUpperActing = false;
                        m_bLowerActing = false;
						SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
					}
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_JUMP )
				{
					if ( GetLowerActionID() != CA_JUMP )
					{
						SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
					}
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_FRONTDASH )
				{
					if ( GetLowerActionID() != CA_FRONTDASH )
					{
						SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
					}
				} 
				else if ( m_Action.m_ulLowerChrAction == CA_FORWARDJUMP )
				{
					if ( GetLowerActionID() != CA_FORWARDJUMP )
					{
						SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
					}
				}
				else if ( m_Action.m_ulLowerChrAction == CA_BACKJUMP )
				{
					if ( GetLowerActionID() != CA_BACKJUMP )
					{
						SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
					}
				}
				else
				{
					if( CA_GATHERSTART <= m_Action.m_ulLowerChrAction && m_Action.m_ulLowerChrAction <= CA_GATHEREND)
						InitWeapon( 0 );

					if( CA_SOCIALSTART <= m_Action.m_ulLowerChrAction && m_Action.m_ulLowerChrAction <= CA_SOCIALEND)
						InitWeapon( 0 );

					SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
				}
            }
        }
    }
    else
    {
        if ( !m_queActionData.empty() )
        {
            CharActionNode* pTempAction = m_queActionData.front();
            m_queActionData.pop();
            m_Action = (*pTempAction);
            delete pTempAction;

            m_vecPosition = m_Action.m_vecNextPosition;
            m_fDirection = m_Action.m_fDirection;

            if ( m_Action.m_ulUpperChrAction != CA_FALLDOWN )
            {
                m_bCharDead = false;
            }

            if ( !m_bCharDead )
            {
                if ( m_Action.m_ulUpperChrAction == CA_FALLDOWN )
                {
                    Dead();
                }
                else
                {
                    SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::UpdateMonsterAction()
void RYLCreature::UpdateMonsterAction()
{
	if ( m_pCharModel )
    {
        InitCharAction();

        if ( m_pCharModel->IsActionFinished( 0 ) )
        {
            // Action Queue가 비어 있으면
            if ( m_queActionData.empty() )
            {
                // 현재 액션을 계속 취해준다.
				if ( !m_Action.m_usAniNumCount )//&& m_Action.m_usAniNum != 0 )
                {
                    CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );
                    SetAction( m_ulUpperCharActionID, m_ulLowerCharActionID );
                }
                
            }
        }

		bool bForceMove = FALSE ;
        if ( !m_queActionData.empty() || m_Action.m_usAniNumCount )
        {
            if ( !m_queActionData.empty() )
            {
                CharActionNode* pTempAction = m_queActionData.front();
                m_queActionData.pop();
                m_Action = (*pTempAction);
                delete pTempAction;

				bForceMove = TRUE ;
            }

			if ( m_pCharModel->IsActionFinished( 0 ) || bForceMove )
			{
				m_fDirection = m_Action.m_fDirection;
				m_pCharModel->SetDirection( m_fDirection );

				if ( m_Action.m_ulLowerChrAction == CA_ATTACK 
				|| m_Action.m_ulLowerChrAction == CA_CASTING )
				{
					switch( m_Action.m_ulLowerChrAction )
					{
					case CA_ATTACK:
						{
							m_pCharModel->AddMotion( "ATTACK", 0, 0.7f );
						}
						break;

					case CA_CASTING:
						{   
							m_pCharModel->AddMotion( "CASTING", 0, 0.7f );
						}
						break;

					}

					--m_Action.m_usAniNumCount;

					//====================================================
					// Monster EventEffect				
					Z3DTOK TokEffect;
					if ( m_pCharModel->CheckEvent( "EFFECT", &TokEffect ) )
					{
						const char* pstrEffectName = g_TokEventName.GetString( TokEffect );

						m_pCreatureEffect->MonsterEffectProcess( pstrEffectName, 0 );
					}
					//====================================================
					return;
				}
				// Attack 동작이 아닐 경우
				else	 
				{
					//====================================================				
					// Monster Event Effect 처리 부분 				
					Z3DTOK TokEffect;
					if ( m_pCharModel->CheckEvent( "EFFECT",&TokEffect ) )
					{
						const char* pstrEffectName = g_TokEventName.GetString( TokEffect );
						m_pCreatureEffect->MonsterEffectProcess( pstrEffectName, 1 );
					}
					//====================================================				
				}

				vector3 vecMove( cosf( ( -m_Action.m_fDirection ) - FLOAT_PHI / 2.0f ), 0.0f, sinf( ( -m_Action.m_fDirection ) - FLOAT_PHI / 2.0f ) );

				if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Slow ) )
				{
					vecMove *= m_Action.m_fVec / 4.0f;
				}
				else if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
				{
					vecMove *= m_Action.m_fVec / 2.0f;
				}
				else
				{
					vecMove *= m_Action.m_fVec;
				}

				vecMove *= 100.0f;

				switch ( m_Action.m_ulLowerChrAction )
				{
					case CA_RUN:
					case CA_WALK:
					case CA_WALKBACK:
					case CA_WALKLEFT:
					case CA_WALKRIGHT:
						{
							CSceneManager::CharacterMovementVector( m_pCharModel, vecMove );
						}
						break;

					default:
						{
							m_pCharModel->SetPosition( m_Action.m_vecNextPosition );
							CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );
						}
						break;
				}

				if ( !m_bCharDead )
				{
					SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
				}

				// edith 2009.08.06 서버에서 AniNum이 0이올때 무한으로 애니메이션 도는걸 방지하기 위한 루틴
//				if(m_Action.m_usAniNumCount > 0)
					--m_Action.m_usAniNumCount;
			}
		}
    }
    else
    {
        if ( !m_queActionData.empty() || m_Action.m_usAniNumCount )
        {
            if ( !m_queActionData.empty() )
            {
                CharActionNode* pTempAction = m_queActionData.front();
                m_queActionData.pop();
                m_Action = (*pTempAction);
                delete pTempAction;
            }

            vector3 vecMove( cosf( ( -m_Action.m_fDirection ) - FLOAT_PHI / 2.0f ), 0.0f, sinf( ( -m_Action.m_fDirection ) - FLOAT_PHI / 2.0f ) );
            vecMove *= m_Action.m_fVec;
            vecMove *= m_Action.m_ulFrame;
			vecMove *= 100.0f;
            vecMove *= m_Action.m_usAniNum;

            m_fDirection = m_Action.m_fDirection;
            vector3 vecTempPos = m_Action.m_vecNextPosition + vecMove;
            m_vecPosition = vecTempPos;

            if ( !m_bCharDead )
            {
                SetAction( m_Action.m_ulUpperChrAction, m_Action.m_ulLowerChrAction );
            }

            m_Action.m_usAniNum = m_Action.m_usAniNumCount = 0;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::AddActionData()
void RYLCreature::AddActionData( CharActionNode* pAction )
{
    if ( pAction->m_ulLowerChrAction > CA_ACTIONEND )
    {
        delete pAction;
        pAction = NULL;
        return;
    }
/*
	// edith 2009.08.06 스탑동작일때 모든 액션 정보 초기화
	if(pAction->m_usAniNum == 0 && pAction->m_usAniNumCount == 0)
	{
	    while( !m_queActionData.empty() )
		{
			CharActionNode* pTempAction = m_queActionData.front();

			delete pTempAction;

			m_queActionData.pop();
		}

		m_ulUpperCharActionID   = CA_WAIT;
		m_ulLowerCharActionID   = CA_WAIT;
		m_bUpperActing          = false;
		m_bLowerActing          = false;
		m_bUpperAble            = true;        
		m_bLowerAble            = true;

		m_Action.m_usAniNum     = 0;
		m_Action.m_usAniNumCount= 0;
	}
*/
	m_queActionData.push( pAction );
}

// it's private member( using in public SetAction() )
//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetActionSub()
bool RYLCreature::SetActionSub( unsigned long ulUpperAction, const char* pstrUpperAction, unsigned long ulLowerAction, const char* pstrLowerAction )
{
    if ( !m_bCharDead )
    {
        m_bFirstDead = false;
    }

    if ( m_bFirstDead )
        return false;

    if ( !m_bFirstDead && m_bCharDead )
    {
        m_bFirstDead = true;
    }

    bool bAction = false;

    if ( m_ulUpperCharActionID != ulUpperAction )
	{
		if ( IsCancelAction( 1 ) )
		{
			m_ulUpperCharActionID = ulUpperAction;
			m_bUpperActing = false;
            m_bUpperAble = IsCancelAction( 1 ) ? true : false;
		}
	}

	if ( m_ulLowerCharActionID != ulLowerAction )
	{
		if ( IsCancelAction( 2 ) )
		{
			m_ulLowerCharActionID = ulLowerAction;
			m_bLowerActing = false;
			m_bLowerAble = IsCancelAction( 2 ) ? true : false;
		}
	}

	if ( ( ( m_ulUpperCharActionID == CA_AIMEDSHOT1 || m_ulUpperCharActionID == CA_AIMEDSHOT2 || m_ulUpperCharActionID == CA_CASTING || m_ulUpperCharActionID == CA_REST || m_ulUpperCharActionID == CA_WAIT || m_ulUpperCharActionID == CA_WAITH1 || m_ulUpperCharActionID == CA_WAITH2) && !m_bUpperActing ) &&
		( ( m_ulLowerCharActionID == CA_AIMEDSHOT1 || m_ulLowerCharActionID == CA_AIMEDSHOT2 || m_ulLowerCharActionID == CA_CASTING || m_ulLowerCharActionID == CA_REST || m_ulLowerCharActionID == CA_WAIT || m_ulLowerCharActionID == CA_WAITH1 || m_ulLowerCharActionID == CA_WAITH2) && !m_bLowerActing ) )
	{
		m_pCharModel->SetMotion( pstrUpperAction, 3 );
		m_ulUpperCharActionID = ulUpperAction;
		m_ulLowerCharActionID = ulLowerAction;
		m_bUpperActing = true;
	
        m_bLowerActing = true;
        m_bUpperAble = IsCancelAction( 1 ) ? true : false;
        m_bLowerAble = IsCancelAction( 2 ) ? true : false;

		return true;
	}

	if ( !m_bUpperActing )
	{
		m_pCharModel->SetMotion( pstrUpperAction, 1 );
		m_ulUpperCharActionID = ulUpperAction;
		m_bUpperActing = true;
		bAction = true;
        m_bUpperAble = IsCancelAction( 1 ) ? true : false;
	}

	if ( !m_bLowerActing )
	{
		m_pCharModel->SetMotion( pstrLowerAction, 2 );
		m_ulLowerCharActionID = ulLowerAction;
		m_bLowerActing = true;
		bAction = true;
        m_bLowerAble = IsCancelAction( 2 ) ? true : false;
	}
    
    return bAction;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetShape()
void RYLCreature::SetShape( const char** strShape, const Broadcast2nd::CNetworkEquipGrade& equipGrade, bool bForceChange )
{
	// Minotaurs  장비 장착
	using namespace Item::EquipmentPos;

    CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( m_ulCharID );

    if ( !pData )
        return;

    m_ucRace  = pData->m_cRace;
    m_usClass = pData->m_usClass;

	bool bAvata = false;
    bool bChange = bForceChange, bWeapon = bForceChange, bShield = bForceChange;

	for ( int i = 0; i < MAX_EQUPMENT_POS; ++i )
	{
		// 변화없이 비었을 때
		if ( NULL == m_strShape[ i ] && NULL == strShape[ i ] )
		{
			continue;
		}

		// 빈 곳에 장착하거나 장착한 것을 벗을 때
		if ( NULL == m_strShape[ i ] || NULL == strShape[ i ] )
		{
			bChange = true;
		}
		// 이미 장비하고 있는 곳을 다른 것으로 바꿀 때
		else
		{
			if ( strcmp( m_strShape[ i ], strShape[ i ] ) )
			{          
				switch ( i )
				{
					case SHIELD_HAND1:
					case SHIELD_HAND2:
					{
						if ( m_ucRace == 0 )
						{
							bShield = true;
						} 
						else
						{
							bChange = true;
						}
						break;
					}

					case WEAPON_HAND1:
					case WEAPON_HAND2:
					{
						bWeapon = true;
						break;
					}
					case AVATA:
					{
						bChange = true;
						m_ucEnableRide = 0;
						break;
					}

					default:
					{
						bChange = true;
						break;
					}
				}
			}
		}

		m_strShape[ i ] = strShape[ i ];
	}

	m_EquipGrade = equipGrade;

	if ( m_pCharModel == NULL ) 
        return;

	// m_ucRideFlag  0 무조건 내린다. 1 이면 무조건 탄다.

	// 내린다.
	if(m_strShape[ AVATA ] == NULL)
	{
		if(m_ucEnableRide == 0)	// 아직 안탔다.
			bAvata = false;
		else if(m_ucEnableRide == 1)	// 현재 탄 이미지다
		{
			bAvata = true;
			bChange = true;
		}
	}
	else 
	{
		// 아바타 아이템이 지정되어있다.
		if(m_ucEnableRide == m_ucRideFlag)	
			bAvata = false;			// 상태가 같으면 아바타를 바꾸지 않는다.
		else
		{
			bAvata = true;			// 타고있는 플래그와 현재 상태가 다르니깐 바꾼다.
			bChange = true;
		}
	}

	// Avata에서 교체가 일어났다.
	if(bAvata)
	{	
		
		// 상태 변경
		// 물이 아닐 때 점프 착지 사운드 출력
		SetFlying(false);
		SetFlyFirstCheck(false);	

		SetUpperActing(false);
		SetLowerActing(false);

		SetJumpAttackVector(vector3(0.0f, 0.0f, 0.0f));
		CCameraControl::m_fJumpRate = 0.01f;

		CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData(GetCharID());
		if (GetLowerActionID() != CA_JUMPATTACK2)
		{
			// edith 점프
			// 연속 점프가 아닌 경우 착지 처리
			if ((pSelfData->m_usClass != CClass::Rogue && 
				pSelfData->m_usClass != CClass::Assassin && 
				pSelfData->m_usClass != CClass::ShadowOff) || !KeyPressed(VK_SPACE))		
			{
				SetAction(CA_LAND, CA_LAND);
				m_ulSkillComboCount = 0;
			}
		} 
		
		bool bSwim = m_pCharModel->GetFixHeight();
		
		const vector3	vPos	= *(GetPosition());
/*
		// 계속 번쩍여서 뺀다. 대신 나자신만 Q를 눌러 변신할때만 넣어줌
		CEffScript *pLevelUpEff = new CEffScript;
		CSceneManager::AddEffectScript( pLevelUpEff );

		pLevelUpEff->GetScriptBinData( "respawn.esf", pGame->m_bSelfEffect );
		pLevelUpEff->SetStartPos(vPos.x,vPos.y,vPos.z);			

		pLevelUpEff->SetEndPos( (D3DXVECTOR3*)GetPosition(), 1 );
*/		
		if(m_strShape[AVATA] == NULL || m_ucEnableRide == 1) // Avata를 빼면 원래 모습으로 돌아간다.
		{
			POS psPos = { 0.0f, 0.0f, 0.0f };

			const float		fDir	= GetDirection();										

			char strSex[MAX_PATH];
					
			switch (m_ucRace)
			{
			case CClass::RaceType::HUMAN :
				{
					if ( pData->m_cSex == 1 )
					{
						strcpy( strSex, "PC_MAN.GCMDS" ) ; 
					}
					else 
					{
						strcpy( strSex, "PC_WOMAN.GCMDS" ) ;
					}
					break;
				}

			case CClass::RaceType::AKHAN :
				{
					if ( pData->m_cSex == 1 )
					{
						strcpy( strSex, "PC_Akhan_A.GCMDS" ) ; 
					}
					else 
					{
						strcpy( strSex, "PC_Akhan_B.GCMDS" ) ;
					}
					break;
				}
			}

			SetModelFileName(strSex);
			CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strSex);

			if(pCharModel)
			{
				pCharModel->SetMotionSheet( "NO_WEAPON" );
				pCharModel->SetPosition(vPos);
				pCharModel->SetDirection(fDir);
			}
			m_ucEnableRide = 0;
		}
		else if(m_ucEnableRide == 0)
		{
			char strModel[128];
			strcpy(strModel, strShape[AVATA]);
			
			// 신규 탈것.
			if(strnicmp(strModel,"mnt_",4) == 0)	
			{
				if(pData->m_cRace == 0) // 인간
				{
					if(pData->m_cSex == 1)	// 남자
					{
						POS psPos = { 0.0f, 0.0f, 0.0f };
						const float		fDir	= GetDirection();
						
						strcat(strModel, "_M.GCMDS");
						SetModelFileName(strModel);

						CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

						if(pCharModel)
						{
							pCharModel->SetMotionSheet( "NO_WEAPON" );

							pCharModel->SetPosition(vPos);
							pCharModel->SetDirection(fDir);
							pCharModel->SetMotionSheet("MS");
						}
					}
					else	// 여자
					{
						POS psPos = { 0.0f, 0.0f, 0.0f };

						const float		fDir	= GetDirection();
						
						strcat(strModel, "_W.GCMDS");
						SetModelFileName(strModel);

						CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

						if(pCharModel)
						{
							pCharModel->SetMotionSheet( "NO_WEAPON" );

							pCharModel->SetPosition(vPos);
							pCharModel->SetDirection(fDir);
							pCharModel->SetMotionSheet("MS");
						}
					}
				}
				else if(pData->m_cRace == 1) // 아칸
				{
					if(pData->m_cSex == 1)	// 컴배던트
					{
						POS psPos = { 0.0f, 0.0f, 0.0f };
						const float		fDir	= GetDirection();
						
						strcat(strModel, "_C.GCMDS");
						SetModelFileName(strModel);

						CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

						if(pCharModel)
						{
							pCharModel->SetMotionSheet( "NO_WEAPON" );

							pCharModel->SetPosition(vPos);
							pCharModel->SetDirection(fDir);
							pCharModel->SetMotionSheet("MS");
						}
					}
					else	// 오피세이터
					{
						POS psPos = { 0.0f, 0.0f, 0.0f };
						
						const float		fDir	= GetDirection();
						
						strcat(strModel, "_O.GCMDS");
						SetModelFileName(strModel);

						CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

						if(pCharModel)
						{
							pCharModel->SetMotionSheet( "NO_WEAPON" );

							pCharModel->SetPosition(vPos);
							pCharModel->SetDirection(fDir);
							pCharModel->SetMotionSheet("MS");
						}
					}
				}

				m_ucEnableRide = 1;
			}
			else if(strnicmp(strModel,"Rider",5) == 0)	
			{
				if(pData->m_cSex == 1)
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					const float		fDir	= GetDirection();
					
					strcat(strModel, "_M.GCMDS");
					SetModelFileName(strModel);

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

					if(pCharModel)
					{
						pCharModel->SetMotionSheet( "NO_WEAPON" );

						pCharModel->SetPosition(vPos);
						pCharModel->SetDirection(fDir);
						pCharModel->SetMotionSheet("MS");
					}
				}
				else
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					
					const float		fDir	= GetDirection();
					
					strcat(strModel, "_W.GCMDS");
					SetModelFileName(strModel);

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

					if(pCharModel)
					{
						pCharModel->SetMotionSheet( "NO_WEAPON" );

						pCharModel->SetPosition(vPos);
						pCharModel->SetDirection(fDir);
						pCharModel->SetMotionSheet("MS");
					}
					//m_bUpperActing = FALSE;
					//m_bLowerActing = FALSE;
				}
				m_ucEnableRide = 1;
		
			}
			else if(strlen(strModel) != 0)
			{
				POS psPos = { 0.0f, 0.0f, 0.0f };
				const float		fDir	= GetDirection();
				
				strcat(strModel, ".GCMDS");
				SetModelFileName(strModel);

				CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(this, strModel);

				if(pCharModel)
				{
					pCharModel->SetMotionSheet( "NO_WEAPON" );

					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);
					pCharModel->SetMotionSheet("MS");
				}
				
				m_ucEnableRide = 1;
			}
		}
		if(bSwim)
			m_pCharModel->SetMotion("SWIM");
		/*else
			m_pCharModel->SetMotion("WAIT");*/
	}

	if (!m_EquipGrade.GetRightHand() && !bWeapon )
	{		 
		m_pCharModel->SetAttachmentSlot( "WEAPON", NULL );		
	}
	else if(m_EquipGrade.GetRightHand() && !bWeapon)
	{
		m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON_HAND1 + m_ulWeaponPos ], 
			GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );
	}

	if (!m_EquipGrade.GetLeftHand() && !bShield ) 
	{
		m_pCharModel->SetAttachmentSlot( "SHIELD", NULL );		
	}		
	else if(m_EquipGrade.GetRightHand() && !bWeapon)
	{
		m_pCharModel->SetAttachmentSlot( "SHIELD", m_strShape[ SHIELD_HAND1 + m_ulWeaponPos ], 
			GetEquipGrade().GetLeftHand(), GetEquipGrade().GetLeftHand() );
	}

	if ( m_ucRace == 0 )			// Human
	{
		if ( bChange )
		{
			m_pCharModel->BeginOutfitSetting();

			// Rodin : 임시 봉인 (장비 그레이드별 이펙트)
			m_pCharModel->SetOutfitSlot( "HEAD", m_strShape[ HELM ], GetEquipGrade().GetHead(), GetEquipGrade().GetHead() );
//			m_pCharModel->SetOutfitSlot( "HEAD", m_strShape[ HELM ] );

			m_pCharModel->SetOutfitSlot( "BODY", m_strShape[ SHIRT ] );
			m_pCharModel->SetOutfitSlot( "BODY", m_strShape[ TUNIC ] );

			// Rodin : 임시 봉인 (장비 그레이드별 이펙트)
			m_pCharModel->SetOutfitSlot( "BODY", m_strShape[ ARMOUR ], GetEquipGrade().GetBody(), GetEquipGrade().GetBody() );
//			m_pCharModel->SetOutfitSlot( "BODY", m_strShape[ ARMOUR ] );

		//	m_pCharModel->SetOutfitSlot( "HAND", m_strShape[ GLOVE ] );
			m_pCharModel->SetOutfitSlot( "HAND", m_strShape[ GLOVE ], GetEquipGrade().GetBody(), GetEquipGrade().GetBody() ); 
//			m_pCharModel->SetOutfitSlot( "FOOT", m_strShape[ BOOTS ] );
			m_pCharModel->SetOutfitSlot( "FOOT", m_strShape[ BOOTS ], GetEquipGrade().GetBody(), GetEquipGrade().GetBody() ); 

			m_pCharModel->EndOutfitSetting();
		}


		if ( bWeapon )
        {
            m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON_HAND1 + m_ulWeaponPos ], 
				GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );
        }

		if ( bShield ) 
        {
            m_pCharModel->SetAttachmentSlot( "SHIELD", m_strShape[ SHIELD_HAND1 + m_ulWeaponPos ], 
				GetEquipGrade().GetLeftHand(), GetEquipGrade().GetLeftHand() );
        }
	} 
    else if ( m_ucRace == 1 )	// Akhan
	{
		if ( bChange )
		{
			m_pCharModel->BeginOutfitSetting();

			// Rodin : 임시 봉인 (장비 그레이드별 이펙트)
			m_pCharModel->SetOutfitSlot( "HEAD", m_strShape[ HEAD ], GetEquipGrade().GetHead(), GetEquipGrade().GetHead() );
//			m_pCharModel->SetOutfitSlot( "HEAD", m_strShape[ HEAD ] );

			// Rodin : 임시 봉인 (장비 그레이드별 이펙트)
			m_pCharModel->SetOutfitSlot( "BODY", m_strShape[ BODY ], GetEquipGrade().GetBody(), GetEquipGrade().GetBody() );
//			m_pCharModel->SetOutfitSlot( "BODY", m_strShape[ BODY ] );

			m_pCharModel->SetOutfitSlot( "PELVIS", m_strShape[ PELVIS ], GetEquipGrade().GetBody(), GetEquipGrade().GetBody() );
			m_pCharModel->SetOutfitSlot( "ARM", m_strShape[ PROTECT_ARM ], GetEquipGrade().GetBody(), GetEquipGrade().GetBody() );

			m_pCharModel->SetOutfitSlot( "SKILL", m_strShape[ SKILL_ARM ], GetEquipGrade().GetLeftHand(), GetEquipGrade().GetLeftHand() );

			m_pCharModel->EndOutfitSetting();
		}

		if ( bWeapon ) 
        {
            m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON ], GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );
        }
	}

	if ( m_bCharDead ) 
    {
        InitWeapon( 0 ); 
    }
    else 
    {
		if(m_ucEnableRide == 1)
			InitWeapon( 2 );
		else
			InitWeapon( 1 );
    }

//	InitCharAction();
}

Broadcast2nd::CNetworkEquipGrade RYLCreature::GetEquipGrade(void)
{
	// 옵션을 끈 경우
	if ( !(CRYLGameData::Instance()->m_coOption.m_dwEquipEffect & 1) )
	{
		return Broadcast2nd::CNetworkEquipGrade();
	}

	// 스텔스/하이드 상태인 경우
	if (this != RYLCreatureManager::Instance()->GetSelfCreature())
	{
		if (GetEffect()->GetChantStatus(Skill::SpellID::Hide) || 
			GetEffect()->GetChantStatus(Skill::SpellID::Stealth))
		{
			return Broadcast2nd::CNetworkEquipGrade();
		}
	}

	return m_EquipGrade;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::InitCharAction()
void RYLCreature::InitCharAction()
{
    if ( m_bCharDead )
	{
		m_ulUpperCharActionID = CA_FALLDOWN;
		m_ulLowerCharActionID = CA_FALLDOWN;
	}

	if ( m_pCharModel )
	{
		if ( m_ulLowerCharActionID == CA_ATTACK || m_ulLowerCharActionID == CA_ATTACK2 || m_ulLowerCharActionID == CA_ATTACK3 ||
			m_ulLowerCharActionID == CA_ATTACK4 || m_ulLowerCharActionID == CA_ATTACK5 || m_ulLowerCharActionID == CA_ATTACK6)
		{
			;
		} 
        else
		{
			if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
			{
				m_pCharModel->SetAnimationSpeed( 0.6f );
			} 
            else
			{
				m_pCharModel->SetAnimationSpeed( 1.2f );
			}
		}
	}

	if ( m_pCharModel->IsActionFinished( 0 ) && !m_bCharDead  )
	{
		if ( m_ulUpperCharActionID == CA_HIT1 || m_ulUpperCharActionID == CA_WAITH1 )
		{
			m_ulUpperCharActionID = CA_WAITH1;
		} 
        else if ( m_ulUpperCharActionID == CA_HIT2 || m_ulUpperCharActionID == CA_WAITH2 )
		{
			m_ulUpperCharActionID = CA_WAITH2;
		} 
        else if ( m_pCreatureEffect->GetChantStatus(26 ) )
		{
			m_ulUpperCharActionID = CA_STUN;
		} 
        else if ( m_ulUpperCharActionID == CA_SWIM )
		{
			m_ulUpperCharActionID = CA_SWIM;
		} 
        else if ( m_bSitMode )
		{
			m_ulUpperCharActionID = CA_REST;
		} 
        else if ( !m_bStillCasting )
		{
			m_ulUpperCharActionID = CA_WAIT;
		}

		m_bUpperAble    = true;
		m_bUpperActing  = false;
	}

	if ( m_pCharModel->IsActionFinished( 1 ) )
	{
		if ( m_ulLowerCharActionID == CA_FIRE1 || m_ulLowerCharActionID == CA_FIRE2 || m_ulLowerCharActionID == CA_SHOT || m_ulLowerCharActionID == CA_JUMPSHOT )
		{
			InitWeapon( 1 );
		}

		if ( CA_GATHERSTART <= m_ulLowerCharActionID && m_ulLowerCharActionID <= CA_GATHEREND)
		{
			InitWeapon( 1 );
		}
		if ( CA_SOCIALSTART <= m_ulLowerCharActionID && m_ulLowerCharActionID <= CA_SOCIALEND)
		{
			InitWeapon( 1 );
		}
		

		if ( m_bCharDead )
		{
			m_bLowerActing = false;
		} 
        else
		{
			if ( m_ulLowerCharActionID == CA_LEFTDASH || m_ulLowerCharActionID == CA_RIGHTDASH || m_ulLowerCharActionID == CA_BACKDASH )
			{
				CSceneManager::CharacterFirstAccelate( m_pCharModel,false );			
				CSceneManager::AddDelayTimer( m_pCharModel,30 );
			}

			if ( m_ulLowerCharActionID == CA_HIT1 || m_ulLowerCharActionID == CA_WAITH1 )
			{
				m_ulLowerCharActionID = CA_WAITH1;
			} 
            else if ( m_ulLowerCharActionID == CA_HIT2 || m_ulLowerCharActionID == CA_WAITH2 )
			{
				m_ulLowerCharActionID = CA_WAITH2;
			} 
            else if ( m_pCreatureEffect->GetChantStatus(26 ) )
			{
				m_ulLowerCharActionID = CA_STUN;
			} 
            else if ( m_ulLowerCharActionID == CA_SWIM )
			{
				m_ulLowerCharActionID = CA_SWIM;
			} 
            else if ( m_bSitMode )
			{
				m_ulLowerCharActionID = CA_REST;
			} 
            else if ( m_ulLowerCharActionID != CA_FORWARDJUMP && m_ulLowerCharActionID != CA_BACKJUMP &&
					 m_ulLowerCharActionID != CA_JUMP && m_ulLowerCharActionID != CA_FRONTDASH && !m_bStillCasting )
			{
				m_ulLowerCharActionID = CA_WAIT;
			}

			m_bLowerAble    = true;
			m_bLowerActing  = false;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::InitWeapon()
void RYLCreature::InitWeapon( unsigned long ulEquip )
{
	using namespace Item::EquipmentPos;

	CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( m_ulCharID );
    if ( !pData )
        return;

    m_ucRace  = pData->m_cRace;
    m_usClass = pData->m_usClass;

    if ( m_pCharModel )
	{
		switch ( ulEquip )
		{
			case 0:
            {
				if ( m_ucRace == 0 )
				{
					m_pCharModel->SetMotionSheet( "NO_WEAPON" );

					m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON_HAND1 + m_ulWeaponPos ], 
						GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );
					m_pCharModel->SetAttachmentSlot( "SHIELD", m_strShape[ SHIELD_HAND1 + m_ulWeaponPos ], 
						GetEquipGrade().GetLeftHand(), GetEquipGrade().GetLeftHand() );

					m_pCharModel->SetAttachmentHolder( "WEAPON", 0 );
					m_pCharModel->SetAttachmentHolder( "SHIELD", 2 );
				} 
                else if ( m_ucRace == 1 )
				{
					m_pCharModel->SetMotionSheet( "NO_WEAPON" );

					m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON ], 
						GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );

					m_pCharModel->SetAttachmentHolder( "WEAPON", 0 );
				}
            }
			break;

			case 2:
			{
				if ( m_ucRace == 0 )
				{
					m_pCharModel->SetMotionSheet( "NO_WEAPON" );

					m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON_HAND1 + m_ulWeaponPos ], 
						GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );
					m_pCharModel->SetAttachmentSlot( "SHIELD", m_strShape[ SHIELD_HAND1 + m_ulWeaponPos ], 
						GetEquipGrade().GetLeftHand(), GetEquipGrade().GetLeftHand() );

					m_pCharModel->SetAttachmentHolder( "WEAPON", 0 );
					m_pCharModel->SetAttachmentHolder( "SHIELD", 1 );
				} 
				else if ( m_ucRace == 1 )
				{
					m_pCharModel->SetMotionSheet( "NO_WEAPON" );

					m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON ], 
						GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );

					m_pCharModel->SetAttachmentHolder( "WEAPON", 0 );
				}
			}
			break;

            case 1:
            {
                if ( RYLCreatureManager::GetMotionSheet )
                {
                    if ( m_bSitMode )
                    {
                        InitWeapon( 0 );
                    } 
                    else
                    {
                        if ( m_ucRace == 0 )
                        {
							// 모션 시트 알아내기
                            char* strMotionSheet = 
								RYLCreatureManager::GetMotionSheet( 
								const_cast<char*>( m_strShape[ WEAPON_HAND1 + m_ulWeaponPos ] ),                                                                              
								const_cast<char*>( m_strShape[ SHIELD_HAND1 + m_ulWeaponPos ] ), 
								m_usClass );		
                            
                            m_pCharModel->SetMotionSheet( strMotionSheet );

                            if (!strcmp(strMotionSheet, "NO_WEAPON_B"))
                            {
                                m_bNoneComboAttackMode = false;
                            } 
                            else
                            {
                                m_bNoneComboAttackMode = true;
                            }

                            m_pCharModel->SetAttachmentSlot( "WEAPON", m_strShape[ WEAPON_HAND1 + m_ulWeaponPos ], 
								GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );
                            m_pCharModel->SetAttachmentSlot( "SHIELD", m_strShape[ SHIELD_HAND1 + m_ulWeaponPos ], 
								GetEquipGrade().GetLeftHand(), GetEquipGrade().GetLeftHand() );

                            m_pCharModel->SetAttachmentHolder( "WEAPON", 1 );
                            m_pCharModel->SetAttachmentHolder( "SHIELD", 3 );
                        } 
                        else if ( m_ucRace == 1 )
                        {
							// 모션 시트 알아내기
                            char* strMotionSheet = 
								RYLCreatureManager::GetMotionSheet( 
								const_cast<char*>( m_strShape[ WEAPON ] ), 
								const_cast<char*>( m_strShape[ SKILL_ARM ] ), 
								m_usClass );		
                            
                            m_pCharModel->SetMotionSheet( strMotionSheet );

                            if (!strcmp(strMotionSheet, "NO_WEAPON_B"))
                            {
                                m_bNoneComboAttackMode = false;
                            } 
                            else
                            {
                                m_bNoneComboAttackMode = true;
                            }

                            m_pCharModel->SetAttachmentSlot( "WEAPON" , m_strShape[ WEAPON ], 
								GetEquipGrade().GetRightHand(), GetEquipGrade().GetRightHand() );

                            m_pCharModel->SetAttachmentHolder( "WEAPON", 1 );
                        }
                    }
                } 
                else		// 클라이언트 외 처리
                {
                    m_pCharModel->SetMotionSheet( "WEAPON_ONE_HAND" );
                    m_pCharModel->SetAttachmentHolder( "WEAPON", 1 );
                    m_bNoneComboAttackMode = true;
                }
            }
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::IsCancelAction()
bool RYLCreature::IsCancelAction( unsigned long ulPart )
{
    switch ( ulPart )
	{
		case 1:
            {
			    if ( m_ulUpperCharActionID == CA_RUN || m_ulUpperCharActionID == CA_WALK ||
				    m_ulUpperCharActionID == CA_WALKLEFT || m_ulUpperCharActionID == CA_WALKRIGHT ||
				    m_ulUpperCharActionID == CA_WALKBACK || m_ulUpperCharActionID == CA_WAIT ||
				    m_ulUpperCharActionID == CA_WAITH1 || m_ulUpperCharActionID == CA_WAITH2 ||
				    m_ulUpperCharActionID == CA_REST || m_ulUpperCharActionID == CA_SWIM )
				    return true;
            }
			break;

		case 2:
            {
			    if ( m_ulLowerCharActionID == CA_RUN || m_ulLowerCharActionID == CA_WALK ||
				    m_ulLowerCharActionID == CA_WALKLEFT || m_ulLowerCharActionID == CA_WALKRIGHT ||
				    m_ulLowerCharActionID == CA_WALKBACK || m_ulLowerCharActionID == CA_WAIT ||
				    m_ulLowerCharActionID == CA_WAITH1 || m_ulLowerCharActionID == CA_WAITH2 ||
				    m_ulLowerCharActionID == CA_REST || m_ulLowerCharActionID == CA_SWIM )
				    return true;
            }
			break;

		case 3:
            {
			    if ( ( m_ulUpperCharActionID == CA_RUN || m_ulUpperCharActionID == CA_WALK ||
				      m_ulUpperCharActionID == CA_WALKLEFT || m_ulUpperCharActionID == CA_WALKRIGHT ||
				      m_ulUpperCharActionID == CA_WALKBACK || m_ulUpperCharActionID == CA_WAIT ||
				      m_ulUpperCharActionID == CA_WAITH1 || m_ulUpperCharActionID == CA_WAITH2 ||
				      m_ulUpperCharActionID == CA_REST || m_ulUpperCharActionID == CA_SWIM || 
                      m_ulUpperCharActionID == CA_JUMP ) && 
			          ( m_ulLowerCharActionID == CA_RUN || m_ulLowerCharActionID == CA_WALK ||
				      m_ulLowerCharActionID == CA_WALKLEFT || m_ulLowerCharActionID == CA_WALKRIGHT ||
				      m_ulLowerCharActionID == CA_WALKBACK || m_ulLowerCharActionID == CA_WAIT ||
				      m_ulLowerCharActionID == CA_WAITH1 || m_ulLowerCharActionID == CA_WAITH2 ||
				      m_ulLowerCharActionID == CA_REST || m_ulLowerCharActionID == CA_SWIM ) )
				      return true;
            }
			break;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Respawn()
void RYLCreature::Respawn()
{
    InitCreature( true );

    m_ulUpperCharActionID = CA_WAIT;
    m_ulLowerCharActionID = CA_WAIT;
    m_bUpperActing  = false;
    m_bLowerActing  = false;
    m_bUpperAble    = true;
    m_bLowerAble    = true;

    SetAction( CA_WAIT, CA_WAIT );
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Landing()
bool RYLCreature::Landing( CCharacterControl* lpObjControl )
{
	bool bLanding = true;

	if (true == GetFlying())
	{
		CZ3DGeneralChrModel* pSelfModel = GetCharModel();

		CollisionType   CT = CSceneManager::GetCharacterCollisionType(pSelfModel);	
		int				nJump = CSceneManager::GetCharacterEvent(pSelfModel, EVENT_JUMP);

		// 내 점프만 가능
		if (nJump && GetFlyFirstCheck() == false)		
		{
			SetFlyFirstCheck(true);
		}

		if (CT == CT_NONE)
		{
			SetUpperAble(false);
			SetLowerAble(false);

			SetUpperActing(true);
			SetLowerActing(true);

			bLanding = false;
		} 
		else if (CT == CT_TERRAIN || CT == CT_BOTTOM || CT == CT_WATER)
		{
			if (true == GetFlyFirstCheck())
			{
				// 물이 아닐 때 점프 착지 사운드 출력
				if (CT != CT_WATER) 
				{
					lpObjControl->PlaySound(200, *(GetPosition())) ;
				}

				SetFlying(false);
				SetFlyFirstCheck(false);	

				SetUpperActing(false);
				SetLowerActing(false);

				SetJumpAttackVector(vector3(0.0f, 0.0f, 0.0f));
				CCameraControl::m_fJumpRate = 0.01f;

				CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData(GetCharID());
				if (GetLowerActionID() != CA_JUMPATTACK2)
				{
					// edith 점프
					// 연속 점프가 아닌 경우 착지 처리
					if ((pSelfData->m_usClass != CClass::Rogue && 
						pSelfData->m_usClass != CClass::Assassin && 
						pSelfData->m_usClass != CClass::ShadowOff) || !KeyPressed(VK_SPACE))		
					{
						SetAction(CA_LAND, CA_LAND);
						m_ulSkillComboCount = 0;
						lpObjControl->WrapSendMovingPacket(CA_LAND, CA_LAND, GetDirection(), *(GetPosition()));
					}
				} 
				else if (pSelfData->m_usClass == CClass::Attacker)
				{
					SetAction(CA_JUMPATTACK3, CA_JUMPATTACK3);
					lpObjControl->WrapSendMovingPacket(CA_JUMPATTACK3, CA_JUMPATTACK3, GetDirection(), *(GetPosition()));
				}
			}
		}
	}

	return bLanding;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Walk()
void RYLCreature::Walk( unsigned char ucDirection, float fRunFactor )
{
    // if sitting down, stand up and do nothing.
    if ( m_bLowerAble )
    {
        if ( m_bSitMode )
        {
            ToggleCrouch();

            return;
        }
    }

    switch ( ucDirection )
    {
    case MOVE_FOWARD:
        {
            if ( m_fRunFactor < 155.0f ) 
                m_fRunFactor += fRunFactor;

            SetAction( CA_RUN, CA_RUN );
        }
        break;

    case MOVE_RIGHT:
        {
            m_fRunFactor = 0.0f;
            SetAction( CA_WALKRIGHT, CA_WALKRIGHT );
        }
        break;

    case MOVE_BACK:
        {
            m_fRunFactor = 0.0f;
            SetAction( CA_WALKBACK, CA_WALKBACK );
        }
        break;

    case MOVE_LEFT:
        {
            m_fRunFactor = 0.0f;
            SetAction( CA_WALKLEFT, CA_WALKLEFT );
        }
        break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Dead()
void RYLCreature::Dead()
{
	if ( Siege::EMBLEM == m_wObjectType || Siege::CASTLE_ARMS_NPC == m_wObjectType || SHORT_RANGE_CASTLE_ARMS == m_wObjectType)
	{
		return ;
	}

	// edith 2009.05.05 죽었을때 정보초기화
	SetFlying(false);
	SetFlyFirstCheck(false);	

	SetUpperActing(false);
	SetLowerActing(false);

	SetJumpAttackVector(vector3(0.0f, 0.0f, 0.0f));
	CCameraControl::m_fJumpRate = 0.01f;

	SetAction(CA_LAND, CA_LAND);
	m_ulSkillComboCount = 0;

    if ( !m_bCharDead )
	{
  		m_fRunFactor    = 0.0f;

		m_bSitMode      = false;
		m_fDeadTimer    = 1.0f;

		m_bCharDead     = true;

		m_bUpperAble    = true;
		m_bLowerAble    = true;
		m_bUpperActing  = false;
		m_bLowerActing  = false;

        m_ulAttackCombo     = 0;
		m_ulSkillComboCount = 0;
		m_bSkillCombo       = false;

		if ( m_pCharModel )
		{
			m_pCharModel->SetAnimationSpeed( 1.0f );

			InitWeapon( 0 );

			CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f  ));
			CSceneManager::CharacterFirstAccelate( m_pCharModel, false );

			SetAction( CA_FALLDOWN, CA_FALLDOWN );
		} 
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Wait()
void RYLCreature::Wait()
{
    if ( m_bUpperAble || m_bLowerAble )
	{
		m_fRunFactor = 0.0f;
/*
		// edith 2009.07.29 점프 스킬 시전 버그 수정부분
		if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Stun ) ||
			m_pCreatureEffect->GetChantStatus( Skill::SpellID::Hold ) )
			SetFlying(false);
*/
		if ( m_bSitMode )
		{
			if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Stun ) )
			{
				SetAction( CA_STUN, CA_STUN );

			} else
			{
				SetAction( CA_REST, CA_REST );
			}
		} 
        else if ( m_ulUpperCharActionID == CA_WAITH1 )
		{
			SetAction( CA_WAITH1, CA_WAITH1 );
		} 
        else if ( m_ulUpperCharActionID == CA_WAITH2 )
		{
			SetAction( CA_WAITH2, CA_WAITH2 );
		} 
        else if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Stun ) )
		{
			SetAction( CA_STUN, CA_STUN );
		} 
        else
		{
			SetAction( CA_WAIT, CA_WAIT );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::ToggleCrouch()
void RYLCreature::ToggleCrouch()
{
    if ( m_bUpperAble && m_bLowerAble && m_pCharModel )
	{
		m_fRunFactor = 0.0f;

		if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Slow ) )
		{
			m_pCharModel->SetAnimationSpeed( 1.0f );
		} 
        else if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
		{
			m_pCharModel->SetAnimationSpeed( 0.5f );
    		} 
        else
		{
			m_pCharModel->SetAnimationSpeed( 1.0f );
		}

		CSceneManager::CharacterMovementVector( m_pCharModel, vector3( 0.0f, 0.0f, 0.0f ) );

		if ( m_bSitMode )
		{
			InitWeapon( 0 );
			SetAction( CA_STANDUP, CA_STANDUP );
			m_bSitMode = false;
		} 
        else
		{
			InitWeapon( 0 );
			SetAction( CA_SITDOWN, CA_SITDOWN );
			m_bSitMode = true;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Dash()
bool RYLCreature::Dash( unsigned char ucDirection )
{
    if ( m_bUpperAble && !m_bSitMode )
	{
        bool bResult = true;

		switch ( ucDirection )
		{
			case MOVE_LEFT:
                {
				    bResult = SetAction( CA_LEFTDASH, CA_LEFTDASH );
                }
				break;

			case MOVE_RIGHT:
                {
				    bResult = SetAction( CA_RIGHTDASH, CA_RIGHTDASH );
                }
				break;

			case MOVE_BACK:
                {
				    bResult = SetAction( CA_BACKDASH, CA_BACKDASH );
                }
				break;
		}		
		m_bUpperAble = true;

		return bResult;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Jump()
bool RYLCreature::Jump()
{
    bool bResult = false;

	if(GetGMModelFlag())
		return bResult;

	if ( m_bUpperAble && m_bLowerAble && !m_bSitMode )
	{
		if ( m_ulLowerCharActionID != CA_WAIT )
		{
			const char *strMotionSheet = m_pCharModel->GetCurrentMotionSheetName();

			if ( strMotionSheet &&
				( !strcmp( strMotionSheet, "CLASS_ROGUE" ) || !strcmp( strMotionSheet, "WEAPON_SHADOW_OFF" ) ||
				  !strcmp( strMotionSheet, "WEAPON_2CLAW" ) || !strcmp( strMotionSheet, "WEAPON_2DA" ) ) )
			{
				if ( KeyPressed( 'W' ) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_FORWARD))
                {
					bResult = SetAction( CA_FRONTDASH, CA_FRONTDASH );
                }
				else if ( KeyPressed( 'S' ) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_BACKWARD))
				{
                    //
				}
				else
                {
					SetAction( CA_JUMP, CA_JUMP );
                }
			} 
            else
			{
				if ( KeyPressed( 'W' ) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_FORWARD))
                {
					bResult = SetAction( CA_FORWARDJUMP, CA_FORWARDJUMP );
                }
				else if ( KeyPressed( 'S' ) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_BACKWARD))
                {
					bResult = SetAction( CA_BACKJUMP, CA_BACKJUMP );
                }
				else
                {
					SetAction( CA_JUMP, CA_JUMP );
                }
			}

			m_bLowerAble = false; 
			m_bLowerAble = false;
		}
		else
		{
			SetAction( CA_JUMP, CA_JUMP );
			m_vecJumpAttackVector = vector3( 0.0f, 0.0f, 0.0f );
		}

		// edith 2009.07.29 점프 스킬 시전 버그 수정부분
		if ( !m_pCreatureEffect->GetChantStatus( Skill::SpellID::Stun ) &&
			!m_pCreatureEffect->GetChantStatus( Skill::SpellID::Hold ) )
			SetFlying(true);

		//-- Jump sound --//
		string strSoundFileName = SOUNDFILEPATH;
        strSoundFileName += "jump.wav";

//		try
//		{
            CSound3D* pSound3D; 
            if (  CSoundMgr::_GetInstance()->GetSound3D( pSound3D, strSoundFileName.c_str(), 1 ) )
			{
				pSound3D->Play(); 
				D3DVECTOR Pos; 
				Pos.x = m_vecPosition.x; 
				Pos.y = m_vecPosition.y; 
				Pos.z = m_vecPosition.z; 
				pSound3D->SetPosition( Pos );
				pSound3D->SetMinDistance( 1000.0f ); 
				pSound3D->SetMaxDistance( 2000.0f ); 
			}
/*		}
		catch( exception &e )
		{
			static bool bFirstShowMessage = false;

			if ( bFirstShowMessage == false )
			{
				MessageBox( NULL,e.what(),0,0 );
				bFirstShowMessage = true;
			}
		}
*/
		/////////////
		return bResult;
	}

	return bResult;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::BeginCast()
void RYLCreature::BeginCast( const char* pstrMotion )
{
    m_bUpperAble    = true;
	m_bLowerAble    = true;
	m_bUpperActing  = false;
	m_bLowerActing  = false;

    string strMotion = pstrMotion;

	unsigned long ulWeapon = RYLCreatureManager::GetWeaponType( const_cast<char*>( m_strShape[ Item::EquipmentPos::WEAPON_HAND1 + m_ulWeaponPos ] ) );

    if ( ulWeapon != 14 && strMotion.compare( "AIMED_SHOT1" ) )
	{
		InitWeapon( 0 );
	}
    else
	{
		if ( m_ucRace == 0 )
		{
			InitWeapon( 1 );
		}
        else if ( m_ucRace == 1 )
		{
			InitWeapon( 0 );
		}
	}

	SetAction( pstrMotion, pstrMotion );
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Casting()
bool RYLCreature::Casting( unsigned long ulUpperAct, unsigned long ulLowerAct )
{
    unsigned long ulWeapon = RYLCreatureManager::GetWeaponType( const_cast<char*>( m_strShape[ Item::EquipmentPos::WEAPON_HAND1 + m_ulWeaponPos ] ) );

	m_bStillCasting = true;

	if ( m_bUpperAble )
	{
		if ( ulWeapon != 14 &&				// STAFF
			( ( ulLowerAct != CA_AIMEDSHOT1 && ulLowerAct != CA_AIMEDSHOT2 ) && 
			( m_ulLowerCharActionID != CA_AIMEDSHOT1 && m_ulLowerCharActionID != CA_AIMEDSHOT2 ) ) )
		{
			InitWeapon( 0 );
		} else
		{
			if ( m_ucRace == 0 )
			{
				InitWeapon( 1 );
			} 
            else if ( m_ucRace == 1 )
			{
				InitWeapon( 0 );
			}
		}

		m_fRunFactor = 0.0f;

		if ( m_bSitMode ) 
        { 
            ToggleCrouch(); 
            return true; 
        }

		unsigned long ulUpperAct2, ulLowerAct2;

		if ( ulUpperAct == 0xFFFFFFFF && ulLowerAct == 0xFFFFFFFF )
		{
			if ( m_ulLowerCharActionID == CA_AIMEDSHOT1 )
			{
				ulUpperAct2 = CA_AIMEDSHOT2;
				ulLowerAct2 = CA_AIMEDSHOT2;
			} 
            else
			{
				ulUpperAct2 = m_ulUpperCharActionID;
				ulLowerAct2 = m_ulLowerCharActionID;
			}
		} 
        else
		{
			ulUpperAct2 = ulUpperAct;
			ulLowerAct2 = ulLowerAct;
		}

		return SetAction( ulUpperAct2, ulLowerAct2 );
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::CancelSkill()
void RYLCreature::CancelSkill()
{
    if ( m_ulUpperCharActionID == CA_CASTING || m_ulUpperCharActionID == CA_AIMEDSHOT1 || m_ulUpperCharActionID == CA_AIMEDSHOT2 )
	{
		m_bUpperAble            = true;
		m_bLowerAble            = true;
		m_bUpperActing          = false;
		m_bLowerActing          = false;
		m_ulUpperCharActionID   = CA_WAIT;
		m_ulLowerCharActionID   = CA_WAIT;

		Wait();
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::EndSkill()
void RYLCreature::EndSkill( const char* pstrMotion )
{
    string strMotion = pstrMotion;

    m_bUpperAble    = true;
	m_bLowerAble    = true;
	m_bUpperActing  = false;
	m_bLowerActing  = false;
    m_bStillCasting = false;

    if ( strMotion.compare( "FIRE1" ) && strMotion.compare( "FIRE2" ) && strMotion.compare( "SHOT" ) && strMotion.compare( "JUMP_SHOT" ) )
	{
		InitWeapon( 1 );
	}

	if ( strncmp( pstrMotion, "SOCIAL", 6) == 0 || strncmp( pstrMotion, "GATHER", 6) == 0 )
	{
		/// 소셜모션 테스트
		InitWeapon( 0 );
	}

	SetAction( pstrMotion, pstrMotion );
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SkillAttack()
void RYLCreature::SkillAttack( unsigned long ulUpperAct, unsigned long ulLowerAct )
{
    if ( m_bUpperAble && m_bLowerAble )
	{
		SetAction( ulUpperAct, ulLowerAct );
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::Attack()
bool RYLCreature::Attack( unsigned long ulUpperAct, unsigned long ulLowerAct )
{
    if ( m_bFlying )
	{
		InitWeapon( 1 );

		if ( ( m_ulLowerCharActionID != CA_JUMPATTACK && m_ulLowerCharActionID != CA_JUMPATTACK2 ) && m_bFlyFirstCheck )
		{
			if ( m_usClass == 6 || m_usClass == 2 || m_usClass == 7 || m_usClass == 20 || m_usClass == 24 )				// 점프 공격
			{
				m_bUpperAble    = true;
				m_bLowerAble    = true;
				m_bUpperActing  = false;
				m_bLowerActing  = false;

				if ( KeyPressed( 'S' ) && ( m_usClass == 2 || m_usClass == 7 || m_usClass == 20 ) )		// 내려 찍기
				{
					SetAction( CA_JUMPATTACK2, CA_JUMPATTACK2 );
				} else
				{
					SetAction( CA_JUMPATTACK, CA_JUMPATTACK );
				}

				return true;
			}
		}
	}

	if ( m_bUpperAble )
	{
		InitWeapon( 1 );

		m_fRunFactor = 0.0f;

		if ( m_bSitMode ) 
        { 
            ToggleCrouch(); 
            return true; 
        }

		unsigned long ulUpperAct2, ulLowerAct2;

		if ( ulUpperAct == 0xFFFFFFFF && ulLowerAct == 0xFFFFFFFF )
		{
			switch ( m_ulLowerCharActionID )
			{
				case CA_WALK:
				case CA_RUN:
                    {
					    ulUpperAct2 = CA_ATTACKADVANCE;
					    ulLowerAct2 = m_ulLowerCharActionID;
                    }
					break;

				case CA_WALKLEFT:
                    {
					    ulUpperAct2 = CA_ATTACKLEFT;
					    ulLowerAct2 = m_ulLowerCharActionID;
                    }
					break;

				case CA_WALKRIGHT:
                    {
					    ulUpperAct2 = CA_ATTACKRIGHT;
					    ulLowerAct2 = m_ulLowerCharActionID;
                    }
					break;

				case CA_WALKBACK:
                    {
					    ulUpperAct2 = CA_ATTACKRETREAT;
					    ulLowerAct2 = m_ulLowerCharActionID;
                    }
					break;

				case CA_LEFTDASH:
                    {
					    if ( m_usClass == 1 || m_usClass == 5 || m_usClass == 6 || m_usClass == 19 || m_usClass == 20 )		// 대쉬 공격
					    {
						    if ( m_ulUpperCharActionID == CA_LEFTDASH ) m_ulUpperCharActionID = CA_WAIT;
						    ulUpperAct2 = CA_ATTACKLEFT;
						    ulLowerAct2 = m_ulLowerCharActionID;
					    } 
                        else
					    {
						    return false;
					    }
                    }
					break;

				case CA_RIGHTDASH:
                    {
					    if ( m_usClass == 1 || m_usClass == 5 || m_usClass == 6 || m_usClass == 19 || m_usClass == 20 )		// 대쉬 공격
					    {
						    if ( m_ulUpperCharActionID == CA_RIGHTDASH ) 
                                m_ulUpperCharActionID = CA_WAIT;

						    ulUpperAct2 = CA_ATTACKRIGHT;
						    ulLowerAct2 = m_ulLowerCharActionID;
					    } 
                        else
					    {
						    return false;
					    }
                    }
					break;

				default:
                    {
					    switch ( m_ulAttackCombo )
					    {
						    case 0:
                                {
                                    ulUpperAct2 = CA_ATTACK;
                                    ulLowerAct2 = CA_ATTACK;
                                }
                                break;

                            case 1:
                                {
                                    ulUpperAct2 = CA_ATTACK2;
                                    ulLowerAct2 = CA_ATTACK2;
                                }
                                break;

                            case 2:
                                {
                                    ulUpperAct2 = CA_ATTACK3;
                                    ulLowerAct2 = CA_ATTACK3;
                                }
                                break;

                            case 3:
                                {
                                    ulUpperAct2 = CA_ATTACK4;
                                    ulLowerAct2 = CA_ATTACK4;
                                }
                                break;

                            case 4:
                                {
                                    ulUpperAct2 = CA_ATTACK5;
                                    ulLowerAct2 = CA_ATTACK5;
                                }
                                break;

                            case 5:
                                {
                                    ulUpperAct2 = CA_ATTACK6;
                                    ulLowerAct2 = CA_ATTACK6;
                                }
							    break;
					    }
                    }
					break;
			}
		} 
        else
		{
			ulUpperAct2 = ulUpperAct;
			ulLowerAct2 = ulLowerAct;
		}

		if ( SetAction( ulUpperAct2, ulLowerAct2 ) )
		{
			if ( m_strShape[Item::EquipmentPos::WEAPON_HAND1 + m_ulWeaponPos] )
			{
				if ( m_pCharModel )
				{
					if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
					{
						m_pCharModel->SetAnimationSpeed( 0.6f );
					} 
                    else
					{
						m_pCharModel->SetAnimationSpeed( 1.2f );
					}
				}

				switch ( RYLCreatureManager::GetWeaponType( const_cast<char *>(m_strShape[Item::EquipmentPos::WEAPON_HAND1 + m_ulWeaponPos]) ) )
				{
					case 6:	// 검
					case 7:	// 둔기
					case 8: // 도끼
                        {
						    if ( m_pCharModel &&
							    ( m_usClass == 4 || m_usClass == 11 ||						// 성직자( 클레릭 제외 )
							    m_usClass == 3 || m_usClass == 9 || m_usClass == 10 ) )		// 메이지
						    {
							    if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
							    {
								    m_pCharModel->SetAnimationSpeed( 0.6f );	
							    } 
                                else
							    {
								    m_pCharModel->SetAnimationSpeed( 1.2f );	
							    }
						    }
                        }
						break;

					case 9:		// 양손 둔기
					case 10:	// 양손 도끼
					case 11:	// 양손 검
                        {
						    if ( m_pCharModel &&
							    ( m_usClass == 2 || m_usClass == 7 || m_usClass == 8 ||		// 로그
							    m_usClass == 4 || m_usClass == 11 || m_usClass == 12 ||		// 성직자
							    m_usClass == 3 || m_usClass == 9 || m_usClass == 10 ) )		// 메이지
						    {
							    if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
							    {
								    m_pCharModel->SetAnimationSpeed( 0.6f );		
							    } 
                                else
							    {
								    m_pCharModel->SetAnimationSpeed( 1.2f );		
							    }
						    }
                        }
						break;

					case 12:
					case 13:
                        {
						    if ( m_pCharModel )
						    {
							    if ( m_usClass == 10 )				// 인챈터
							    {
								    if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
								    {
									    m_pCharModel->SetAnimationSpeed( 0.6f );
								    } else
								    {
									    m_pCharModel->SetAnimationSpeed( 1.2f );
								    }
							    } 
                                else if ( m_usClass == 8 )			// 아쳐
							    {
								    if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Frozen ) )
								    {
									    m_pCharModel->SetAnimationSpeed( 0.6f );
								    } else
								    {
									    m_pCharModel->SetAnimationSpeed( 1.2f );
								    }
							    }
						    }
                        }
						break;
				}
			}

			return true;
		}
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::PlaySlide()
void RYLCreature::PlaySlide()
{
    if ( m_pCreatureEffect->GetChantStatus( Skill::SpellID::Hold ) ) // hold시 slide 작동 x 
    { 
		m_bSlide = false;
		return;
	}

	if ( m_bSlide ) 
    {
		float fCharDir = GetDirection();

		fCharDir = -fCharDir;
		fCharDir -= FLOAT_PHI / 2.0f;

		vector3 vecCharToward;
		
		vecCharToward.x = cosf( fCharDir );
		vecCharToward.z = sinf( fCharDir );	
		vecCharToward.y = 0.0f;
		vecCharToward.Normalize();

		vector3     vecTemp;
		vector3*    vecCharPos;
		float       fTemplength;

		if ( m_effSlideValue.m_CurrentSlideLength < m_effSlideValue.m_SlideLength )
		{
			vecTemp  = vecCharToward * ( m_effSlideValue.m_SlideSpeed );
			vecTemp *= ( -1.0f );
			m_effSlideValue.m_SlideSpeed += m_effSlideValue.m_SlideAccel;
			
			if ( m_effSlideValue.m_SlideAccel <= 0.0f )
			{
				if ( m_effSlideValue.m_SlideSpeed < m_effSlideValue.m_LimitSpeed )
					m_effSlideValue.m_SlideSpeed = m_effSlideValue.m_LimitSpeed;
			}
			else
			{
				if ( m_effSlideValue.m_SlideSpeed > m_effSlideValue.m_LimitSpeed )
					m_effSlideValue.m_SlideSpeed = m_effSlideValue.m_LimitSpeed;
			}
		}
		else
		{
			vecTemp.x = 0.0f;
			vecTemp.y = 0.0f;
			vecTemp.z = 0.0f;
			m_bSlide = false;
		}

		fTemplength = vecTemp.GetLens();

		m_effSlideValue.m_CurrentSlideLength += fTemplength;

		vector3 vecBeforePos;

		vecCharPos = GetPosition();	
		
		vecBeforePos.x = vecCharPos->x;
		vecBeforePos.y = vecCharPos->y;
		vecBeforePos.z = vecCharPos->z;

		SetPosition( vector3( vecCharPos->x + vecTemp.x, vecCharPos->y, vecCharPos->z + vecTemp.z ) );

		// slide 먼지 render
		if ( m_bSlide && ( !m_bFlying ) )
			RenderSlide( vecBeforePos.x, vecBeforePos.y, vecBeforePos.z, vecCharPos->x, vecCharPos->y, vecCharPos->z,30.0f );

	}	
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::RenderSlide()
void RYLCreature::RenderSlide( float fBx, float fBy, float fBz, float fNx, float fNy, float fNz, float fUnitSize )
{
    //unitsize : 이팩트 사이 간격
	float fVerLength = 0.0f;
	int nUnitCount = 0;

	D3DXVECTOR3 D3DVecTempvec;
	D3DXVECTOR3 D3DVecTempvec2( 0.0f,0.0f,0.0f );

	D3DXVECTOR3 D3DVecRFoot;
	D3DXVECTOR3 D3DVecLFoot;

	D3DVecTempvec.x = fNx - fBx;
	D3DVecTempvec.y = fNy - fBy;
	D3DVecTempvec.z = fNz - fBz;
	
	fVerLength = D3DXVec3Length( &D3DVecTempvec );

	D3DXVec3Normalize( &D3DVecTempvec,&D3DVecTempvec );
	
	CZ3DObject* snappoint = NULL;
	float       fBodyScale = 0.0f;

	if ( m_pCharModel == NULL ) // model 이 NULL 일때 skip
		return;

	//rfoot				
	snappoint = m_pCharModel->GetSkeletonPartObject( "D3DVecRFoot" );
	fBodyScale = m_pCharModel->GetScale();
	
	if ( snappoint != NULL ) 
    {
		vector3 snaptm;
		snappoint->GetPosition( snaptm );
		 
		D3DVecRFoot.x = snaptm.x;
		D3DVecRFoot.y = snaptm.y;
		D3DVecRFoot.z = snaptm.z;

		D3DVecRFoot *= fBodyScale;
	}

	//lfoot
	snappoint = m_pCharModel->GetSkeletonPartObject( "D3DVecLFoot" );
	fBodyScale = m_pCharModel->GetScale();
	
	if ( snappoint != NULL ) 
    {
		vector3 snaptm;
		snappoint->GetPosition( snaptm );
		 
		D3DVecLFoot.x = snaptm.x;
		D3DVecLFoot.y = snaptm.y;
		D3DVecLFoot.z = snaptm.z;

		D3DVecLFoot *= fBodyScale;
	}
			
	while( fVerLength > D3DXVec3Length( &D3DVecTempvec2 ) ) 
    { 
		// 이번에 미끌린 거리와 그 이전의 위치 사이에 effect 출력  
		D3DXVECTOR3 effectpos( fNx,fNy,fNz );
		
		D3DVecTempvec2 += D3DVecTempvec * fUnitSize;
		//rfoot
		CX3DEffect *pEffect = NULL;
		pEffect = new CX3DEffect;
		CSceneManager::AddEffect( pEffect );
		pEffect->Load( EFFECTPATH,"smoke.eff" );
		pEffect->SetLoop( FALSE );				
		pEffect->SetAxis( 0.0f,0.0f,0.0f );
		pEffect->SetCenter( effectpos.x - D3DVecTempvec2.x + D3DVecRFoot.x,
						    effectpos.y - D3DVecTempvec2.y + D3DVecRFoot.y + 20.0f,
						    effectpos.z - D3DVecTempvec2.z + D3DVecRFoot.z );
		pEffect->SetFrame( static_cast<float>( nUnitCount * 2.0f ) );
		
		//lfoot
		CX3DEffect *pEffect2 = NULL;
		pEffect2 = new CX3DEffect;
		CSceneManager::AddEffect( pEffect2 );
		pEffect2->Load( EFFECTPATH,"smoke.eff" );
		pEffect2->SetLoop( FALSE );				
		pEffect2->SetAxis( 0.0f,0.0f,0.0f );
		pEffect2->SetCenter( effectpos.x - D3DVecTempvec2.x + D3DVecLFoot.x,
						     effectpos.y - D3DVecTempvec2.y + D3DVecLFoot.y + 20.0f,
						     effectpos.z - D3DVecTempvec2.z + D3DVecLFoot.z );
		pEffect2->SetFrame( static_cast<float>( nUnitCount * 2.0f ) );
		
		nUnitCount++;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::IsInSafetyZone()
bool RYLCreature::IsInSafetyZone()
{
    return CBgmManager::_GetInstance()->IsSafetyZone( D3DXVECTOR3( m_vecPosition.x, m_vecPosition.y, m_vecPosition.z ) );
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::ClearCreature()
void RYLCreature::ClearCreature()
{
    m_strGCMDSName.clear();
    m_strFaceType.clear();
    m_strHairType.clear();

	std::fill_n(m_strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

    while( !m_queActionData.empty() )
    {
        CharActionNode* pTempAction = m_queActionData.front();

        delete pTempAction;
        pTempAction = NULL;

        m_queActionData.pop();
    }

    if ( m_pCreatureEffect )
    {
        delete m_pCreatureEffect;
        m_pCreatureEffect = NULL;
    }

    if ( m_pDamageEffect )
    {
        delete m_pDamageEffect;
        m_pDamageEffect = NULL;
    }

	if ( m_pCharStreetStallNameLabel )
	{
		delete m_pCharStreetStallNameLabel ;
		m_pCharStreetStallNameLabel = NULL ;
	}

	if(m_pCharTempGuildNameLabel)
	{
		delete m_pCharTempGuildNameLabel;
		m_pCharTempGuildNameLabel = NULL;
	}

	if ( m_pCharNameLabel )
	{
		delete m_pCharNameLabel ;
		m_pCharNameLabel = NULL ;
	}

	if ( m_pCharJobNameLabel ) 
	{
		delete m_pCharJobNameLabel ;
		m_pCharJobNameLabel = NULL ;
	}

	if ( m_pCharChatMsgLabel )
	{
		delete [] m_pCharChatMsgLabel ;
		m_pCharChatMsgLabel = NULL ;
	}

	if ( m_pSignpostImg )
	{
		delete m_pSignpostImg ;
		m_pSignpostImg = NULL ;
	}

	if ( m_pZ3DOBB )
	{
		delete m_pZ3DOBB ;
		m_pZ3DOBB = NULL ;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::AttackWeaponRun()
float RYLCreature::AttackWeaponRun( float fRunFactor )
{
    if ( m_bLowerAble )
	{
		//if ( m_bSitMode ) { CrouchAndStandup(); return; }

		//if ( m_fRunFactor < 155.0f ) m_fRunFactor += fRunFactor;

		if ( m_fRunFactor < 155.0f )
			m_fRunFactor = fRunFactor;

		SetAction( CA_RUN, CA_RUN );

		return m_fRunFactor;
	}

	return 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetIsCastleObject()
bool RYLCreature::GetIsCastleObject()
{
	if ( m_wObjectType == Siege::EMBLEM ||					
		 m_wObjectType == Siege::GATE ||					
		 m_wObjectType == Siege::BACKDOOR ||
		 m_wObjectType == Siege::CASTLE_ARMS_NPC ||
		 m_wObjectType == Siege::GUARD ||					
		 m_wObjectType == Siege::SHORT_RANGE_CASTLE_ARMS ||	
		 m_wObjectType == Siege::LONG_RANGE_CASTLE_ARMS )
	{
		return true ;
	}

	return false ;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetIsCastleNPC()
bool RYLCreature::GetIsCastleNPC()
{
	if ( m_wObjectType == Siege::BACKDOOR || m_wObjectType == Siege::CASTLE_ARMS_NPC )
	{
		return true ;
	}

	return false ;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetIsSiegeArms()
bool RYLCreature::GetIsSiegeArms()
{
	if ( m_wObjectType == Siege::SHORT_RANGE_SIEGE_ARMS ||
		 m_wObjectType == Siege::LONG_RANGE_SIEGE_ARMS ||
		 m_wObjectType == Siege::AIRSHIP )
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::AddRiderID()
void RYLCreature::AddRiderID( DWORD dwRiderID )
{
	// 이미 타고 있는지 검사
	for (int i=0; i<Siege::AIRSHIP_RIDER_NUM; ++i)
	{
		if ( m_dwRiderID[i] == dwRiderID )
		{
			return ;
		}
	}

	// 타고 있지 않다면, 추가 (주인이라면 0번 인덱스에 들어가야한다.)
	switch ( m_wObjectType )
	{
		case Siege::SHORT_RANGE_SIEGE_ARMS:
		case Siege::LONG_RANGE_SIEGE_ARMS:
		{
			if ( 0 == m_dwRiderID[0] && m_dwOwnerID == dwRiderID )
			{
				m_dwRiderID[0] = dwRiderID ;
			}
			break;
		}

		case Siege::SHORT_RANGE_CASTLE_ARMS:
		case Siege::LONG_RANGE_CASTLE_ARMS:
		{
			if ( 0 == m_dwRiderID[0] )
			{
				m_dwRiderID[0] = dwRiderID ;
			}
			break;
		}

		case Siege::AIRSHIP:
		{
			if ( m_dwOwnerID == dwRiderID )
			{
				m_dwRiderID[0] = dwRiderID ;
			}
			else
			{
				for (int i=1; i<Siege::AIRSHIP_RIDER_NUM; ++i)
				{
					if ( 0 == m_dwRiderID[i] )
					{
						m_dwRiderID[i] = dwRiderID ;
						return ;
					}
				}
			}
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::DeleteRiderID()
void RYLCreature::DeleteRiderID( DWORD dwRiderID )
{
	for (int i=0; i<Siege::AIRSHIP_RIDER_NUM; ++i)
	{
		if ( m_dwRiderID[i] == dwRiderID )
		{
			m_dwRiderID[i] = 0 ;
			return ;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::SetRiders( DWORD* pdwRiders )
void RYLCreature::SetRiders( DWORD* pdwRiders )
{
	std::copy(&pdwRiders[0], &pdwRiders[Siege::AIRSHIP_RIDER_NUM], &m_dwRiderID[0]) ;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetCastleDoorUpgradePrice()
unsigned long RYLCreature::GetCastleDoorUpgradePrice()
{ 
	return ( unsigned long )( ( float )GetCastleDoorRecoveryPrice() * 0.2f * ( float )m_cUpgradeStep ) ; 
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetCastleDoorUpgradeTime()
unsigned long	RYLCreature::GetCastleDoorUpgradeTime()
{ 
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( GetRealObjType() ) ;

	return lpMonster->m_MonsterInfo.m_cUpgradeSpeed ;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetCastleDoorRepairPrice()
unsigned long RYLCreature::GetCastleDoorRepairPrice()
{
    CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( m_ulCharID );
    if ( !pData )
        return 0;

	// 1~10 대미지는 100 Godl으로 간주.. 
	return ( pData->m_ulMaxHP - pData->m_lCurrHP ) * Siege::CAMP_REPAIR_GOLD_PER_HP ; 
}	

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetCastleDoroRecoveryPrice()
unsigned long RYLCreature::GetCastleDoorRecoveryPrice()    
{ 
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( GetRealObjType() ) ;

	CRYLCastle*     lpCastle   = CRYLCastleManager::Instance()->FindCastle( GetCastleID() ) ;
	RYLCreature*    lpEmblem   = lpCastle->GetEmblem() ;
	FLOAT		    fGoldDown  = 0.0f ;
    
	const CMonsterMgr::MonsterProtoType *lpMonster2 = g_MonsterMgr.GetMonsterProtoType( lpEmblem->GetRealObjType( true ) ) ;

	if ( lpMonster )
	{
		fGoldDown = lpMonster2->m_MonsterInfo.m_fDevelopGoldDown ; // 생성비용절감
	}

	return lpMonster->m_MonsterInfo.m_dwDevelopGold - ( lpMonster->m_MonsterInfo.m_dwDevelopGold * fGoldDown ) ;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetCastleDoorRecoveryTime()
unsigned long RYLCreature::GetCastleDoorRecoveryTime()
{
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( GetRealObjType() ) ;

	return lpMonster->m_MonsterInfo.m_cDevelopSpeed ;
}


//--------------------------------------------------------------------------------------------------------------------------
// RYLCreature::GetRealObjType()
unsigned short RYLCreature::GetRealObjType( bool bEmblemEnable )
{
	if ( bEmblemEnable )
	{
		return  ( m_wObjectType + ( m_cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
						+ ( m_cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
						+ m_cUpgradeStep ) ;
	}
	else 
	{
		return  ( m_wObjectType + ( m_cState  * Siege::UPGRADE_KIND_NUM ) + m_cUpgradeStep ) ; 
	}
}

unsigned long	RYLCreature::GetCastleArmsDevelopePrice( DWORD dwArmsType )
{
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( dwArmsType ) ;
	if ( lpMonster )
	{
		return lpMonster->m_MonsterInfo.m_dwDevelopGold ;
	}

	return 1000000 ;
}

unsigned long	RYLCreature::GetCastleArmsDevelopeTime( DWORD dwArmsType )
{
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( dwArmsType ) ;
	if ( lpMonster )
	{
		return lpMonster->m_MonsterInfo.m_cDevelopSpeed ;
	}

	return 10 ;
}

unsigned long	RYLCreature::GetCastleArmsRepairPrice()
{
	CharDataNode* pTempData	= RYLCharacterDataManager::Instance()->GetCharData( GetCharID() ) ;
	if ( pTempData )
	{
		unsigned long ulDiff = pTempData->m_ulMaxHP - pTempData->m_lCurrHP ;
		float fPercent = (float)ulDiff / (float)pTempData->m_ulMaxHP * 100.0f ;
		return static_cast<unsigned long>( ceilf( fPercent / 10.0f ) * Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT ) ;
	}

	return 1000000 ;
}

unsigned long	RYLCreature::GetCastleArmsRepairTime()
{
	return Siege::CASTLE_ARMS_REPAIR_TIME ;
}
