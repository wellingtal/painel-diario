// CharacterControl.cpp: implementation of the CCharacterControl class.
//
//////////////////////////////////////////////////////////////////////

#include <winsock2.h>
#include <mmsystem.h>

#include "FrameTimer.h"
#include "WinInput.h"

#include "SoundMgr.h"
#include "RYLObjectControl.h"
#include "SceneManager.h"
#include "BaseGraphicsLayer.h"
#include "CameraControl.h"

#include "..\\Effect\\CEffscript.h"

#include "RYLGameData.h"
#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "RYLCharacterControl.h"
#include "RYLShortAttackWeaponControl.h"
#include "RYLLongAttackWeaponControl.h"
#include "RYLShortDefenceWeaponControl.h"
#include "RYLLongDefenceWeaponControl.h"
#include "RYLDropshipControl.h"

#include <Network/ClientSocket/Parse/ParseCastle.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Creature/EnemyCheck.h>

#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include "NPC/NPCList.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLDialog.h"
#include "RYLNetworkData.h"

#include "RYLJoystickConfigurePanel.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int MAXCHRLOAD=30;

const float fDeleteChrLens=10000.0f;

#define ATTACKABLE_RANGE 150.0f
#define TARGETNEAR_RANGE 100.0f
#define PLAYERTARGETNEAR_RANGE 1000.0f

#include "DeviceInput.h"
#include "GMMemory.h"
unsigned char KeyPressed(unsigned char key)
{
	if (g_DeviceInput.GetIsFocus())
	{
		return HIBYTE( GetAsyncKeyState( key ) );
	}

	return 0;
}

bool		( *CCharacterControl::IsExistToList )			( unsigned short List_In, DWORD CharID_In ) = NULL ;
bool		( *CCharacterControl::CharAddressRequireInfo )	( DWORD SenderID_In, DWORD CharID_In ) = NULL ;
BOOL		( *CCharacterControl::SendMovingPacket )		( unsigned long dwUpperAni, unsigned long dwLowerAni, 
															  float fDir, vector3 &vecMove ) = NULL ;
BOOL		( *CCharacterControl::SendCharRespawn )			( unsigned long dwChrID ) = NULL ;
BOOL		( *CCharacterControl::SendMoveUpdatePacket )	( float fDir, vector3 &vecPosition ) = NULL ;
BOOL		( *CCharacterControl::SendCharAttack )			( vector3 &vecPos, float fDir, unsigned short wType, bool bCasting, 
															  unsigned char cSkillLock, unsigned char cSkillLevel, 
															  unsigned char cAtCount, unsigned short wDefenserNum, 
															  unsigned long dwDefenser[ 10 ],
															  bool bMissileAttack, vector3 &vecDstPos ) = NULL ;
bool		( *CCharacterControl::PickItem )				( DWORD nItemID ) = NULL ;
char*		( *CCharacterControl::GetMotionSheet )			( char *strWeaponName, char *strShieldName, unsigned short wClass ) = NULL ;
unsigned long ( *CCharacterControl::GetWeaponType )			( char *strWeaponName ) = NULL ;
void		( *CCharacterControl::CheckTargetLength )		( unsigned long &dwTargetID ) = NULL ;
void		( *CCharacterControl::CheckTargetforSkill )		( unsigned long &dwTargetID, BOOL bCheckLength ) = NULL ;
BOOL		( *CCharacterControl::CheckSkillStart )			( BOOL bDown ) = NULL ;
BOOL		( *CCharacterControl::CheckStillCasting )		( unsigned long &dwFunction ) = NULL ;
void		( *CCharacterControl::SetAutoRun )				( BOOL bAutoRun ) = NULL ;
BOOL		( *CCharacterControl::GetAutoRun )				( void ) = NULL ;
void		( *CCharacterControl::ChangeWeapon )			( void ) = NULL ;
void		( *CCharacterControl::SetChangeWeapon )			( BOOL bChangeWeapon ) = NULL ;
vector3		( *CCharacterControl::GetMonsterMinBox )		( unsigned long dwChrID ) = NULL ;
vector3		( *CCharacterControl::GetMonsterMaxBox )		( unsigned long dwChrID ) = NULL ;
void		( *CCharacterControl::SetKillCounter )			( long lKillCounter ) = NULL ;
void		( *CCharacterControl::SetClientExitCounter )	( long lExitCounter ) = NULL ;
void		( *CCharacterControl::SetLogoutCounter )		( long lLogoutCounter ) = NULL ;
void		( *CCharacterControl::SetPortalCounter )		( long lPortalCounter ) = NULL ;
void		( *CCharacterControl::SetSkillUseCounter )		( long lSkillUseCounter ) = NULL ;
BOOL		( *CCharacterControl::CheckAttackable )			( void ) = NULL ;
BOOL		( *CCharacterControl::SendSummonAttack )		( unsigned long dwTargetID ) = NULL ;
unsigned long ( *CCharacterControl::GetClientIdentity )		( unsigned long dwChrID ) = NULL;
unsigned long ( *CCharacterControl::GetClientEnemyType )	( unsigned long dwChrID) = NULL;
unsigned short ( *CCharacterControl::GetEnchantLevel )		( unsigned long lID ) = NULL;
unsigned long ( *CCharacterControl::GetEnchantTime )		( unsigned long lID ) = NULL;
unsigned short ( *CCharacterControl::GetSkillIDbyChantID )	( unsigned char cChant ) = NULL;
void		( *CCharacterControl::UpdateCamp )				( RYLCreature* pCreature ) = NULL;

CCharacterControl::CCharacterControl()
{
	m_lpDuelReady				= NULL ;
	m_bComboSuccess				= FALSE ;
	m_bFreelook					= FALSE ;
	m_vecCameraPos				= vector3( 0.0f, 0.0f, 0.0f ) ;
	m_fCameraDir				= 0.0f ;

	m_MouseClickMode			= 0 ;
	m_bSkillExecuted			= FALSE ;
	m_vecPrePosition			= vector3( 0.0f, 0.0f, 0.0f ) ;
	m_nCameraAnimation			= 0 ;
	m_bClientUsed				= false ;
	m_bClickMove				= false ;
	m_bAttackable				= TRUE ;
	m_bAttackResult				= TRUE ;

	m_bSummon					= FALSE ;
	m_bSummonCmd				= FALSE ;
	m_cSummonCmd				= 0 ;

	m_bAutoFollowMode			= FALSE ;
	m_dwAutoFollowChrID			= 0xFFFFFFFF ;

	m_bClickAttack				= false ;
	m_dwClickModeAttackTargetID = 0xFFFFFFFF ;
	m_dwNormalTargetID			= 0xFFFFFFFF ;
	m_dwSpecialTargetID			= 0xFFFFFFFF ;
	m_dwNormalTargetMonsterID	= 0xFFFFFFFF ;
	m_fSkillLength				= 0.0f ;

	m_lAutoRunCounter			= 0 ;
	m_fRunSpeed					= 0.0f ;
	m_fWalkSpeed				= 0.0f ;

	m_fCameraSpeed				= 100.0f ;

	m_bAutoTargetAttack			= false ;
	m_dwAutoTargetID			= 0xFFFFFFFF ;
	m_bSkillAttack				= false ;

	m_nSelectItem				= 0xffffffff ;
	m_SimpleHumanTexture[ 0 ]	= NULL ;
	m_SimpleHumanTexture[ 1 ]	= NULL ;
	m_SimpleHumanTexture[ 2 ]	= NULL ;
	m_SimpleHumanTexture[ 3 ]	= NULL ;

	m_SkillAttackValue			= 0 ;

	m_dwSlideTick = timeGetTime();

	InitDuelTarget() ;

	//=========================================================
	// 공성관련
	m_pRYLCharacterControl			= new CRYLCharacterControl( this ) ;
	m_pRYLShortAttackWeaponControl	= new CRYLShortAttackWeaponControl( this ) ;
	m_pRYLLongAttackWeaponControl	= new CRYLLongAttackWeaponControl( this ) ;
	m_pRYLShortDefenceWeaponControl = new CRYLShortDefenceWeaponControl( this ) ;
	m_pRYLLongDefenceWeaponControl	= new CRYLLongDefenceWeaponControl( this ) ;
	m_pRYLDropshipControl			= new CRYLDropshipControl( this ) ;	
	m_WeaponControlKind				= CHARACTER_CONTROL ;
}

CCharacterControl::~CCharacterControl()
{
	if ( m_SimpleHumanTexture[ 3 ] ) { delete m_SimpleHumanTexture[ 3 ] ; m_SimpleHumanTexture[ 3 ] = NULL ; }
	if ( m_SimpleHumanTexture[ 2 ] ) { delete m_SimpleHumanTexture[ 2 ] ; m_SimpleHumanTexture[ 2 ] = NULL ; }
	if ( m_SimpleHumanTexture[ 1 ] ) { delete m_SimpleHumanTexture[ 1 ] ; m_SimpleHumanTexture[ 1 ] = NULL ; }
	if ( m_SimpleHumanTexture[ 0 ] ) { delete m_SimpleHumanTexture[ 0 ] ; m_SimpleHumanTexture[ 0 ] = NULL ; }

    RYLCreatureManager::Instance()->ClearCreatureManager();

	if ( m_pRYLDropshipControl )			{ delete m_pRYLDropshipControl ;			m_pRYLDropshipControl = NULL ; }
	if ( m_pRYLLongDefenceWeaponControl )	{ delete m_pRYLLongDefenceWeaponControl ;	m_pRYLLongDefenceWeaponControl = NULL ; }
	if ( m_pRYLShortDefenceWeaponControl )	{ delete m_pRYLShortDefenceWeaponControl ;	m_pRYLShortDefenceWeaponControl = NULL ; }
	if ( m_pRYLLongAttackWeaponControl )	{ delete m_pRYLLongAttackWeaponControl ;	m_pRYLLongAttackWeaponControl = NULL ; }
	if ( m_pRYLShortAttackWeaponControl )	{ delete m_pRYLShortAttackWeaponControl ;	m_pRYLShortAttackWeaponControl = NULL ; }
	if ( m_pRYLCharacterControl )			{ delete m_pRYLCharacterControl ;			m_pRYLCharacterControl = NULL ; } 	
}

void		CCharacterControl::Create()
{
	m_dwRunFactorTimer			= CFrameTimer::Regist( 200.0f ) ;
	m_nCharMovingPacketTimerID	= CFrameTimer::Regist( 4.0f ) ;

	if ( m_bClientUsed==false )
		CCameraControl::m_nCameraAnimationTimer = CFrameTimer::Regist( 30.0f ) ;			
	else
		CCameraControl::m_nCameraAnimationTimer=CFrameTimer::Regist( 100.0f ) ;

	if ( m_SimpleHumanTexture[ 0 ]==NULL )
	{
		CTexture::SetPath( INTERFACETEXTUREPATH ) ;
		
		m_SimpleHumanTexture[ 0 ]=new CTexture() ;
		m_SimpleHumanTexture[ 0 ]->Load( "SimpleHumanFront.dds" ) ;
		m_SimpleHumanTexture[ 1 ]=new CTexture() ;
		m_SimpleHumanTexture[ 1 ]->Load( "SimpleHumanBack.dds" ) ;
		m_SimpleHumanTexture[ 2 ]=new CTexture() ;
		m_SimpleHumanTexture[ 2 ]->Load( "SimpleHumanLeft.dds" ) ;
		m_SimpleHumanTexture[ 3 ]=new CTexture() ;
		m_SimpleHumanTexture[ 3 ]->Load( "SimpleHumanRight.dds" ) ;		
	}
}



//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::Update()
void CCharacterControl::Update( float fUpdateTime )
{
    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
    if ( vecCharID.empty() )
        return;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return;
    
    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if ( !pSelfData )
        return;

    static vector3 vecBeforeView = vector3( 0.0f, 0.0f, 0.0f );
    vector3 vecViewPos = CSceneManager::GetCamera()->GetMatPosition()->GetLoc();

    vector3		vecChrPos, vecLens;
	float		fInterLens;
    
    pSelfCreature->SetSelfLens( 0.0f );

    float fCameraDir = pSelfModel->GetDirection();

    vector3 vecCameraInterPos( cosf( fCameraDir ), 0.0f, sinf( fCameraDir ) );
    vecCameraInterPos.Normalize();

    pSelfModel->GetPosition( vecChrPos );
    pSelfCreature->SetPosition( vecChrPos );

    vector3 vecFoward = CSceneManager::GetCamera()->GetViewTowardVector();
    vecFoward *= -1.0f;
    CSceneManager::GetCamera()->SetVecPosition( vecFoward + vecChrPos );

    vector3 vecTemp = vecViewPos - vecBeforeView;

	if ( vecTemp.GetLens() >= 10000.0f )
        CSceneManager::m_HeightField.InitNowPos();

    vecBeforeView = vecViewPos;

	// Rodin : 아래에 연속으로 나오는 루프들은 합칠 수 없다면, 
	//		별개의 함수로 제작해서 Update 함수의 가독성을 높이도록 합시다.

    // Creature 속성에 따라 충돌 타입 설정
	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 

        if ( NULL == pTempCreature )
            continue;

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

        if ( pTempModel )
        {
            if ( !pTempCreature->GetIsDead() )
            {
                CSceneManager::CharacterCollisionAble( pTempModel, true );
            }
            else
            {
                CSceneManager::CharacterCollisionAble( pTempModel, false );
            } 

            if ( pTempCreature != pSelfCreature )
            {
                switch ( GetClientIdentity( pTempCreature->GetCharID() ) )
                {
                case Creature::CT_PC:
                    {
                        CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
                        CSceneManager::CharacterGravityAble( pTempModel, false );
                    }
                    break;

                case Creature::CT_NPC:
                    {
                        CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_FIRSTBOTTOM );
                        CSceneManager::CharacterGravityAble( pTempModel, true );
                    }
                    break;

                case Creature::CT_MONSTER:
                    {
                        CSceneManager::CharacterCollisionDetectType( pTempModel, pTempCreature->GetCDTType() );
                        CSceneManager::CharacterGravityAble( pTempModel, true );
                    }
                    break;
                }
            }
        }
        else
        {
            vecLens     = *( pSelfCreature->GetPosition() ) - *( pTempCreature->GetPosition() );
            fInterLens  = vecLens.GetLens();
            vecLens		= *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
            pTempCreature->SetSelfLens( vecLens.GetLens() );
            continue;
        }

	
		pTempCreature->SetPosition( pTempModel->GetPosition() );
		pTempCreature->SetDirection( pTempModel->GetDirection() );

	
    }

    // quick sort가 들어가야되는데 creature manager의 자료구조가 map으로 되어 있어서 곤란함 -_-;

	// 지역변수로 vector를 선언하고 map의 내용을 복사한 후 함수자를 이용해서 sort하시면 됩니다. 
	// 살짝 복작한 작업이라 예시는 힘들 듯 하고... 주석다신 분이 요청하시면 제작해 드리겠습니다. (2004-08-14 by 로딘)

    float fAngle;

    // Lens 설정
	pos = vecCharID.begin();
	end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
            continue;

        if ( pTempCreature == pSelfCreature )
            continue;  

        matrix matCameraPos = *CSceneManager::m_ViewCamera->GetMatPosition();
		vector3 vecCameraPos = vector3(matCameraPos._41, matCameraPos._42, matCameraPos._43);
		
		if ( CSceneManager::m_pCameraScript ) 
        {
			if ( CSceneManager::m_pCameraScript->ISPlay() ) 
            {
				D3DXVECTOR3 vecDXCameraPos = CSceneManager::m_pCameraScript->GetVecPos();
				vecCameraPos = vector3( vecDXCameraPos.x, vecDXCameraPos.y, vecDXCameraPos.z );
			}
		}
		
        vecChrPos	= *(pTempCreature->GetPosition() );
		vecLens		= vecCameraPos - vecChrPos;
		fInterLens	= vecLens.GetLens();
		vecLens		= vecChrPos - *( pTempCreature->GetPosition() );
        pTempCreature->SetSelfLens( vecLens.GetLens() );
		vecLens.y	= 0.0f;
		vecLens.Normalize();

		fAngle = vecCameraInterPos * vecLens;
		fAngle = acosf( fAngle );
		fAngle = fabsf( fAngle );

        if ( fAngle <= FLOAT_PHI / 3.0f )
		{
            pTempCreature->SetSelfLens( pTempCreature->GetSelfLens() * 0.1f );
		}

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

		if ( pTempModel )
		{
			if ( fInterLens <= 1800.0f )
			{
				pTempModel->SetLODLevel( 0 );
			}
			else if ( fInterLens <= 3000.0f )
			{
				pTempModel->SetLODLevel( 1 );
			}
			else if ( fInterLens <= 4500.0f )
			{
				pTempModel->SetLODLevel( 2 );
			}
			else
			{
				pTempModel->SetLODLevel( 3 );
			}
			
			const CMonsterMgr::MonsterProtoType* lpMonster = 
				g_MonsterMgr.GetMonsterProtoType(pTempCreature->GetCharID() & 0x0000FFFF);            
			if (NULL != lpMonster)
			{
				if (false == lpMonster->m_MonsterInfo.m_bUseLOD)
				{
					pTempModel->SetLODLevel( 0 );
				}
			}
		}
    }

	pos = vecCharID.begin();
	end = vecCharID.end();

	for (unsigned long ulLoopCount = 0; pos != end; ++pos, ++ulLoopCount)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
            continue;

        if ( static_cast<int>( ulLoopCount ) >= MAXCHRLOAD )
            break;

        if ( pTempCreature == pSelfCreature )
            continue;

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

        if ( !pTempModel && pTempCreature->GetIsUsed() )
        {                                  
            pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

            pTempCreature->SetShape( true );

            if ( pSelfCreature == pTempCreature )
            {
				// by Hades Kang - option dialog < chant effect >
				pTempCreature->GetEffect()->SetChantEffect( 
					pTempCreature->GetEffect()->GetChantStatus(), true, m_bGoodChantEffectShow[0], m_bBadChantEffectShow[0] );
			}
			else
			{
				// by Hades Kang - option dialog < chant effect >
				int index = 0;
				switch ( g_CharacterData.GetClientEnemyType( pTempCreature->GetCharID()) )
				{
					case EnemyCheck::EC_FRIEND:		index = 1;	break;
					case EnemyCheck::EC_NEUTRAL:	index = 2;	break;
					case EnemyCheck::EC_ENEMY:		index = 3;	break;
				}

				pTempCreature->GetEffect()->SetChantEffect( 
					pTempCreature->GetEffect()->GetChantStatus(), false, m_bGoodChantEffectShow[index], m_bBadChantEffectShow[index] );
            }

            if ( GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_NPC )
            {
                pTempModel->SetPosition( *( pTempCreature->GetPosition() ) + vector3( 0.0f, -50.0f, 0.0f ) );

                CCharacterLightShadowManager::CCharacterDataNode CheckNode = CSceneManager::GetCharacterDataNode( pTempModel );
				CheckNode.m_CollisionType = CT_NONE;
				CSceneManager::SetCharacterDataNode( CheckNode );
				CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_FIRSTBOTTOM );
            }
            else
            {
                pTempModel->SetPosition( *(pTempCreature->GetPosition() ) );
            }

            pTempModel->SetScale( pTempCreature->GetScale() );
            pTempModel->SetDirection( pTempCreature->GetDirection() );
        }
    }

    // ?
    CCharacterLightShadowManager::CCharacterDataNode CheckNode;

	pos = vecCharID.begin();
	end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
			continue;

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
		
		if ( NULL == pTempModel )
            continue;

		// 병기에 탑승한 캐릭터는 그리지 않는다.
		if ( pTempCreature->GetRideEnable() )
		{
			//if(pTempCreature->GetCharID()!=pSelfCreature->GetRiderID(0))
			{
				CheckNode = CSceneManager::GetCharacterDataNode( pTempModel );
				pTempCreature->GetEffect()->SetEffectVisibility( false );
				CheckNode.m_bRender     = false;
				CheckNode.m_fAlphaValue = -1.0f;
				CSceneManager::SetCharacterDataNode( CheckNode );
			}

			continue;
		}

        CheckNode = CSceneManager::GetCharacterDataNode( pTempModel );
		//int totalkillstreak = pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ) + 
		//	pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ) + pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ) + 
		//	pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ) + pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ) + 
		//	pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ) + pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 );
		//switch(totalkillstreak)
		//{
		//case 1:
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_100.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;
		//
		//case 2:
		//	pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_100.esf");
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_101.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;

		//case 3:
		//	pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_101.esf");
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_102.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;

		//case 4:
		//	pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_102.esf");
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_103.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;

		//case 5:
		//	pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_103.esf");
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_104.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;

		//case 6:
		//	pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_104.esf");
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_105.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;

		//case 7:
		//	pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_105.esf");
		//	pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_106.esf", static_cast<unsigned long>( 0 ), true, true );
		//	break;

		//default:
		//	break;

		//}
		if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_100.esf", static_cast<unsigned long>( 0 ), true, true );
		 }
	if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_100.esf");
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_101.esf", static_cast<unsigned long>( 0 ), true, true );
		 }
	if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_101.esf");
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_102.esf", static_cast<unsigned long>( 0 ), true, true );
		 }
	if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_102.esf");
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_103.esf", static_cast<unsigned long>( 0 ), true, true );
		 }
	if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_103.esf");
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_104.esf", static_cast<unsigned long>( 0 ), true, true );
		 }
	if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_104.esf");
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_105.esf", static_cast<unsigned long>( 0 ), true, true );
		 }
	if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
			&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
		 {
			 pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_105.esf");
			 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_106.esf", static_cast<unsigned long>( 0 ), true, true );
		 }

	//
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_100.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		 //pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_100.esf");
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_101.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		// pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_101.esf");
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_102.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		// pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_102.esf");
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_103.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		// pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_103.esf");
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_104.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		// pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_104.esf");
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_105.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
	//if ((pTempCreature->GetCharacterName() != pSelfCreature->GetCharacterName())&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff1 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff2 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff3 ))&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff4 ))
	//		&&( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff5 ))&&( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::KillStreakBuff6 )))//hz killstreak show check
	//	 {
	//		// pTempCreature->GetEffect()->DeleteBodyEffect( "PC_Level_105.esf");
	//		 pTempCreature->GetEffect()->AddBodyEffect( "PC_Level_106.esf", static_cast<unsigned long>( 0 ), true, true );
	//	 }
        if ( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) )
        {
            switch ( GetClientIdentity( pTempCreature->GetCharID() ) )
            {
            case Creature::CT_PC:
                {
                    if ( GetClientEnemyType( pTempCreature->GetCharID() ) == EnemyCheck::EC_ENEMY )
                    {
                        pTempCreature->GetEffect()->SetEffectVisibility( false );
                        CheckNode.m_bRender     = false;
                        CheckNode.m_fAlphaValue = -1.0f;
                    }
                    else
					{
						if(pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::MichaelBless ) )
						{
						    CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
							if(pTempData && (pTempData->m_cNation == pSelfData->m_cNation))
							{
								// edith 2009.02.28 스텔스 사용시 캐릭터 반투명하게 수정
								pTempCreature->GetEffect()->SetEffectVisibility( true );
								CheckNode.m_bRender		= true;
								CheckNode.m_fAlphaValue = 0.5f;
							}
							else
							{
								pTempCreature->GetEffect()->SetEffectVisibility( false );
								CheckNode.m_bRender		= false;
								CheckNode.m_fAlphaValue = -1.0f;
							}
						}
						else
						{
							pTempCreature->GetEffect()->SetEffectVisibility( true );
							CheckNode.m_bRender		= true;
							CheckNode.m_fAlphaValue = 0.5f;
						}
					}
                }
                break;

            case Creature::CT_MONSTER:
                {
                    pTempCreature->GetEffect()->SetEffectVisibility( false );
					CheckNode.m_bRender		= false;
					CheckNode.m_fAlphaValue = -1.0f;
                }
                break;

            case Creature::CT_NPC:            
                {
                    pTempCreature->GetEffect()->SetEffectVisibility( true );
					CheckNode.m_bRender		= true;
					CheckNode.m_fAlphaValue = -1.0f;
                }
                break;
            }
        }
        else
        {
			CheckNode.m_bRender		= true;
			CheckNode.m_fAlphaValue = -1.0f;
        }

        CSceneManager::SetCharacterDataNode( CheckNode );

        if ( pTempCreature == pSelfCreature )
            continue;

        switch ( GetClientIdentity( pTempCreature->GetCharID() ) )
        {
        case Creature::CT_PC:
			{
				UpdatePlayer( pTempCreature, fUpdateTime );
            }
			break;
		case Creature::CT_SIEGE_OBJECT:
			{
				if( UpdateCamp )
					UpdateCamp( pTempCreature );
				UpdatePlayer( pTempCreature, fUpdateTime );
			}
			break;

		case Creature::CT_NPC:
		case Creature::CT_MONSTER:	
		case Creature::CT_STRUCT:
		case Creature::CT_SUMMON:
            {
				UpdateMonster( pTempCreature, fUpdateTime );
            }
			break;
        }
    }

    if ( m_bFreelook )
	{
		CheckNode = CSceneManager::GetCharacterDataNode( pSelfModel ) ;
        pSelfCreature->GetEffect()->SetEffectVisibility( false ) ;
		CheckNode.m_bRender		= false ;
		CheckNode.m_fAlphaValue = -1.0f ;
		CSceneManager::SetCharacterDataNode( CheckNode ) ;
	}

    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

	pos = vecCharID.begin();
	end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature2 = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature2 )
            continue;

		/// 몸에 밖히는 화살 업데이트		
        pTempCreature2->GetEffect()->ArrowUpdate();
		/// 캐릭터에 붙는 effect 업데이트
		pTempCreature2->GetEffect()->BodyEffectUpdate();
		/// weapon effect update
		pTempCreature2->GetEffect()->WeaponEffectUpdate();
	}

	unsigned long	CurrentSlideTick = timeGetTime() ;
	int				slidenum = ( CurrentSlideTick - m_dwSlideTick ) / DEFAULTESTICK ;

	for ( int slc = 0 ; slc < slidenum ; slc ++ ) 
	{
		m_dwSlideTick = CurrentSlideTick ;

        RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

		pos = vecCharID.begin();
		end = vecCharID.end();

		for (; pos != end; ++pos)
		{
			RYLCreature* pTempCreature2 = RYLCreatureManager::Instance()->GetCreature(*pos); 
            if ( NULL == pTempCreature2 )
                continue;

			pTempCreature2->PlaySlide() ;
		}
	}    
}

unsigned long CCharacterControl::GetScreenPosChr(POINT &Point, int nMode, float fLength)
{
	return 0xFFFFFFFF;
}

void		CCharacterControl::MouseMove( int MouseX, int MouseY )
{
}

void		CCharacterControl::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{	
	m_pRYLLongAttackWeaponControl->Render( pd3dDevice ) ;
}

void		CCharacterControl::MakeShake(int nChrID)
{
}


bool		LineArcIntersection( vector3 vecPlaneNormal, vector3 vecPlaneOrigin, 
								 vector3 vecOrigin, vector3 vecNormal, 
								 float fAngleArc, float fRad )
{	
	float	fD = CIntersection::PointFromPlane( vecPlaneNormal,vecPlaneOrigin,vecOrigin ) ;
	float	fAngleTheta = acos( fD / fRad ) ;
	float	fAngleInter = vecPlaneNormal * vecNormal ;

	if ( (fAngleTheta + fAngleArc ) > fAngleInter )
		return true ;

	return false ;
}

bool		BoxIn( vector3 vecPos, vector3 vecMax, vector3 vecMin, float fRad )
{
	vector3		vecTmp ;
	float		fTmpLength = 0.0f ;

	vecTmp		= vecPos - vecMin ;
	fTmpLength  = vecTmp.GetLens() ;

	if ( fTmpLength < ( fRad ) )
		return true ;
	
	vecTmp		= vecPos - vecMax ;
	fTmpLength  = vecTmp.GetLens() ;

	if ( fTmpLength < ( fRad ) )
		return true ;
	
	return false ;
}

bool		BoxOut( vector3 vecPos, vector3 vecMax, vector3 vecMin, float fRad )
{
	if ( vecMax.x+fRad <= vecPos.x || vecMax.z+fRad <= vecPos.z ||
		 vecMin.x-fRad >= vecPos.x || vecMin.z-fRad >= vecPos.z )
	{
		return true ;
	}

	return false;
}

bool		CylinderOBBIntersection( vector3 vecPos, vector3 vecDir, float fArcAngle,
									 vector3 vecMax, vector3 vecMin, float fRad )
{
	if ( BoxIn( vecPos, vecMax, vecMin, fRad ) )
	{
		return true ;
	}
	else if ( BoxOut( vecPos, vecMax, vecMin, fRad ) )
	{
		return false ;
	}

	if ( LineArcIntersection( vector3( 0.0f, 0.0f, -1.0f ), vecMax, vecPos, vecDir, fArcAngle, fRad ) )	
		return true ;
	if ( LineArcIntersection( vector3( 0.0f, 0.0f,  1.0f ), vecMin, vecPos, vecDir, fArcAngle, fRad ) )	
		return true ;
	if ( LineArcIntersection( vector3( 1.0f, 0.0f,  0.0f ), vecMin, vecPos, vecDir, fArcAngle, fRad ) )	
		return true ;
	if ( LineArcIntersection( vector3(-1.0f, 0.0f,  0.0f ), vecMax, vecPos, vecDir, fArcAngle, fRad ) )	
		return true ;

	return false; 	
}

bool		CCharacterControl::MakeAttackInfo( unsigned short wSkill, unsigned char cSkillLock, unsigned char cSkillLevel, 
											   unsigned char cAtCount, float fAttackAngle, float fAttackRate, BOOL bAngle, BOOL bGod )
{
	bool result = false;
	switch ( m_WeaponControlKind )
	{
		case CHARACTER_CONTROL :				// 캐릭터 컨트롤
			result = m_pRYLCharacterControl->MakeAttackInfo( wSkill, cSkillLock, cSkillLevel, cAtCount, fAttackAngle, fAttackRate, bAngle, bGod ) ;
			break ;
		case LONG_ATTACKWEAPON_CONTROL :		// 원거리 공성병기 컨트롤
			result = m_pRYLLongAttackWeaponControl->MakeAttackInfo( wSkill, cSkillLock, cSkillLevel, cAtCount, fAttackAngle, fAttackRate, bAngle, bGod ) ;
			break ;
		case SHORT_ATTACKWEAPON_CONTROL :		// 근거리 공성병기 컨트롤
			result = m_pRYLShortAttackWeaponControl->MakeAttackInfo( wSkill, cSkillLock, cSkillLevel, cAtCount, fAttackAngle, fAttackRate, bAngle, bGod ) ;
			break ;
		case LONG_DEFENCEWEAPON_CONTROL :		// 원거리 수성병기 컨트롤
			result = m_pRYLLongDefenceWeaponControl->MakeAttackInfo( wSkill, cSkillLock, cSkillLevel, cAtCount, fAttackAngle, fAttackRate, bAngle, bGod ) ;
			break ;
		case SHORT_DEFENCEWEAPON_CONTROL :		// 근거리 수성병기 컨트롤
			result = m_pRYLShortDefenceWeaponControl->MakeAttackInfo( wSkill, cSkillLock, cSkillLevel, cAtCount, fAttackAngle, fAttackRate, bAngle, bGod ) ;
			break ;
		case DROPSHIP_CONTROL :					// 수송선 컨트롤
			result = m_pRYLDropshipControl->MakeAttackInfo( wSkill, cSkillLock, cSkillLevel, cAtCount, fAttackAngle, fAttackRate, bAngle, bGod ) ;
			break ;		
	}
	return result;
}

void		CCharacterControl::SetCamera( int dx, int dy, int dz )
{	
	switch ( m_WeaponControlKind )
	{
		case CHARACTER_CONTROL :				// 캐릭터 컨트롤
			m_pRYLCharacterControl->SetCamera( dx, dy, dz ) ;
			break ;
		case LONG_ATTACKWEAPON_CONTROL :		// 원거리 공성병기 컨트롤
			m_pRYLLongAttackWeaponControl->SetCamera( dz, dy, dz ) ;
			break ;
		case SHORT_ATTACKWEAPON_CONTROL :		// 근거리 공성병기 컨트롤
			m_pRYLShortAttackWeaponControl->SetCamera( dz, dy, dz ) ;
			break ;
		case LONG_DEFENCEWEAPON_CONTROL :		// 원거리 수성병기 컨트롤
			m_pRYLLongDefenceWeaponControl->SetCamera( dx, dy, dz ) ;
			break ;
		case SHORT_DEFENCEWEAPON_CONTROL :		// 근거리 수성병기 컨트롤
			m_pRYLShortDefenceWeaponControl->SetCamera( dx, dy, dz ) ;
			break ;
		case DROPSHIP_CONTROL :					// 수송선 컨트롤
			m_pRYLDropshipControl->SetCamera( dz, dy, dz ) ;
			break ;		
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::UpdatePlayer()
void CCharacterControl::UpdatePlayer( RYLCreature* pCreature, float fUpdateTime )
{
    pCreature->UpdatePlayerAction();
}

//--------------------------------------------------------------------------------------------------------------------------
// GetChantID()
unsigned long GetChantID( string strScriptFile )
{
    unsigned long  ulChantID = 0;

    if ( strScriptFile.compare( "Chant_BattleSong.esf" ) )
    {
        ulChantID = 1;
    }
    else if ( strScriptFile.compare( "Chant_MaintenanceChant.esf" ) )
    {
        ulChantID = 2;
    }
    else if ( strScriptFile.compare( "Chant_AccelerationChant.esf" ) )
    {
        ulChantID = 3;
    }
    else if ( strScriptFile.compare( "Chant_LifeAura.esf" ) )
    {
        ulChantID = 4;
    }
	else if ( strScriptFile.compare( "Chant_Blaze.esf" ) )
    {
        ulChantID = 15;
    }
    else if ( strScriptFile.compare( "Chant_Charging.esf" ) )
    {
        ulChantID = 16;
    }
    else if ( strScriptFile.compare( "Chant_ManaShell2.esf" ) )
    {
        ulChantID = 18;
    }
    else if ( strScriptFile.compare( "Chant_Encourage.esf" ) )
    {
        ulChantID = 19;
    }
    else if ( strScriptFile.compare( "Chant_EnchantWeapon.esf" ) )
    {
        ulChantID = 20;
    }
    else if ( strScriptFile.compare( "Chant_BrightArmor.esf" ) )
    {
        ulChantID = 21;
    }
    else if ( strScriptFile.compare( "Chant_HardenSkin.esf" ) )
    {
        ulChantID = 22;
    }
    else if ( strScriptFile.compare( "Chant_Flexibility.esf" ) )
    {
        ulChantID = 23;
    }
    else if ( strScriptFile.compare( "Chant_Guard.esf" ) )
    {
        ulChantID = 24;
    }
    else if ( strScriptFile.compare( "Chant_Hold.esf" ) )
    {
        ulChantID = 25;
    }
    else if ( strScriptFile.compare( "Chant_Stun.esf" ) )
    {
        ulChantID = 26;
    }
    else if ( strScriptFile.compare( "Chant_Frozen.esf" ) )
    {
        ulChantID = 27;
    }
    else if ( strScriptFile.compare( "Chant_Poisoned.esf" ) )
    {
        ulChantID = 28;
    }
    else if ( strScriptFile.compare( "Chant_LowerStrength.esf" ) )
    {
        ulChantID = 29;
    }
	else if ( strScriptFile.compare( "Chant_Fired.esf" ) )
	{
		ulChantID = 51;
	}
	else if ( strScriptFile.compare( "Chant_CurseOfBlind.esf" ) )
	{
		ulChantID = 52;
	}
	else if ( strScriptFile.compare( "PC_Level_100.esf" ) )
	{
		ulChantID = 53;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "PC_Level_101.esf" ) )
	{
		ulChantID = 54;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "PC_Level_102.esf" ) )
	{
		ulChantID = 55;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "PC_Level_103.esf" ) )
	{
		ulChantID = 56;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "PC_Level_104.esf" ) )
	{
		ulChantID = 57;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "PC_Level_105.esf" ) )
	{
		ulChantID = 58;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "PC_Level_106.esf" ) )
	{
		ulChantID = 59;//hz killstreak buff was 53
	}
	else if ( strScriptFile.compare( "Chant_StealHand.esf" ) )
	{
		ulChantID = 64;
	}

    return ulChantID;
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::UpdateMonster()
void CCharacterControl::UpdateMonster( RYLCreature* pCreature, float fUpdateTime )
{
    CZ3DGeneralChrModel* pModel = pCreature->GetCharModel();

    if ( pModel )
    {
		Z3DTOK TokEffect;
            
		if ( pModel && pModel->CheckEvent( "EFFECT", &TokEffect ) )
		{
			const char* pstrEffectName = g_TokEventName.GetString( TokEffect );
	        
			pCreature->GetEffect()->MonsterEffectProcess( pstrEffectName, 1 );
		}

        if ( pCreature->GetDeadTimer() >= 0.0f )
        {
			

			pCreature->SetDeadTimer( pCreature->GetDeadTimer() - 0.01f );
            if ( pCreature->GetDeadTimer() <= 0.0f )
            {
                RYLCreatureManager::Instance()->DeleteCreature( pCreature->GetCharID() );
                return;
            }

            pModel->SetAlphaLevel( pCreature->GetDeadTimer() );
            return;
        }

        pCreature->UpdateMonsterAction();
   }
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::GetClientSummon()
unsigned long CCharacterControl::GetClientSummon( unsigned long dwChrID, BOOL bNation, BOOL bForSkill )
{
	if ( CRYLGameData::GetClientIdentity( dwChrID ) == Creature::CT_SUMMON )
	{
        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwChrID );
        if ( !pTempCreature )
            return 0;

        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwChrID );
        if ( !pTempData )
            return 0;

        RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return 0;

        CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
        if ( !pSelfData )
            return 0;
		
		unsigned long dwResult = Creature::CT_PC;

		BOOL bCheck = FALSE;
        if ( GetIsDuelTarget( pTempData->m_ulSummonMasterCharID ) )
		{
			dwResult = Creature::CT_MONSTER;
		} 
        else if ( !pSelfData->m_bPeaceMode )
		{
			bCheck = TRUE;
		}
		else if ( bForSkill )
		{
			bCheck = TRUE;
		}

		if ( bCheck )
		{
            vector<unsigned long> vecCharID;
            RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

			std::vector<unsigned long>::iterator pos = vecCharID.begin();
			std::vector<unsigned long>::iterator end = vecCharID.end();

			for (; pos != end; ++pos)
			{
				RYLCreature* pTempCreature2 = RYLCreatureManager::Instance()->GetCreature(*pos); 
                if ( NULL == pTempCreature2 )
                    continue;

                if ( pSelfCreature == pTempCreature2 )
                    continue;

                CharDataNode* pTempData2 = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature2->GetCharID() );
                if ( NULL == pTempData2 )
                    continue;

                if ( pTempCreature2->GetCharID() == pTempData->m_ulSummonMasterCharID )
                {
                    if ( pTempData2->m_cNation != pSelfData->m_cNation )
                    {
                        if ( !pSelfData->m_bPeaceMode )
                            dwResult = Creature::CT_MONSTER;
                    }
                    break;
                }
            }
		}

		return dwResult;
	}
	else if ( dwChrID & Creature::MONSTER_BIT )
	{
		return Creature::CT_MONSTER;
	}
	else if ( dwChrID & Creature::NPC_BIT )
	{
		return Creature::CT_NPC;
	}
	else
	{
        RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return false;

        CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
        if ( !pSelfData )
            return 0;

        unsigned long dwResult = Creature::CT_PC;

        if ( bNation )
        {
		    BOOL bCheck = FALSE;

		    if ( GetIsDuelTarget( dwChrID ) )
		    {
			    dwResult = Creature::CT_MONSTER;
		    }
            else if ( !pSelfData->m_bPeaceMode || bForSkill )
		    {
			    bCheck = TRUE;
		    }

		    if ( bCheck )
		    {
                vector<unsigned long> vecCharID;
                RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

				std::vector<unsigned long>::iterator pos = vecCharID.begin();
				std::vector<unsigned long>::iterator end = vecCharID.end();

				for (; pos != end; ++pos)
				{
					RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
                    if ( NULL == pTempCreature )
                        continue;

                    if ( pSelfCreature == pTempCreature )
                        continue;

                    CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
                    if ( NULL == pTempData )
                        continue;

                    if ( pTempCreature->GetCharID() == dwChrID )
                    {
                        if ( pTempData->m_cNation != pSelfData->m_cNation )
                        {
                            if ( !pSelfData->m_bPeaceMode )
                                dwResult = Creature::CT_MONSTER;
                        }
                        break;
                    }
                }
		    }
        }
	    return dwResult;
	}
	return 0;
}

void		CCharacterControl::WrapSendMovingPacket( DWORD dwUpperAction, DWORD dwLowerAction, float fDir, vector3 vecPos )
{
	if ( SendMovingPacket )
	{
		( *SendMovingPacket )( dwUpperAction,dwLowerAction,fDir,vecPos ) ;
	}	
}

BOOL		CCharacterControl::WrapSendCharRespawn( DWORD dwChrID )
{
	if ( SendCharRespawn )
	{
		return ( *SendCharRespawn )( dwChrID ) ;
	}

	return FALSE ;
}

void		CCharacterControl::WrapSendMoveUpdatePacket( float fDir, vector3 vecPos )
{
	if ( SendMoveUpdatePacket )
	{
		( *SendMoveUpdatePacket )( fDir,vecPos ) ;
	}
}

void		CCharacterControl::WrapSendCharAttack( vector3 &vecPos, float fDir, unsigned short wAtType, bool bCasting, 
												   unsigned char cSkillLock, unsigned char cSkillLevel, 
												   unsigned char cAtCount, unsigned short wDefenserNum,unsigned long  lpAtNode[],
												   bool bMissileAttack, vector3 &vecDstPos )
{
	if ( SendCharAttack )
	{
		( *SendCharAttack )( vecPos, fDir, wAtType, bCasting, cSkillLock, cSkillLevel, cAtCount, wDefenserNum, lpAtNode, bMissileAttack, vecDstPos ) ;
	}
}

void		CCharacterControl::MouseModeChange()
{
	if ( m_MouseClickMode == 0 )
	{
		//전투모드
		m_MouseClickMode = 1 ;		

		m_Camera.m_nCameraMode = 2 ;
		m_Camera.m_nPrepareCameraMode = 1 ;
		
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return;

        CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
        if ( pSelfModel )
		{
            pSelfModel->SetDirection( -m_Camera.m_fCameraRotX + 3.14159f / 2.0f ) ;

			vector3 vecChrPos ;

			pSelfModel->GetPosition( vecChrPos ) ;
			vecChrPos += vector3( 0.0f,150.0f,0.0f ) ;

			vector3		vecStart,vecEnd ;

			m_Camera.CalcCameraPosition( vecChrPos,-m_Camera.m_fCameraRotX,
										 m_Camera.m_fBattleCameraRotY,
										 m_Camera.m_fBattleInterCharacterCamera,vecEnd ) ;

			m_Camera.CalcCameraPosition( vecChrPos,-m_Camera.m_fCameraRotX,
										 m_Camera.m_fCameraRotY,
										 m_Camera.m_fMouseClickInterCharacterCamera,
										 vecStart);

			m_Camera.m_vecStartPosition = vecStart ;
			m_Camera.m_vecEndPosition	= vecEnd ;
			m_Camera.m_vecNowPosition	= vecStart ;
			m_Camera.m_vecTargetCameraPosition = vecChrPos ;

			vector3		vecDir = vecEnd - vecStart ;
			vecDir.Normalize() ;
			m_Camera.m_vecVelocity = vector3( 0.0f,0.0f,0.0f ) ;
			m_Camera.m_vecAccelate = vecDir * 0.16f ;// 30/sec Adder
			m_Camera.m_fTotalTimeInterval = 60.0f * 10.0f ;			 	
			m_Camera.m_fTime = 0.0f ;
		}
	}
	else
	{
		//마우스 클릭모드

        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

		if ( !pSelfCreature )
            return;

		//CCharacterControl* pObjControl =  m_pRYLCharacterControl->GetCreatureControl();
		//memcpy( &m_vecClickPos, pSelfCreature->GetPosition(), sizeof( m_vecClickPos ) ); 
		// 마우스 이동위치를 현재 자리로 초기화

		m_bClickMove   = false;						// 클릭 이동 유무
	    m_bClickAttack = false; 


        CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
        if ( pSelfModel )
		{
			float fChrDir = pSelfModel->GetDirection() ;		
			
			m_Camera.m_fCameraRotX =- fChrDir + 3.14159f / 2.0f ;
			
			vector3 vecChrPos ;
			pSelfModel->GetPosition( vecChrPos ) ;

			vecChrPos += vector3( 0.0f,150.0f,0.0f ) ;

			vector3		vecStart,vecEnd ;				   

			m_Camera.CalcCameraPosition( vecChrPos, -m_Camera.m_fCameraRotX,
										 m_Camera.m_fBattleCameraRotY,
										 m_Camera.m_fBattleInterCharacterCamera,vecStart ) ;

			m_Camera.CalcCameraPosition( vecChrPos,-m_Camera.m_fCameraRotX,
										 m_Camera.m_fCameraRotY,
										 m_Camera.m_fMouseClickInterCharacterCamera,vecEnd ) ;

			m_Camera.m_vecStartPosition = vecStart ;
			m_Camera.m_vecEndPosition	= vecEnd ;
			m_Camera.m_vecNowPosition	= vecStart ;
			m_Camera.m_vecTargetCameraPosition = vecChrPos ;

			vector3 vecDir = vecEnd - vecStart ;
			vecDir.Normalize() ;
			m_Camera.m_vecVelocity = vector3( 0.0f,0.0f,0.0f ) ;
			m_Camera.m_vecAccelate = vecDir * 0.16f ;// 30/sec Adder
			m_Camera.m_fTotalTimeInterval = 60.0f * 10.0f ;				
			m_Camera.m_fTime = 0.0f ;		
			
			m_MouseClickMode = 0 ;
			m_Camera.m_nCameraMode = 0 ;
			m_Camera.m_nCameraMode = 2 ;
			m_Camera.m_nPrepareCameraMode = 0 ;
		}
	}
}

void		CCharacterControl::SetCameraToCharDir()
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
	if ( pSelfModel )
	{
		float fChrDir = pSelfModel->GetDirection() ;		

		m_Camera.m_fCameraRotX =- fChrDir + 3.14159f / 2.0f ;

		vector3 vecChrPos ;
		pSelfModel->GetPosition( vecChrPos ) ;

		vecChrPos += vector3( 0.0f,150.0f,0.0f ) ;

		vector3	vecEnd ;

		m_Camera.CalcCameraPosition( vecChrPos,-m_Camera.m_fCameraRotX,
									 m_Camera.m_fCameraRotY,
									 m_Camera.m_fMouseClickInterCharacterCamera, vecEnd ) ;

		m_Camera.m_vecStartPosition = vecEnd ;
		m_Camera.m_vecEndPosition	= vecEnd ;
		m_Camera.m_vecNowPosition	= vecEnd ;
		m_Camera.m_vecTargetCameraPosition = vecChrPos ;

		m_MouseClickMode = 0 ;
		m_Camera.m_nCameraMode = 0 ;
		m_Camera.m_nCameraMode = 2 ;
		m_Camera.m_nPrepareCameraMode = 0 ;
	}
}

vector3			*CCharacterControl::GetFindChrPos( unsigned long dwChrID )
{
    RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwChrID );

    if ( pTempCreature )
    {
        return pTempCreature->GetPosition();
    }

    return NULL;
}

bool		CCharacterControl::MakeSkillAttackInfo( unsigned long dwTargetID, unsigned short wSkill, 
												    unsigned char cSkillLock, unsigned char cSkillLevel, 
													unsigned char cAtCount, BOOL bGod, BOOL bDead )
{
	switch ( m_WeaponControlKind )
	{
		case CHARACTER_CONTROL :				// 캐릭터 컨트롤
			m_pRYLCharacterControl->MakeSkillAttackInfo( dwTargetID, wSkill, cSkillLock, cSkillLevel, cAtCount, bGod, bDead ) ;
			break ;
		case LONG_ATTACKWEAPON_CONTROL :		// 원거리 공성병기 컨트롤
			m_pRYLLongAttackWeaponControl->MakeSkillAttackInfo( dwTargetID, wSkill, cSkillLock, cSkillLevel, cAtCount, bGod, bDead ) ;
			break ;
		case SHORT_ATTACKWEAPON_CONTROL :		// 근거리 공성병기 컨트롤
			m_pRYLShortAttackWeaponControl->MakeSkillAttackInfo( dwTargetID, wSkill, cSkillLock, cSkillLevel, cAtCount, bGod, bDead ) ;
			break ;
		case LONG_DEFENCEWEAPON_CONTROL :		// 원거리 수성병기 컨트롤
			m_pRYLLongDefenceWeaponControl->MakeSkillAttackInfo( dwTargetID, wSkill, cSkillLock, cSkillLevel, cAtCount, bGod, bDead ) ;
			break ;
		case SHORT_DEFENCEWEAPON_CONTROL :		// 근거리 수성병기 컨트롤
			m_pRYLShortDefenceWeaponControl->MakeSkillAttackInfo( dwTargetID, wSkill, cSkillLock, cSkillLevel, cAtCount, bGod, bDead ) ;
			break ;
		case DROPSHIP_CONTROL :					// 수송선 컨트롤
			m_pRYLDropshipControl->MakeSkillAttackInfo( dwTargetID, wSkill, cSkillLock, cSkillLevel, cAtCount, bGod, bDead ) ;
			break ;		
	}
	return true ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::MakeAttacked()
void CCharacterControl::MakeAttacked( DWORD dwChrID, unsigned long dwWeaponType )
{
    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
    if ( vecCharID.size() < 2 )
        return;

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
            continue;

		if ( pTempCreature->GetCharID() == dwChrID )
		{
            CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

			if ( pTempModel )
			{
				float fAttackActionRate = rand() % 6;
				fAttackActionRate *= 0.1f;
				fAttackActionRate += 0.3f;

                if ( !( pTempCreature->GetLowerActionID() == CA_SHOT || pTempCreature->GetLowerActionID() == CA_JUMPSHOT || 
					pTempCreature->GetLowerActionID() == CA_FIRE1 || pTempCreature->GetLowerActionID() == CA_FIRE2 || 

					(CA_GATHERSTART <= pTempCreature->GetLowerActionID() && pTempCreature->GetLowerActionID() <= CA_GATHEREND) ||
					(CA_SOCIALSTART <= pTempCreature->GetLowerActionID() && pTempCreature->GetLowerActionID() <= CA_SOCIALEND) ||
			
					pTempCreature->GetLowerActionID() == CA_AIMEDSHOT1 || pTempCreature->GetLowerActionID() == CA_AIMEDSHOT2 || 
					pTempCreature->GetLowerActionID() == CA_AIMEDSHOT3 || pTempCreature->GetLowerActionID() == CA_OVERBASH1 || 
					pTempCreature->GetLowerActionID() == CA_OVERBASH2 || pTempCreature->GetLowerActionID() == CA_OVERBASH3 || 
					pTempCreature->GetLowerActionID() == CA_OVERBASH4 || pTempCreature->GetLowerActionID() == CA_OVERBASH5 || 
					pTempCreature->GetLowerActionID() == CA_TURNPUNCH1 || pTempCreature->GetLowerActionID() == CA_TURNPUNCH2 || 
					pTempCreature->GetLowerActionID() == CA_TURNPUNCH3 || pTempCreature->GetLowerActionID() == CA_COMBINATIONBLOW || 
					pTempCreature->GetLowerActionID() == CA_BACKSTAB || pTempCreature->GetLowerActionID() == CA_POWERDRAIN || 
					pTempCreature->GetLowerActionID() == CA_BASH || pTempCreature->GetLowerActionID() == CA_ROUNDSWING ) && 
					pTempCreature->GetLowerActionID() != CA_REST && pTempCreature->GetLowerActionID() != CA_SITDOWN && 
					pTempCreature->GetLowerActionID() != CA_STANDUP )
				{
					if ( fAttackActionRate > 0.5f )
                    {
						fAttackActionRate = 0.5f;
                    }

					if ( pTempModel->IsAddonActionFinished( 1 ) )
                    {
						pTempModel->AddMotion( "ATTACKED", 3, fAttackActionRate );
                    }
				}

				vector3 vecChrPos;

				pTempModel->GetPosition( vecChrPos );
				CSceneManager::AddChrLight( pTempModel, vecChrPos + vector3( 0.0f, 200.0f, 0.0f ), 0xffff2200, 400.0f, 6 );

				vector3		vecEffectPos = vecChrPos;
				vecEffectPos.y += 120.0f;

                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if ( !pSelfCreature )
                    return;

				CEffScript *test_script = new CEffScript;
				CSceneManager::AddEffectScript( test_script );
				test_script->GetScriptBinData( "attack1.esf" );
				test_script->SetStartPos( pSelfCreature->GetPosition()->x,
									      pSelfCreature->GetPosition()->y,
										  pSelfCreature->GetPosition()->z );
				test_script->SetEndPos( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z );

                CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
                if ( !pSelfModel )
                    return;

				test_script->SetChr( pSelfModel, pTempModel );

				// particle
				CX3DEffect *pEffect = new CX3DEffect;
				CSceneManager::AddEffect( pEffect );
				pEffect->Load( EFFECTPATH, "attack1_particle.eff" );
				pEffect->SetLoop( FALSE );				
				pEffect->SetCenter( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z );
				PlaySound( dwWeaponType, vecEffectPos );
			}
		}
	}			
}

void		CCharacterControl::RegistAttacked( DWORD dwSrcChrID, DWORD dwTarChrID, unsigned long dwWeaponType )
{
	for ( int cNode = 0 ; cNode < (int)m_AttackedList.size(); cNode ++ )
	{
		if ( m_AttackedList[ cNode ].m_dwSrcChrID == dwSrcChrID &&
		 	 m_AttackedList[ cNode ].m_dwTarChrID == dwTarChrID )
		{
			return ;
		}
	}

	CAttackedNode AddNode ;
	AddNode.m_dwSrcChrID   = dwSrcChrID ;
	AddNode.m_dwTarChrID   = dwTarChrID ;
	AddNode.m_dwWeaponType = dwWeaponType ;
	m_AttackedList.push_back( AddNode ) ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::CheckAttackedTiming()
void CCharacterControl::CheckAttackedTiming()
{
    for ( int i = (int)m_AttackedList.size() - 1; i >= 0; --i )
	{
        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_AttackedList[ i ].m_dwSrcChrID );
        
        if ( pTempCreature )
        {
            CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

            if ( pTempModel )
            {
                int nResult = CSceneManager::GetCharacterEvent( pTempModel, EVENT_HIT );

		        if ( nResult )
		        {
			        MakeAttacked( m_AttackedList[ i ].m_dwTarChrID, m_AttackedList[ i ].m_dwWeaponType );
		        }
            }
        }

		// edith erase 오류발생 할수있음.
		m_AttackedList.erase( m_AttackedList.begin()+ i );
	}
}

void CCharacterControl::CheckCameraShakeTiming()
{
}

void		CCharacterControl::SetDirectionforMouse( vector3 &vecDirection )
{
	vecDirection.Normalize() ;
	vector3 vecZero			= vector3( 1.0f, 0.0f, 0.0f ) ;
	float fMoveDirection	= vecDirection * vecZero ;

	if ( fMoveDirection >= 1.0f ) 
		fMoveDirection = 1.0f ;
	
	if ( fMoveDirection <= -1.0f )
		fMoveDirection = -1.0f ;
	
	fMoveDirection = acosf( fMoveDirection ) ;

	if ( vecDirection.z < 0.0f )
	{
		fMoveDirection = fMoveDirection ;
	}
	else
	{
		fMoveDirection = ( FLOAT_PHI * 2.0f ) - fMoveDirection ;
	}
	
	fMoveDirection -= FLOAT_PHI / 2.0f ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( pSelfModel )
    {
        pSelfModel->SetDirection( fMoveDirection );
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::PlayerProcessOperation()
void CCharacterControl::PlayerProcessOperation( BOOL bKeyAble )
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;
	if ( !bKeyAble )
	{
		if ( pSelfCreature->GetIsDead() && !pSelfCreature->GetLowerActing() && g_DeviceInput.GetIsLeftMouseDown() )
		{
			CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			pScene->GetRespawnDlg()->SetVisible( TRUE ) ;
			return ;
		}
		return;
	}


    if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
	{
		if (!pSelfCreature->GetRideFlag() && !pSelfCreature->GetIsDead() && (g_DeviceInput.KeyDown( 'C' ) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_REST)) )
		{
			InitGlobalAction();

            pSelfCreature->ToggleCrouch();

			SetKillCounter( 0 );
			SetClientExitCounter( 0 );
			SetLogoutCounter( 0 ) ;
			SetPortalCounter( 0 );
			SetSkillUseCounter( 0 );

            WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
                                  pSelfCreature->GetLowerActionID(),
                                  pSelfCreature->GetDirection(),
                                  *( pSelfCreature->GetPosition() ) );
			return;
		}
	}

    if ( pSelfCreature->GetIsDead() && !pSelfCreature->GetLowerActing() && g_DeviceInput.GetIsLeftMouseDown() )
	{
		CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		pScene->GetRespawnDlg()->SetVisible( TRUE ) ;

		return ;
	}
}

VOID		CCharacterControl::CharRespawn() 
{
	InitGlobalAction() ;
	SetKillCounter( 0 ) ;
	SetClientExitCounter( 0 );
	SetLogoutCounter( 0 ) ;
	SetPortalCounter( 0 ) ;
	SetSkillUseCounter( 0 ) ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

	if ( WrapSendCharRespawn( pSelfCreature->GetCharID() ) )
	{
        pSelfCreature->GetEffect()->DeleteBodyEffect() ;

		WrapSendMovingPacket( CA_RESPAWN, CA_RESPAWN, 
                              pSelfCreature->GetDirection(),
                              *( pSelfCreature->GetPosition() ) ) ;
	}
}

void		CCharacterControl::PlaySound( int nMethod,vector3 vecPos )
{	
	char strSoundFilename[ 256 ] ;	
	switch ( nMethod )
	{
		case 3:
		case 4:
			sprintf( strSoundFilename, "%s%s%d.wav", SOUNDFILEPATH, "Bowhit", rand() % 4 ) ;
			break;
		case 200: // 점프 착지
			sprintf( strSoundFilename, "%s%s",SOUNDFILEPATH,"jump2.wav" ) ;
			break;
		default:
			sprintf( strSoundFilename, "%s%s%d.mnd", SOUNDFILEPATH, "Attacked", rand() % 5 ) ;
			break;
	}
//	try
	{
		//새로삽입한코드 
		CSound3D* pSound3D ;
		if ( !CSoundMgr::_GetInstance()->GetSound3D( pSound3D, strSoundFilename ) ) return; 
		pSound3D->Play(); 
		D3DVECTOR Pos; 
		Pos.x = vecPos.x; Pos.y = vecPos.y; Pos.z = vecPos.z; 
		pSound3D->SetPosition( Pos ); 
		
		switch (nMethod)
		{
			case 3:
			case 4:
				pSound3D->SetMinDistance( 2000.0f ); 
				pSound3D->SetMaxDistance( 2500.0f );
				break;
			case 200:
				pSound3D->SetMinDistance( 1000.0f ); 
				pSound3D->SetMaxDistance( 2000.0f );
				break;
			default:
				pSound3D->SetMinDistance( 1000.0f ); 
				pSound3D->SetMaxDistance( 2000.0f );
				break;
		}
	}
/*
	catch( exception &e )
	{
		static bool bFirstShowMessage = false ;
		if ( bFirstShowMessage == false )
		{
			MessageBox( NULL, e.what(), 0, 0 ) ;
			bFirstShowMessage = true ;
		}
	}
*/
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::MakeAttackInfo()
void		CCharacterControl::MakeAttackInfo( DWORD dwEnemyChrID )
{	
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( pSelfModel )
        return;

    // HIDE 캐릭터가 공격하지 못하도록
	if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) || pSelfCreature->GetRideEnable() )
        return;

	float fAngle = ( -pSelfModel->GetDirection() ) - FLOAT_PHI / 2.0f;

    vector3		vecSelfDir( cosf( fAngle ), 0.0f, sinf( fAngle ) );
    vector3     vecEnemyDir;

	vector3		vecLens;
	matrix		matSelfChrPos;
	matSelfChrPos.MakeIdent();
	matSelfChrPos._41 = pSelfCreature->GetPosition()->x;
	matSelfChrPos._43 = pSelfCreature->GetPosition()->z;

	vector3		vecFirstAttackPos;

	unsigned long	dwDefenser[ 10 ];
	ZeroMemory( &dwDefenser, sizeof( unsigned long ) * 10 );

	int		nAttackCount = 0;

    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

    if ( vecCharID.size() < 2 )
        return;

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
            continue;

		// Hide 캐릭터를 때리지 못하도록
		if ( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) || pTempCreature->GetRideEnable() )
            continue;

		
        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

		if ( pTempModel && pTempCreature->GetCharID() == dwEnemyChrID )
		{			
			pTempModel->GetPosition( vecFirstAttackPos );

			float fAttackActionRate = rand() % 6;
			fAttackActionRate *= 0.1f;
			fAttackActionRate += 0.3f;

			if ( fAttackActionRate > 0.5f ) 
				fAttackActionRate = 0.5f;

			if ( pTempModel->IsAddonActionFinished( 1 ) )
				pTempModel->AddMotion( "ATTACKED", 3, fAttackActionRate );

			// 석상 체크
            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
            if ( !pTempData )
                continue;
        
            if ( pTempData->m_nChrPattern != 8 )		
			{
				CSceneManager::CharacterRandomPos( pTempModel, 10, vecSelfDir * 50.0f );
			}
			
			CSceneManager::AddChrLight( pTempModel, vecFirstAttackPos + vector3( 0.0f, 100.0f, 0.0f ), 0xffff2200, 400.0f, 6 );

			vector3		vecEffectPos = vecFirstAttackPos + vector3( 0.0f, 120.0f, 0.0f );

			CEffScript* test_script = new CEffScript;
			CSceneManager::AddEffectScript( test_script );
			test_script->GetScriptBinData( "attack1.esf" );

			test_script->SetStartPos( pSelfCreature->GetPosition()->x, 
									  pSelfCreature->GetPosition()->y,
									  pSelfCreature->GetPosition()->z );
			test_script->SetEndPos( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z );
			test_script->SetChr( pSelfModel, pTempModel );
			
			// particle
			CX3DEffect *pEffect = new CX3DEffect;
			CSceneManager::AddEffect( pEffect );
			pEffect->Load( EFFECTPATH, "attack1_particle.eff" );
			pEffect->SetLoop( FALSE );

			float fAttackAngle = pTempModel->GetDirection();
			pEffect->SetAxis( 0.0f, -( ( 180.0f * fAngle ) / 3.14159f ) + 90.0f + fAttackAngle, 0.0f );
			pEffect->SetCenter( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z );
			dwDefenser[ nAttackCount++ ] = pTempCreature->GetCharID();
			break;
		}
	}	

	if ( nAttackCount )
	{
		PlaySound( 0,vecFirstAttackPos );
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::MakeVisualAttackInfo()
void CCharacterControl::MakeVisualAttackInfo()
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return;

    float fAngle = ( -pSelfModel->GetDirection() ) - FLOAT_PHI / 2.0f;
	vector3 vecSelfDir( cosf( fAngle ), 0.0f, sinf( fAngle ) );
    vector3 vecEnemyDir;

	vector3		vecLens;
	float		fInterLens; 	
	matrix		matSelfChrPos;

	matSelfChrPos.MakeIdent();
	matSelfChrPos._41 = pSelfCreature->GetPosition()->x;
	matSelfChrPos._43 = pSelfCreature->GetPosition()->z;

	vector3		vecFirstAttackPos;

	unsigned long	dwDefenser[ 10 ];
	ZeroMemory( &dwDefenser, sizeof( unsigned long ) * 10 );
	int				nAttackCount = 0;
	unsigned long	dwType;

    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
    if ( vecCharID.size() < 2 )
        return;

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
            continue;

        if ( pTempCreature == pSelfCreature )
            continue;

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
        if ( !pTempModel )
            continue;

        if ( pTempCreature->GetIsDead() ) 
			continue;
		
		dwType = GetClientIdentity( pTempCreature->GetCharID() );
		if ( dwType == Creature::CT_MONSTER )
		{
			dwType = GetClientSummon( pTempCreature->GetCharID(), TRUE );

			if ( dwType != Creature::CT_MONSTER ) 
				continue;
		}
		
		vecLens		= *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
		vecLens.y	= 0.0f;
		fInterLens	= vecLens.GetLens();

		if ( fInterLens <= 1300.0f )
		{
			float fEnemyAngle = pTempModel->GetDirection() - FLOAT_PHI;
			vector3 vecDir;
			vecDir.x = cosf( fEnemyAngle );
			vecDir.z = sinf( fEnemyAngle );
			vecDir.y = 0.0f;

			vecDir.x = cosf( fAngle );
			vecDir.z = sinf( fAngle );
			vecDir.y = 0.0f;

			matrix	matDirChr;
			matDirChr      = matSelfChrPos;
			matDirChr._41 += vecDir.x;
			matDirChr._43 += vecDir.z;			

			matrix	matPos, matInv, matResult;
			matPos.YRotation( fEnemyAngle );
			matPos._41 = pTempCreature->GetPosition()->x;
			matPos._43 = pTempCreature->GetPosition()->z;

			matInv.Inverse( matPos );			
			matResult = matSelfChrPos * matInv;
			matDirChr = matDirChr * matInv;

			vecDir = matDirChr.GetLoc() - matResult.GetLoc();		

			vector3 vecMin = vector3( -35.0f, 0.0f, -55.0f );
			vector3 vecMax = vector3( 35.0f, 0.0f, 65.0f );

			if ( GetMonsterMinBox ) 
				vecMin = GetMonsterMinBox( pTempCreature->GetCharID() );

			if ( GetMonsterMaxBox ) 
				vecMax = GetMonsterMaxBox( pTempCreature->GetCharID() );

			if ( CylinderOBBIntersection( matResult.GetLoc(), vecDir, FLOAT_PHI / 3.0f, vecMax, vecMin, 200.0f ) )
			{	
				//fEnemyDirection=(-pTempModel->GetDirection()) - FLOAT_PHI / 2.0f;				

				if ( nAttackCount == 0 )	
				{
					pTempModel->GetPosition( vecFirstAttackPos );					
				}

				vecEnemyDir = vecLens.Normalized();

				float fInterAngle = vecEnemyDir * vecSelfDir;

				fInterAngle = acosf( fInterAngle );

				if ( fInterAngle > FLOAT_PHI / 3.0f )
					continue;

				float fAttackActionRate = rand() % 6;
				fAttackActionRate *= 0.1f;
				fAttackActionRate += 0.3f;

				if ( fAttackActionRate > 0.5f ) 
					fAttackActionRate = 0.5f;

				if ( pTempModel->IsAddonActionFinished( 1 ) )
	 				 pTempModel->AddMotion( "ATTACKED", 3, fAttackActionRate );

                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );

                if ( pTempData->m_nChrPattern != 8 )		// 석상 체크
				{
					CSceneManager::CharacterRandomPos( pTempModel, 10, vecSelfDir * 50.0f );
				}

				vector3		vecTargetLightPos = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
				vecTargetLightPos.Normalize();
				vecTargetLightPos = *( pSelfCreature->GetPosition() ) + vecTargetLightPos * 60.0f;			 	
				CSceneManager::AddChrLight( pTempModel, vecTargetLightPos + vector3( 0.0f, 100.0f, 0.0f ), 0xffff2200, 400.0f, 6 );

				vector3 vecEffectPos = vecTargetLightPos + vector3( 0.0f, 120.0f, 0.0f );

				CEffScript *test_script = new CEffScript;
				CSceneManager::AddEffectScript( test_script );				
				
				if ( m_SkillAttackValue ) 
				{
					test_script->GetScriptBinData( "attack2.esf" );	
				}
				else 
                {
					test_script->GetScriptBinData( "attack1.esf" );
				}
				
				test_script->SetStartPos( pSelfCreature->GetPosition()->x,
										  pSelfCreature->GetPosition()->y,
										  pSelfCreature->GetPosition()->z );

				test_script->SetEndPos( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z );

				test_script->SetChr( pSelfModel, pTempModel );

				// particle
				CX3DEffect *pEffect = new CX3DEffect;
				CSceneManager::AddEffect( pEffect );

				if ( m_SkillAttackValue )
				{
					pEffect->Load( EFFECTPATH, "attack2_particle.eff" );
					if ( m_SkillAttackValue > 0 )
						--m_SkillAttackValue;
				}
				else
				{
					pEffect->Load( EFFECTPATH, "attack1_particle.eff" );
				}
				
				pEffect->SetLoop( FALSE );							
				float fAttackAngle = 0.0f;
				pEffect->SetAxis( 0.0f,-( ( 180.0f * fAngle ) / 3.14159f ) + 90.0f + fAttackAngle, 0.0f );
				pEffect->SetCenter( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z );				
				
				dwDefenser[ ++nAttackCount ] = pTempCreature->GetCharID();
				PlaySound( 0, vecFirstAttackPos );
				
				if ( nAttackCount >= 10 ) 
					break;
			}
		}
	}	
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::CheckMonsterForward()
BOOL CCharacterControl::CheckMonsterForward()
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return false;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return false;

	float fAngle = ( -pSelfModel->GetDirection() ) - FLOAT_PHI / 2.0f;

	vector3		vecLens, vecSelfDir;
	vecSelfDir.x = cosf( fAngle );
	vecSelfDir.z = sinf( fAngle );
	vecSelfDir.y = 0.0f;

	matrix		matSelfChrPos;
	matSelfChrPos.MakeIdent();
	matSelfChrPos._41 = pSelfCreature->GetPosition()->x;
	matSelfChrPos._43 = pSelfCreature->GetPosition()->z;

    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
		if ( NULL == pTempCreature )
            continue;

        if ( pTempCreature == pSelfCreature )
            continue;

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
		if ( NULL == pTempModel ) 
			continue;

        if ( pTempCreature->GetIsDead() ) 
			continue;

        if ( GetClientEnemyType( pTempCreature->GetCharID() ) != EnemyCheck::EC_ENEMY )
			continue;

        vecLens = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
		vecLens.y = 0.0f;

		if ( vecLens.GetLens() <= 400.0f )
		{
			vecLens.Normalize();
			float fDot = vecSelfDir * vecLens;
			float fDot2 = acosf( fDot );

			if ( fabs( fDot2 ) <= ( FLOAT_PHI / 4.0f ) )
			{
 				return TRUE;
			}
		}
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::GetAttacking()
BOOL CCharacterControl::GetAttacking()
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return false;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return false;

    unsigned ulUpperActionID = pSelfCreature->GetUpperActionID();

	if ( ulUpperActionID == CA_ATTACK || ulUpperActionID == CA_ATTACK2 || ulUpperActionID == CA_ATTACK3 ||
		 ulUpperActionID == CA_ATTACK4 || ulUpperActionID == CA_ATTACK5 || ulUpperActionID == CA_ATTACK6 ||
		 ulUpperActionID == CA_ATTACKADVANCE || ulUpperActionID == CA_ATTACKLEFT || ulUpperActionID == CA_ATTACKRIGHT ||
		 ulUpperActionID == CA_ATTACKRETREAT || ulUpperActionID == CA_MOVEATTACK || ulUpperActionID == CA_SHOT || ulUpperActionID == CA_JUMPSHOT ||
		 ulUpperActionID == CA_JUMPATTACK || ulUpperActionID == CA_JUMPATTACK2 || ulUpperActionID == CA_JUMPATTACK3 ||
		 ulUpperActionID == CA_CASTING || ulUpperActionID == CA_FIRE1 || ulUpperActionID == CA_FIRE2 ||

		 (CA_GATHERSTART <= ulUpperActionID && ulUpperActionID <= CA_GATHEREND) ||
		 (CA_SOCIALSTART <= ulUpperActionID && ulUpperActionID <= CA_SOCIALEND) ||

		 ulUpperActionID == CA_AIMEDSHOT1 || ulUpperActionID == CA_AIMEDSHOT2 || ulUpperActionID == CA_AIMEDSHOT3 ||
		 ulUpperActionID == CA_OVERBASH1 || ulUpperActionID == CA_OVERBASH2 || ulUpperActionID == CA_OVERBASH3 || ulUpperActionID == CA_OVERBASH4 ||	ulUpperActionID == CA_OVERBASH5 ||
		 ulUpperActionID == CA_TURNPUNCH3 ||	ulUpperActionID == CA_COMBINATIONBLOW || ulUpperActionID == CA_BACKSTAB ||
		 ulUpperActionID == CA_POWERDRAIN || ulUpperActionID == CA_BASH || ulUpperActionID == CA_ROUNDSWING )
	{
		return TRUE;
	}

	return FALSE;
}

void	CCharacterControl::InitGlobalAction()
{
	SetAutoRun( FALSE ) ;
	m_bAutoFollowMode			= FALSE ;
	m_dwAutoFollowChrID			= 0xFFFFFFFF ;
	m_bAutoTargetAttack			= false ;
	m_dwAutoTargetID			= 0xFFFFFFFF ;
	m_dwClickModeAttackTargetID = 0xFFFFFFFF ;
	m_bClickAttack				= false ;
	m_bClickMove				= false ;
	m_lAutoRunCounter			= 0 ;
}

void	CCharacterControl::SetPreDuelTarget( unsigned long dwDuelTargetID, unsigned char cVSType )
{
	map<unsigned long, unsigned char>::iterator it = m_lstPreDuelTargetID.find(dwDuelTargetID);
	if (it == m_lstPreDuelTargetID.end())
	{
		m_lstPreDuelTargetID.insert(make_pair(dwDuelTargetID, cVSType));
	}
	else
	{
		it->second |= cVSType;
	}
}

void	CCharacterControl::SetDuelTarget( void )
{
	m_lstDuelTargetID = m_lstPreDuelTargetID;
}

void	CCharacterControl::DeleteDuelTarget( unsigned long dwDuelTargetID )
{
	m_lstDuelTargetID.erase(dwDuelTargetID);
}

void	CCharacterControl::InitDuelTarget( void )
{
	m_lstPreDuelTargetID.clear() ;
	m_lstDuelTargetID.clear() ;
}

unsigned char	CCharacterControl::GetIsDuelTarget( unsigned long dwChrID )
{
	map<unsigned long, unsigned char>::iterator it = m_lstDuelTargetID.find(dwChrID);
	if (it != m_lstDuelTargetID.end())
	{
		return it->second;
	}

	return 0;
}

// ----------------------------------------------------------------------------------
// 공성 관련

VOID		 CCharacterControl::SetWeaponControlKind( CONTROL_KIND Control ) 
{
	m_WeaponControlKind = Control ; 

	switch ( m_WeaponControlKind )
	{
		case CHARACTER_CONTROL :				// 캐릭터 컨트롤
			m_pRYLCharacterControl->RestoreCamera() ;
			break ;
		case LONG_ATTACKWEAPON_CONTROL :		// 원거리 공성병기 컨트롤
			m_pRYLLongAttackWeaponControl->SetModifySkileton() ;
			break ;
		case SHORT_ATTACKWEAPON_CONTROL :		// 근거리 공성병기 컨트롤
			m_pRYLShortAttackWeaponControl->SetModifySkileton() ;
			break ;
		case LONG_DEFENCEWEAPON_CONTROL :		// 원거리 수성병기 컨트롤
			m_pRYLLongDefenceWeaponControl->SetModifySkileton() ;
			break ;
		case SHORT_DEFENCEWEAPON_CONTROL :		// 근거리 수성병기 컨트롤
			m_pRYLShortDefenceWeaponControl->SetModifySkileton() ;
			break ;
		case DROPSHIP_CONTROL :					// 수송선 컨트롤
			m_pRYLDropshipControl->SetModifySkileton() ;
			break ;		
	}
}

VOID		CCharacterControl::UpdateCreatureControl( BOOL bKeyAble, BOOL bEdge, int MouseX,int MouseY ) 
{
	// edith 2009.08.12 말타면 2초동안 컨트롤 안되게 하기 위해 집어넣음.
	CRYLGameData*	pGame = CRYLGameData::Instance();
	if(pGame->m_dwRideTickCount != 0)
	{
		if(abs((long)GetTickCount() - (long)pGame->m_dwRideTickCount) < 2000)
			return;
		else
			pGame->m_dwRideTickCount = 0;
	}

	switch ( m_WeaponControlKind )
	{
	case CHARACTER_CONTROL :				// 캐릭터 컨트롤
		m_pRYLCharacterControl->UpdateControl( bKeyAble, bEdge, MouseX, MouseY ) ;
		break ;
	case SHORT_ATTACKWEAPON_CONTROL :		// 근거리 공성병기 컨트롤
		m_pRYLShortAttackWeaponControl->UpdateControl( bKeyAble, bEdge, MouseX, MouseY ) ;
		break ;
	case LONG_ATTACKWEAPON_CONTROL :		// 원거리 공성병기 컨트롤
		m_pRYLLongAttackWeaponControl->UpdateControl( bKeyAble, bEdge, MouseX, MouseY ) ;
		break ;
	case LONG_DEFENCEWEAPON_CONTROL :		// 원거리 수성병기 컨트롤
		m_pRYLLongDefenceWeaponControl->UpdateControl( bKeyAble, bEdge, MouseX, MouseY ) ;
		break ;
	case SHORT_DEFENCEWEAPON_CONTROL :		// 근거리 수성병기 컨트롤
		m_pRYLShortDefenceWeaponControl->UpdateControl( bKeyAble, bEdge, MouseX, MouseY ) ;
		break ;
	case DROPSHIP_CONTROL :					// 수송선 컨트롤
		m_pRYLDropshipControl->UpdateControl( bKeyAble, bEdge, MouseX, MouseY ) ;
		break ;
	}
}

VOID		CCharacterControl::ChangeSiegeModel( unsigned long dwChrID, 
												 const char *strName, 
												 vector3 vecChrPos, 
												 float fDirection, 
												 const char* szType )
{
	if ( !strcmp( szType, "LongSeigeWeapon" ) )
	{
		m_pRYLLongAttackWeaponControl->SetSiegeModel( dwChrID, strName, vecChrPos, fDirection ) ;
	}
	else if ( !strcmp( szType, "ShortSiegeWeapon" ) )
	{
		m_pRYLShortAttackWeaponControl->SetSiegeModel( dwChrID, strName, vecChrPos, fDirection ) ;
	}
	else if ( !strcmp( szType, "LongCastleWeapon" ) )
	{
		m_pRYLLongDefenceWeaponControl->SetSiegeModel( dwChrID, strName, vecChrPos, fDirection ) ;
	}
	else if ( !strcmp( szType, "ShortCastleWeapon" ) )
	{
		m_pRYLShortDefenceWeaponControl->SetSiegeModel( dwChrID, strName, vecChrPos, fDirection ) ;
	}
	else if ( !strcmp( szType, "DropShip" ) )
	{
		m_pRYLDropshipControl->SetSiegeModel( dwChrID, strName, vecChrPos, fDirection ) ;
	}
	else if ( !strcmp( szType, "Character" ) )
	{
		;

	}
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::SwitchModel()
HRESULT		CCharacterControl::SwitchModel( unsigned long dwCID,
											unsigned short wObjectType,
											unsigned char cState,
											unsigned char cUpgradeType,
											unsigned char cUpgradeStep ) 
{
	WORD wRealObjType = wObjectType ;
	
	if ( wObjectType == EMBLEM )
	{
		wRealObjType = wObjectType + ( cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
						+ ( cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
						+ cUpgradeStep ;
	}
	else 
	{
		wRealObjType = wObjectType 
						+ ( cState  * Siege::UPGRADE_KIND_NUM )
						+ cUpgradeStep ; 
	}

	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;

	if ( lpMonster )
	{
        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID );

        if ( pTempCreature )
        {
            CZ3DGeneralChrModel *pAddChrmodel = CSceneManager::AddCharacter( (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, "FACE01", "HAIR01" );
            CZ3DGeneralChrModel* pOldChrmodel = pTempCreature->GetCharModel();

            CSceneManager::m_CharacterManager.SetNodeModel( pOldChrmodel, pAddChrmodel );
            pTempCreature->SetCharModel( pAddChrmodel );
        }
	}

	return S_OK ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::ModelTransform()
//HRESULT CCharacterControl::ModelTransform( unsigned long ulCharID, const char* pstrGCMDSName )
//{
//    RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );
//    
//    if( pTempCreature )
//    {
//        CZ3DGeneralChrModel *pAddChrmodel = CSceneManager::AddCharacter( pstrGCMDSName, "FACE01", "HAIR01" );
//        CZ3DGeneralChrModel* pOldChrmodel = pTempCreature->GetCharModel();
//
//        CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLShortDefenceWeaponControl->GetCharacterDataNode(), TRUE ) ;
//        g_CharacterData.m_pRYLCharacterControl->SetCharacterDataNode( CSceneManager::GetCharacterDataNode( pChrCreature->GetCharModel() ) ) ;
//        CSceneManager::m_CharacterManager.DeleteElement( pChrCreature->GetCharModel() ) ;
//
//        CSceneManager::m_CharacterManager.SetNodeModel( pOldChrmodel, pAddChrmodel );
//        pTempCreature->SetCharModel( pAddChrmodel );
//
//        return S_OK;
//    }
//
//    return S_FALSE;
//}

//--------------------------------------------------------------------------------------------------------------------------
// CCharacterControl::SetNationToCastleNPC()
VOID		 CCharacterControl::SetNationToCastleNPC( unsigned long dwCastleID, unsigned char cNation )
{
	CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
	if ( !lpCastle )
	{
		return ;
	}

	NPCNode* pNpcNode = g_NPCList.GetFirstNPCNode() ;
	
	while ( NULL != pNpcNode )
	{
		if ( lpCastle->GetNameID() == pNpcNode->m_dwTownOrNameID && pNpcNode->m_bBelongToCastle )
		{
            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pNpcNode->m_dwUID );

            if ( pTempData )
            {
				pTempData->m_cNation = lpCastle->GetNation();
            }
		}

		pNpcNode = g_NPCList.GetNextNPCNode() ;
	}
}

void		 CCharacterControl::SwapElement( RYLCreature* pNode1, RYLCreature* pNode2 ) 
{
	vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
    if ( vecCharID.empty() )
        return;

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	//RYLCreature *pFindNode1, *pFineNode2 ;

	for (; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        
		if ( pNode1 == pTempCreature )
            pTempCreature = pNode2 ;

		else if ( pNode2 == pTempCreature )
			pTempCreature = pNode1 ;        
	}
}
