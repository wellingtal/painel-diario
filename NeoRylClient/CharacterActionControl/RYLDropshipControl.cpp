
#include <winsock2.h>

#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Creature/EnemyCheck.h>

#include "RYLDropshipControl.h"
#include "RYLObjectControl.h"
#include "WinInput.h"
#include "FrameTimer.h"
#include "BaseGraphicsLayer.h"
#include "RYLTimer.h"

#include "RYLCharacterDataManager.h"
#include "GMMemory.h"

#define ARMS_MAXSPEED		155.0f
#define ARMS_TIREANGLE		20.0f
#define ARMS_BODYANGLE		10.0f
#define ARMS_ROTATETIME		35.0f

#define ARMS_ONECIRCLE		360.0f
#define ARMS_TIREVELOCITY	1.0f

CRYLDropshipControl::CRYLDropshipControl( CCharacterControl* lpObjControl ) 
{
	m_lpObjControl = lpObjControl ;
	m_lpWingObject		= NULL ;
	m_lpSiegeModel		= NULL ;			// 공성병기 크리쳐

	m_EaseOutDivideInterpolation.Setup( 0, 100, 50 ) ;
	
	int nCount = 0 ;
	while( m_EaseOutDivideInterpolation.GetValue() <= 99.0f )
	{
		if ( nCount > 228 )		break ;
		m_EaseOutDivideInterpolation.Interpolate() ;
		m_byVelocity[ nCount ] = ( unsigned char )m_EaseOutDivideInterpolation.GetValue() ;
		nCount ++ ;
	}
	m_iVelocityControl			= 0 ;
	
	m_fTireSpeed				= 0.0f ;
	m_lRealTireRotateUpdate		= 0L ;		// Update Time 
	m_fRealTireAngle			= 0.0f ;	// 실제 회전각도
	
	m_lTireRotateUpdate			= 0L ;	    // 모델에 부착된 타이어 Update time
	m_fTireAngle				= 0.0f ;    // 모델에 부착된 타이어 회전각도

	m_fBodyAngle				= 0.0f ;	// 몸통 회전 

	m_fVelocity					= 1.0f ;
	m_lVelocityUpdate			= 0 ;
	m_lTireAngleUpdate			= 0 ;
	m_lRealAngleUpdate			= 0 ;

	/*
	{
		static unsigned long dwcount1 = 0 ;
		char buff[ 256 ] ;
		sprintf( buff, "CRYLDropshipControl : %d\n", dwcount1 ) ;
		OutputDebugString( buff ) ;
		++ dwcount1 ;
	}
	*/
}
	
CRYLDropshipControl::~CRYLDropshipControl() 
{
	/*
	{
		static unsigned long dwcount2 = 0 ;
		char buff[ 256 ] ;
		sprintf( buff, "~CRYLDropshipControl : %d\n", dwcount2 ) ;
		OutputDebugString( buff ) ;
		++ dwcount2 ;
	}
	*/
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLDropshipControl::SetModifySkileton()
VOID		CRYLDropshipControl::SetModifySkileton() 
{
	/*
	{
		static unsigned long dwcount3 = 0 ;
		char buff[ 256 ] ;
		sprintf( buff, "SetModifySkileton : %d\n", dwcount3 ) ;
		OutputDebugString( buff ) ;
		++ dwcount3 ;
	}
	*/

    CZ3DGeneralChrModel* pSiegeModel = m_lpSiegeModel->GetCharModel();
    if( !pSiegeModel )
        return;

	m_lpWingObject		= pSiegeModel->GetSkeletonByIndex( 0 ) ;
	m_lpWingObject->SetRotationModifierCount( 2 ) ;

	m_fVelocity	= 1.0f ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLDropshipControl::UpdateControl
HRESULT CRYLDropshipControl::UpdateControl( BOOL bKeyAble, BOOL bEdge, int MouseX,int MouseY ) 
{
	/*
	{
		static unsigned long dwcount4 = 0 ;
		char buff[ 256 ] ;
		sprintf( buff, "UpdateControl : %d\n", dwcount4 ) ;
		OutputDebugString( buff ) ;
		++ dwcount4 ;
	}
	*/

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return E_FAIL;

    if( m_lpObjControl->m_MouseClickMode == 0 )
	{
		return E_FAIL;
    }

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

    if( !pSelfModel || !pSelfData )
        return E_FAIL;

    CCharacterLightShadowManager::CCharacterDataNode CheckNode;

	float fAngle = pSelfModel->GetDirection();
    fAngle -= pSelfCreature->GetFixAngle();

    pSelfCreature->SetPosition( pSelfModel->GetPosition() );
    pSelfCreature->SetDirection( pSelfModel->GetDirection() );

	vector3 vecChrPos = pSelfModel->GetPosition();
	vecChrPos += vector3( 0.0f, 150.0f, 0.0f );

	float fChrDir = pSelfModel->GetDirection();
	fChrDir	= -fChrDir;
	fChrDir	-= FLOAT_PHI / 2.0f;

	vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
	vecChrToward.Normalize();

	// 왼쪽 노말
	vector3 vecChrLeft( cosf( fChrDir + FLOAT_PHI / 2.0f ), 0.0f, sinf( fChrDir + FLOAT_PHI / 2.0f ) );

	static BOOL bSpaceKeyDown = FALSE ;
	static bool bReserves	  = false ;
	if ( bKeyAble && KeyPressed( VK_SPACE ))
	{		
		bSpaceKeyDown = TRUE;
	} 
	else 
	{
		bSpaceKeyDown = FALSE;
	}

	m_lpObjControl->CheckAttackedTiming();
	m_lpObjControl->CheckCameraShakeTiming();

    if( true == pSelfCreature->Landing( m_lpObjControl ) )
	{
		pSelfCreature->InitCharAction();
		if( m_lpObjControl->ChangeWeapon ) 
			m_lpObjControl->ChangeWeapon();
	}

    bool	m_bKeyPress = false;
	
	static float fTireAngle	= 0.0f ;
	static float fRealAngle = 0.0f ;
	float	fRunFactor  = CFrameTimer::GetUpdateTimer( m_lpObjControl->m_dwRunFactorTimer );
	float	fUpdate		= CFrameTimer::GetUpdateTimer( GRYLTimer.GetTireTimerID() );
	float	fUpdate2	= CFrameTimer::GetUpdateTimer( GRYLTimer.GetSceneManagerTimerID() ) ;

    if( bKeyAble && !pSelfCreature->GetIsDead() )
	{                                                   
        CollisionType   CT      = CSceneManager::GetCharacterCollisionType( pSelfModel );
		bool            bSpace  = false;

		CheckNode = CSceneManager::GetCharacterDataNode( pSelfModel );		

		//=============================================================================
		// 스킬 유지
		unsigned long dwFunction;
        if( pSelfCreature->GetStillCasting() && m_lpObjControl->CheckStillCasting( dwFunction ) )
		{
			m_bKeyPress = true;

			if( pSelfCreature->Casting() )
			{
				m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
									                  pSelfCreature->GetLowerActionID(),
                                                      pSelfCreature->GetDirection(),
                                                      *( pSelfCreature->GetPosition() ) );
			}
		}
		//=============================================================================

		//=============================================================================
		// 스킬 발동
        if( !m_bKeyPress && CT != CT_WATER && !pSelfCreature->GetSitMode() )
		{
            if( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
			{
				if( m_lpObjControl->CheckSkillStart && m_lpObjControl->CheckSkillStart( TRUE ) 
                  && ( pSelfCreature->GetLowerActionID() != CA_LEFTDASH 
                    && pSelfCreature->GetLowerActionID() != CA_RIGHTDASH 
                    && pSelfCreature->GetLowerActionID() != CA_BACKDASH ) )
				{
					m_bKeyPress = true;
                    pSelfCreature->SetStillCasting( true );

					// 이 스킬이 인스턴트면 바로 발동 된다.
					m_lpObjControl->m_dwStartSkillTick		= timeGetTime();
					m_lpObjControl->m_dwCastingSkillTick	= 0;
					m_lpObjControl->m_dwCastingSkillGrade	= 0;
				}
			}
		}
        //=============================================================================
        if( !m_bKeyPress )
		{
            if( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold )
             && !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
			{   
				if ( g_DeviceInput.GetIsRightMousePress() )
				{
					vector3 vecChrPos = pSelfModel->GetPosition();

					float fLimits = 15000.0f ;

					if ( vecChrPos.y >= fLimits )
					{
						vecChrPos.y = fLimits ;
					}
					else
					{
						vecChrPos.y += 10.0f ;
					}
					pSelfCreature->SetPosition( vecChrPos ) ;
					pSelfModel->SetPosition( vecChrPos ) ;
				}
				
				if ( g_DeviceInput.GetIsLeftMousePress() )
				{
					vector3 vecChrPos = pSelfModel->GetPosition() ;
					pSelfCreature->SetPosition( vecChrPos ) ;
					vector3 vecHightField = vector3( vecChrPos.x, 0.0f, vecChrPos.z ) ;
					vecHightField.y = CSceneManager::m_HeightField.GetHeight( vecHightField ) ;

					if ( vecChrPos.y <= vecHightField.y )
					{
						vecChrPos.y = vecHightField.y ;
					}
					else
					{
						vecChrPos.y -= 10.0f ;
					}

					pSelfCreature->SetPosition( vecChrPos ) ;
					pSelfModel->SetPosition( vecChrPos ) ;
				}

				if( KeyPressed( 'W' ) && KeyPressed( 'A' ) )
				{
					bReserves = false ; 
					m_bKeyPress = true;
					//============================================================================================================
					// 현재 Speed 계산
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
					m_lVelocityUpdate += fUpdate ;

					if ( m_lVelocityUpdate > ARMS_MAXSPEED )
					{
						m_fVelocity		   = ARMS_MAXSPEED ; 
						m_lVelocityUpdate = ARMS_MAXSPEED ;
					}
					else
						m_fVelocity  = (( float )m_lVelocityUpdate / ARMS_MAXSPEED ) * ARMS_MAXSPEED ;
					//============================================================================================================

					//============================================================================================================
					// 바퀴 각도 
					m_lTireAngleUpdate -= fUpdate2 ;
					if ( m_lTireAngleUpdate <= -ARMS_ROTATETIME )
					{
						m_fTireAngle = -ARMS_TIREANGLE ;
						m_lTireAngleUpdate = -ARMS_ROTATETIME ;
					}
					else
					{
						m_fTireAngle = (( float )m_lTireAngleUpdate / ARMS_ROTATETIME ) * ARMS_TIREANGLE ;
					}

					// 좌우 회전
					m_lpWingObject->SetRotationModifier( 0, vector3( 0, 0, 0 ), vector3( 0, 0, 1 ), m_fTireAngle );

					m_lRealAngleUpdate -= fUpdate2 ;
					if ( m_lRealAngleUpdate <= -ARMS_ONECIRCLE )
						m_lRealAngleUpdate = 0 ; 
					else
						m_fRealTireAngle = (( float )m_lRealAngleUpdate / ARMS_ONECIRCLE ) * ARMS_ONECIRCLE ;
					
					pSelfCreature->SetFixAngle( D3DXToRadian( m_fRealTireAngle ) ) ;
					fAngle += D3DXToRadian( m_fRealTireAngle ) ;
					pSelfModel->SetDirection( fAngle ) ;
				} 
				else if( KeyPressed( 'W' ) && KeyPressed( 'D' ) )
				{
					bReserves = false ;
					m_bKeyPress = true;
					//============================================================================================================
					// 현재 Speed 계산
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
					m_lVelocityUpdate += fUpdate ;

					if ( m_lVelocityUpdate > ARMS_MAXSPEED )
					{
						m_fVelocity		   = ARMS_MAXSPEED ;
						m_lVelocityUpdate = ARMS_MAXSPEED ;
					}
					else
						m_fVelocity  = (( float )m_lVelocityUpdate / ARMS_MAXSPEED ) * ARMS_MAXSPEED ;

					// 앞으로 회전
					//============================================================================================================

					//============================================================================================================
					// 바퀴 각도 
					m_lTireAngleUpdate += fUpdate2 ;
					if ( m_lTireAngleUpdate >= ARMS_ROTATETIME )
					{
						m_fTireAngle = ARMS_TIREANGLE ;
						m_lTireAngleUpdate = ARMS_ROTATETIME ;
					}
					else
					{
						m_fTireAngle = (( float )m_lTireAngleUpdate / ARMS_ROTATETIME ) * ARMS_TIREANGLE ;
					}
	
					// 좌우 회전
					m_lpWingObject->SetRotationModifier( 0, vector3( 0, 0, 0 ), vector3( 0, 0, 1 ), m_fTireAngle );
					//=============================================================================================================
					
					m_lRealAngleUpdate += fUpdate2 ;
					if ( m_lRealAngleUpdate >= ARMS_ONECIRCLE )
						m_lRealAngleUpdate = 0 ; 
					else
						m_fRealTireAngle = (( float )m_lRealAngleUpdate / ARMS_ONECIRCLE ) * ARMS_ONECIRCLE ;

					pSelfCreature->SetFixAngle( D3DXToRadian( m_fRealTireAngle ) ) ;
					fAngle += D3DXToRadian( m_fRealTireAngle ) ;
					pSelfModel->SetDirection( fAngle ) ;
				}
				else if ( KeyPressed( 'W' ) )
				{
					bReserves = false ;
					m_bKeyPress = true;
					//============================================================================================================
					// 현재 Speed 계산
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
					m_lVelocityUpdate += fUpdate ;

					if ( m_lVelocityUpdate > ARMS_MAXSPEED )
					{
						m_fVelocity		   = ARMS_MAXSPEED ;
						m_lVelocityUpdate = ARMS_MAXSPEED ;
					}
					else
					{
						m_fVelocity  = (( float )m_lVelocityUpdate / 120.0f ) * ARMS_MAXSPEED ;
					}
					//============================================================================================================

					//============================================================================================================
					// 바퀴 각도 
					if ( m_fTireAngle > 0.0f )						m_fTireAngle -= 1.0f ;
					if ( m_fTireAngle < 0.0f )						m_fTireAngle += 1.0f ;

					if ( m_lTireAngleUpdate > 0.0f )
						m_lTireAngleUpdate -= fUpdate2 ;
					else
						m_lTireAngleUpdate += fUpdate2 ;

					// 좌우 회전
					m_lpWingObject->SetRotationModifier( 0, vector3( 0, 0, 0 ), vector3( 0, 0, 1 ), m_fTireAngle );
				}
				else if ( KeyPressed( 'A' ) )
				{
					bReserves = false ;
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_RIGHT, fRunFactor );
					m_lTireAngleUpdate -= fUpdate2 ;
					if ( m_lTireAngleUpdate <= -ARMS_ROTATETIME )
					{
						m_fTireAngle = -ARMS_TIREANGLE ;
						m_lTireAngleUpdate = -ARMS_ROTATETIME ;
					}
					else
					{
						m_fTireAngle = (( float )m_lTireAngleUpdate / ARMS_ROTATETIME ) * ARMS_TIREANGLE ;
					}

					m_lVelocityUpdate += fUpdate ;
					if ( m_lVelocityUpdate > ARMS_MAXSPEED )
					{
						m_fVelocity		   = ARMS_MAXSPEED ;
						m_lVelocityUpdate = ARMS_MAXSPEED ;
					}
					else
					{
						m_fVelocity  = (( float )m_lVelocityUpdate / 120.0f ) * ARMS_MAXSPEED ;
					}

					// 좌우 회전
					m_lpWingObject->SetRotationModifier( 0, vector3( 0, 0, 0 ), vector3( 0, 0, 1 ), m_fTireAngle );
				}
				else if ( KeyPressed( 'D' ) )
				{
					bReserves = false ;
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_LEFT, fRunFactor );
					m_lTireAngleUpdate += fUpdate2 ;
					if ( m_lTireAngleUpdate >= ARMS_ROTATETIME )
					{
						m_fTireAngle = ARMS_TIREANGLE ;
						m_lTireAngleUpdate = ARMS_ROTATETIME ;
					}
					else
					{
						m_fTireAngle = (( float )m_lTireAngleUpdate / ARMS_ROTATETIME ) * ARMS_TIREANGLE ;
					}
					
					m_lVelocityUpdate += fUpdate ;
					if ( m_lVelocityUpdate > ARMS_MAXSPEED )
					{
						m_fVelocity		   = ARMS_MAXSPEED ;
						m_lVelocityUpdate = ARMS_MAXSPEED ;
					}
					else
					{
						m_fVelocity  = (( float )m_lVelocityUpdate / 120.0f ) * ARMS_MAXSPEED ;
					}
	
					// 좌우 회전
					m_lpWingObject->SetRotationModifier( 0, vector3( 0, 0, 0 ), vector3( 0, 0, 1 ), m_fTireAngle );
				}
                // by ichabod
				// 슬로우 상태일때는 Dash 불가능.
                if( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Slow ) )
				{
					if( KeyPressed( VK_SPACE ) && !bSpace && CT != CT_WATER && CheckNode.m_DelayTimer == 0 )
					{
						if( CT != CT_NONE && CT != CT_CHRSIDE )
						{
							m_bKeyPress = true;

							if( pSelfCreature->Jump() )
							{
								if( g_DeviceInput.KeyHold( 'W' ) )
                                {
                                    pSelfCreature->SetJumpAttackVector( vecChrToward * ( m_lpObjControl->m_fRunSpeed / 35.0f ) );
                                }
								else if( g_DeviceInput.KeyHold( 'S' ) )
                                {
                                    pSelfCreature->SetJumpAttackVector( vecChrToward * -( 150.0f / 35.0f ) );
                                }
								else
                                {
                                    pSelfCreature->SetJumpAttackVector( vector3( 0.0f, 0.0f, 0.0f ) );
                                }
							}

							m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
												                  pSelfCreature->GetLowerActionID(),
                                                                  pSelfCreature->GetDirection(),
                                                                  *( pSelfCreature->GetPosition() ) );
						}				
					}
				}
            }

            if( !m_lpObjControl->m_bSummonCmd )
			{
                if( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
				{
					if( m_lpObjControl->m_bClientUsed == false && KeyPressed( VK_LBUTTON ) && CT != CT_WATER )
					{
						m_bKeyPress = true;

						if( pSelfCreature->Attack() )
						{
							m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
												                  pSelfCreature->GetLowerActionID(),
												                  pSelfCreature->GetDirection(),
                                                                  *( pSelfCreature->GetPosition() ) );
						}
					}

					if( m_lpObjControl->CheckAttackable() && bSpaceKeyDown && m_lpObjControl->m_bClientUsed == true && m_lpObjControl->m_bAttackable && g_DeviceInput.GetIsLeftMousePress() && CT != CT_WATER )
					{
						if( m_lpObjControl->m_bAttackable )
						{
							m_bKeyPress		= true;
							m_lpObjControl->m_bAttackResult = TRUE;

							if( pSelfCreature->Attack() )
							{
								m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
													                  pSelfCreature->GetLowerActionID(),
													                  pSelfCreature->GetDirection(),
                                                                      *( pSelfCreature->GetPosition() ) );
							}
						} 
						else
						{
							m_lpObjControl->m_bAttackResult = FALSE;
						}
					}
				}
			}
		}
	}

    if( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) || pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
	{
		if( m_lpObjControl->SetAutoRun ) 
			m_lpObjControl->SetAutoRun( FALSE );

		m_lpObjControl->m_bAutoFollowMode	= FALSE;
		m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
	}

	if( !m_bKeyPress )
	{
		m_lpObjControl->PlayerProcessOperation( bKeyAble );
		
		if( m_lpObjControl->GetAutoRun && m_lpObjControl->GetAutoRun() )
		{
			m_bKeyPress = true;
            pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
			m_lpObjControl->SetKillCounter( 0 );
			m_lpObjControl->SetClientExitCounter( 0 );
			m_lpObjControl->SetLogoutCounter( 0 ) ;
			m_lpObjControl->SetPortalCounter( 0 );
			m_lpObjControl->SetSkillUseCounter( 0 );
		}

		if( m_lpObjControl->m_bAutoFollowMode )
		{
            RYLCreature* pFollowCreature = RYLCreatureManager::Instance()->GetCreature( m_lpObjControl->m_dwAutoFollowChrID );

            if( pFollowCreature )
            {
                vector3 vecLen = *( pFollowCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
				vecLen.y	   = 0.0f;

				if( vecLen.GetLens() > 700.0f )
				{
					m_lpObjControl->SetDirectionforMouse( vecLen );

                    pSelfCreature->SetDirection( pSelfModel->GetDirection() );
					fChrDir = pSelfModel->GetDirection();

					m_bKeyPress = true;
                    pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
				}
			} 
			else
			{
				m_lpObjControl->m_bAutoFollowMode	= FALSE;
				m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
			}
		}
	}
	else
	{
		m_lpObjControl->SetKillCounter( 0 );
		m_lpObjControl->SetClientExitCounter( 0 );
		m_lpObjControl->SetLogoutCounter( 0 ) ;
		m_lpObjControl->SetPortalCounter( 0 );
		m_lpObjControl->SetSkillUseCounter( 0 );
		
		if( m_lpObjControl->SetAutoRun ) 
			m_lpObjControl->SetAutoRun( FALSE );

		m_lpObjControl->m_bAutoFollowMode	= FALSE;
		m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
	}

    // 스킬 해제
	unsigned long dwFunction;
    if( pSelfCreature->GetStillCasting() && !m_lpObjControl->CheckStillCasting( dwFunction ) )
	{
		m_lpObjControl->m_bSkillExecuted = TRUE;
	}

	if( !m_bKeyPress )
	{
        if( !pSelfCreature->GetIsDead() )
		{
			pSelfCreature->Wait();
		}
	}

	if( !m_lpObjControl->GetAttacking() )
	{
		if( m_lpObjControl->SetChangeWeapon )
			m_lpObjControl->SetChangeWeapon( FALSE );
	}

    switch ( pSelfCreature->GetLowerActionID() )
	{
		case CA_RUN :
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * ( m_fVelocity / 35.0f ) ) ;
			break ;
		case CA_WALKLEFT:
            CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * -( m_fVelocity / 35.0f ) );
        	break;

		case CA_WALKRIGHT:
            CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * ( m_fVelocity / 35.0f ) );
        	break;

		default:
			{
				if ( !m_bKeyPress )
				{
					float fRotSpeed = ( m_fVelocity / ARMS_MAXSPEED ) * 10.0f ;
					m_fTireSpeed += fRotSpeed ;
					
					if ( m_fTireSpeed >= 360 )
						m_fTireSpeed = ( ( float )( ( int )m_fTireSpeed % 360 ) )  ;

					float fTempSped = m_fTireSpeed ; 

					if ( m_fVelocity < 0.0f )		// 후진일 경우 감속
					{
						m_fVelocity += 20.0f ;
						//fTempSped = -m_fTireSpeed ;
					}
										
					if ( m_fVelocity > 0.0f )		// 전진일 경우 감속
					{
						m_fVelocity -= 20.0f ;
						//fTempSped = m_fTireSpeed ;
					}

					if ( m_fVelocity <= 20.0f && m_fVelocity >= -20.0f )
					{
						m_fVelocity = 0 ;
						fTempSped = m_fTireSpeed ;
					}

					CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * ( m_fVelocity / 35.0f ) ) ;
					// 앞으로 회전

					if ( m_fTireAngle > 0.0f )						m_fTireAngle -= 1.0f ;
					if ( m_fTireAngle < 0.0f )						m_fTireAngle += 1.0f ;
					// 좌우 회전
					m_lpWingObject->SetRotationModifier( 0, vector3( 0, 0, 0 ), vector3( 0, 0, 1 ), m_fTireAngle );
				}
			}
				
			break ;
	}

    if( CFrameTimer::GetUpdateTimer( m_lpObjControl->m_nCharMovingPacketTimerID ) >= 1.0f ) // && pSelfCreature->IsCancelAction(2))
	{
		if( pSelfCreature->GetLowerActionID() == CA_WALK
		 || pSelfCreature->GetLowerActionID() == CA_RUN
		 || pSelfCreature->GetLowerActionID() == CA_WALKBACK
		 || pSelfCreature->GetLowerActionID() == CA_WALKLEFT
		 || pSelfCreature->GetLowerActionID() == CA_WALKRIGHT
		 || pSelfCreature->GetLowerActionID() == CA_REST
		 || pSelfCreature->GetLowerActionID() == CA_WAIT
		 || pSelfCreature->GetLowerActionID() == CA_FIRE1
		 || pSelfCreature->GetLowerActionID() == CA_FIRE2

		 || (CA_GATHERSTART <= pSelfCreature->GetLowerActionID() && pSelfCreature->GetLowerActionID() <= CA_GATHEREND)
		 || (CA_SOCIALSTART <= pSelfCreature->GetLowerActionID() && pSelfCreature->GetLowerActionID() <= CA_SOCIALEND)

		 || pSelfCreature->GetLowerActionID() == CA_SHOT
		 || pSelfCreature->GetLowerActionID() == CA_JUMPSHOT
		 || pSelfCreature->GetLowerActionID() == CA_FORWARDJUMP
		 || pSelfCreature->GetLowerActionID() == CA_BACKJUMP
		 || pSelfCreature->GetLowerActionID() == CA_JUMP
		 || pSelfCreature->GetLowerActionID() == CA_FRONTDASH
		 || pSelfCreature->GetLowerActionID() == CA_JUMPATTACK
		 || pSelfCreature->GetLowerActionID() == CA_JUMPATTACK2
		 || pSelfCreature->GetLowerActionID() == CA_FALLDOWN )
		{
			vecChrToward.Normalize();
			vecChrLeft.Normalize();

			vector3 vecMove;

			switch( pSelfCreature->GetLowerActionID() )
			{
				case CA_FORWARDJUMP:
				case CA_FRONTDASH:
				case CA_RUN:
                    {
					    vecMove = vecChrToward * ( m_lpObjControl->m_fRunSpeed / 4.0f );
                    }
					break;

				case CA_WALKBACK:
                    {
					    vecMove = vecChrToward * -( m_lpObjControl->m_fWalkSpeed / 4.0f );
                    }
					break;

				case CA_WALKLEFT:
                    {
					    vecMove = vecChrLeft * ( m_lpObjControl->m_fWalkSpeed / 4.0f );
                    }
					break;

				case CA_WALKRIGHT:
                    {
					    vecMove = vecChrLeft * -( m_lpObjControl->m_fWalkSpeed / 4.0f );
                    }
					break;

				case CA_WALK:
                    {
					    vecMove = vecChrToward * ( m_lpObjControl->m_fWalkSpeed / 4.0f );
                    }
					break;

				case CA_JUMPATTACK:
				case CA_JUMPATTACK2:
                    {
                        vecMove = pSelfCreature->GetJumpAttackVector();
                    }
					break;

				case CA_BACKJUMP:
                    {
					    vecMove = vecChrToward * -( m_lpObjControl->m_fWalkSpeed / 4.0f );
                    }
					break;

				default:
                    {
					    vecMove = vector3( 0.0f, 0.0f, 0.0f );
                    }
					break;
			}

            vecMove += *( pSelfCreature->GetPosition() );

			CollisionType CT = CSceneManager::GetCharacterCollisionType( pSelfModel );

			if( CT == CT_WATER )
			{
                m_lpObjControl->WrapSendMovingPacket( CA_SWIM, CA_SWIM, pSelfCreature->GetDirection(), vecMove );
			} 
			else
			{
                m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
									                  pSelfCreature->GetLowerActionID(),
                                                      pSelfCreature->GetDirection(), vecMove );
			}
		}
	}
    return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLDropshipControl::SetSiegeModel()
VOID		CRYLDropshipControl::SetSiegeModel( unsigned long dwChrID, const char *strName, vector3 vecChrPos, float fDirection ) 
{
	/*
	{
		static unsigned long dwcount5 = 0 ;
		char buff[ 256 ] ;
		sprintf( buff, "SetSiegeModel : %d\n", dwcount5 ) ;
		OutputDebugString( buff ) ;
		++ dwcount5 ;
	}
	*/
    m_lpSiegeModel = RYLCreatureManager::Instance()->AddSiegeCreature( m_lpSiegeModel, dwChrID, strName, vecChrPos, fDirection, "Dropship" );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLDropshipControl::SetCamera()
VOID    CRYLDropshipControl::SetCamera( int dx, int dy, int dz )
{	
	/*
	{
		static unsigned long dwcount6 = 0 ;
		char buff[ 256 ] ;
		sprintf( buff, "SetCamera : %d\n", dwcount6 ) ;
		OutputDebugString( buff ) ;
		++ dwcount6 ;
	}
	*/

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if( !pSelfModel )
        return;

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if( !pSelfData )
        return;

	// 카메라 각도
	if( m_lpObjControl->m_nCameraAnimation != 0 )
	{
		//카메라 애니메이션이 있을때 
		// 없을땐 그냥 캐릭터 조종모드로 전환

		return;
	}

	if( m_lpObjControl->m_bFreelook)  
	{
		D3DXMATRIX *matPosition = ( D3DXMATRIX* )CSceneManager::GetCamera()->GetMatPosition();
		vector3 vecBeforePos    = vector3( matPosition->_41, matPosition->_42, matPosition->_43 );
		float	fDeltaY = dx / 300.0f;
		float	fDeltaX = dy / 300.0f;
		
		D3DXVECTOR3 vecT( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vecR( 0.0f, 0.0f, 0.0f );

		D3DXMATRIX		matT, matR;
		D3DXQUATERNION	qR;	 	

		if( KeyPressed( VK_MENU ) )
			return;
		
		if( KeyPressed( VK_SHIFT ) )
		{
			if( KeyPressed( 'A' ) || KeyPressed( VK_LEFT ) )	vecT.x -= m_lpObjControl->m_fCameraSpeed; // Slide Left			
			if( KeyPressed( 'D' ) || KeyPressed( VK_RIGHT ) )	vecT.x += m_lpObjControl->m_fCameraSpeed; // Slide Right
			if( g_DeviceInput.GetIsRightMousePress() )			vecT.y += m_lpObjControl->m_fCameraSpeed; // Slide Down
			if( g_DeviceInput.GetIsLeftMousePress() )			vecT.y -= m_lpObjControl->m_fCameraSpeed; // Slide Up
			if( KeyPressed( 'W' ) )							    vecT.z += m_lpObjControl->m_fCameraSpeed; // Move Forward
			if( KeyPressed( 'S' ) )							    vecT.z -= m_lpObjControl->m_fCameraSpeed; // Move Backward	
			if( KeyPressed( VK_NUMPAD4 ) )						fDeltaY += 0.008f;
			if( KeyPressed( VK_NUMPAD6 ) )						fDeltaY -= 0.008f;
			if( KeyPressed( VK_NUMPAD8 ) )						fDeltaX -= 0.008f;
			if( KeyPressed( VK_NUMPAD2 ) )						fDeltaX += 0.008f;
		}
		else
		{
			if( KeyPressed( 'A' ) || KeyPressed( VK_LEFT ) )	vecT.x -= 10.0f;			// Slide Left
			if( KeyPressed( 'D' ) || KeyPressed( VK_RIGHT ) )	vecT.x += 10.0f;			// Slide Right
			if( g_DeviceInput.GetIsRightMousePress() )			vecT.y += 10.0f;			// Slide Down
			if( g_DeviceInput.GetIsLeftMousePress() )			vecT.y -= 10.0f;			// Slide Up
			if( KeyPressed( 'W' ) )
				vecT.z += 10.0f; // Move Forward
			if( KeyPressed( 'S' ) )
				vecT.z -= 10.0f; // Move Backward	

			if( KeyPressed( VK_NUMPAD4 ) )						fDeltaY += 0.008f; 
			if( KeyPressed( VK_NUMPAD6 ) )						fDeltaY -= 0.008f;
			if( KeyPressed( VK_NUMPAD8 ) )						fDeltaX -= 0.008f;
			if( KeyPressed( VK_NUMPAD2 ) )						fDeltaX += 0.008f;
		}	
		
		
		float	fRotationX = CSceneManager::GetCamera()->GetRotationX();
		float	fRotationY = CSceneManager::GetCamera()->GetRotationY();
		
		D3DXVECTOR3		vecPos;

		vecPos.x = matPosition->_41;
		vecPos.y = matPosition->_42;
		vecPos.z = matPosition->_43;
		
		D3DXMatrixIdentity( matPosition );

		fRotationX -= fDeltaX;
		fRotationY -= fDeltaY;

		D3DXQuaternionRotationYawPitchRoll( &qR,fRotationY, fRotationX, 0.0f );
		D3DXMatrixRotationQuaternion( &matR, &qR );
		D3DXMatrixMultiply( matPosition, &matR, matPosition );	
		
		matPosition->_41 = vecPos.x;
		matPosition->_42 = vecPos.y;
		matPosition->_43 = vecPos.z;

		D3DXMatrixTranslation( &matT, vecT.x, vecT.y, vecT.z );
		D3DXMatrixMultiply( matPosition, &matT, matPosition );		

		CSceneManager::GetCamera()->SetRotaitionX( fRotationX );
		CSceneManager::GetCamera()->SetRotaitionY( fRotationY );
		//

		vector3 vecCharPos;	
		CollisionType CT;
		vector3 vecOldPos	= vector3( matPosition->_41, matPosition->_42, matPosition->_43 );
		vecCharPos			= vecOldPos;
		vector3 vecDir		= vecOldPos - vecCharPos;

		vecCharPos = CSceneManager::m_CollisionDetection.GetPosition( vecOldPos, vecDir, vecBeforePos, CT, true );			
			
		matPosition->_41 = vecCharPos.x;
		matPosition->_42 = vecCharPos.y;
		matPosition->_43 = vecCharPos.z;
		//

		vector3 vecTempPos( CSceneManager::GetCamera()->GetMatPosition()->_41,
							CSceneManager::GetCamera()->GetMatPosition()->_42,
							CSceneManager::GetCamera()->GetMatPosition()->_43 );
        pSelfCreature->SetPosition( vecTempPos );

		CSceneManager::GetCamera()->SetVecPosition( vector3( CSceneManager::GetCamera()->GetMatPosition()->_41,
															 CSceneManager::GetCamera()->GetMatPosition()->_42,
															 CSceneManager::GetCamera()->GetMatPosition()->_43 ) );		

		matrix *matPos=CSceneManager::GetCamera()->GetMatPosition();
		matrix matInv;
		matInv.Inverse( *matPos );
		
		CSceneManager::GetCamera()->SetMatView( matInv );		
		CSceneManager::GetCamera()->MoveFrustum();
		return;
	}

	// m_MouseClickMode가 1일때는 전투모드
	if( m_lpObjControl->m_bClientUsed )
	{
		dx =- dx;
		dy =- dy;

		if( dy )
		{
			if( m_lpObjControl->m_MouseClickMode == 1 )
			{
				m_lpObjControl->m_Camera.m_fBattleCameraRotY += dy / 300.0f;
			}
			else
			{
				m_lpObjControl->m_Camera.m_fCameraRotY += dy / 300.0f;
			}
		}

		if( dz )
		{
			if( m_lpObjControl->m_MouseClickMode == 1 )
			{
				m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera += dz * 50.0f;
				
				if( m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera < 200.0f )  
					m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera = 200.0f;

				if( m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera > 7000.0f )
					m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera = 7000.0f;
			}
		}
	}
	
	vector3 vecChrPos = pSelfModel->GetPosition();

	m_lpObjControl->m_Camera.m_fCameraRotX += -m_fRealTireAngle;					
	float fInitY = 150.0f;
	
	if( m_lpObjControl->m_MouseClickMode == 1 )
	{
		{
			float	fChrDir  = pSelfModel->GetDirection();
			fChrDir -= FLOAT_PHI / 2.0f;
			m_lpObjControl->m_Camera.UpdateBattleCharacter( vecChrPos, fChrDir, fInitY );
		}
	}
	else
    {
		m_lpObjControl->m_Camera.UpdateClickMode( vecChrPos, fInitY );
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLDropshipControl::MakeAttackInfo()
bool    CRYLDropshipControl::MakeAttackInfo( unsigned short wSkill, unsigned char cSkillLock, unsigned char cSkillLevel, 
									      unsigned char cAtCount, float fAttackAngle, float fAttackRate, BOOL bAngle, BOOL bGod )
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return false;

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if( !pSelfData )
        return false;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if( !pSelfModel )
        return false;

	// Hide 캐릭터가 공격하지 못하도록
	if( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) || pSelfCreature->GetRideEnable() ) 
		return false;	

	float		fAngle = ( -pSelfModel->GetDirection() ) - FLOAT_PHI / 2.0f;
    vector3		vecSelfDir( cosf( fAngle ), 0.0f, sinf( fAngle ) );
    vector3     vecEnemyDir;

	vector3		vecLens;
	float		fInterLens; 	

	matrix		matSelfChrPos;
	matSelfChrPos.MakeIdent();
	matSelfChrPos._41 = pSelfCreature->GetPosition()->x;
    matSelfChrPos._43 = pSelfCreature->GetPosition()->z;

	vector3			vecFirstAttackPos;
	unsigned long	dwDefenser[ 10 ];
	int				nAttackCount = 0;
	unsigned long	dwType, dwNationType;
	ZeroMemory( &dwDefenser, sizeof( unsigned long ) * 10 );

	vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

    if( vecCharID.size() < 2 )
        return false;

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for(; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if( NULL == pTempCreature )
            continue;

		if( pTempCreature == pSelfCreature )
            continue;

        if( pTempCreature->GetIsDead() )
            continue;
	
		// Hide 캐릭터 때리지 못하게
		if( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) || pTempCreature->GetRideEnable() )
            continue;

        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
        if( NULL == pTempModel )
            continue;

        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
        if( NULL == pTempData )
            continue;

		/*
		// 공성 진지 OBB Collision
		bool bOBBEnable = pTempCreature->GetOBBEnable();
		if( bOBBEnable )
		{
			vector3 m_vExtent( 30, 85, 30 );
			pSelfCreature->GetOBB()->SetStaticParameter( m_vExtent );
			
			vector3 vecPos;
			matrix	matRot;

			matRot.MakeIdent();
			float fAngle = ( -pSelfModel->GetDirection() ) - FLOAT_PHI / 2.0f;
			matRot.YRotation( fAngle );

			vecPos = *( pSelfCreature->GetPosition() );
			vecPos.y += 85;

			pSelfCreature->GetOBB()->SetDynamicParameter( vecPos, matRot );
			
			CZ3DBoundingVolumeObject::_SetRenderFlag( true );

			if( pTempModel->CheckCollision( *( pSelfCreature->GetOBB() ) ) )
			{
				if( nAttackCount == 0 ) 	
				{
					pTempModel->GetPosition( vecFirstAttackPos );
				}

				float fAttackActionRate = rand() % 6;
				fAttackActionRate += 0.3f;
				fAttackActionRate *= 0.1f;

				if( fAttackActionRate > 0.5f) 
					fAttackActionRate = 0.5f;

				if( pTempModel->IsAddonActionFinished( 1 ) )
					pTempModel->AddMotion( "ATTACKED", 3, fAttackActionRate );

				if( pTempData->m_nChrPattern != 8 )		// 석상 체크
				{
					CSceneManager::CharacterRandomPos( pTempModel, 10, vecSelfDir * 50.0f );
				}

				vector3 vecTargetLightPos = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
				vecTargetLightPos.Normalize();
				vecTargetLightPos = *( pSelfCreature->GetPosition() ) + vecTargetLightPos * 60.0f;				
				CSceneManager::AddChrLight( pTempModel, vecTargetLightPos + vector3( 0.0f, 100.0f, 0.0f ), 0xffff2200, 400.0f, 6 );

				vector3 vecEffectPos = vecTargetLightPos+vector3( 0.0f, 120.0f, 0.0f );

				CEffScript *test_script = new CEffScript;
				CSceneManager::AddEffectScript( test_script );
				if( fAttackRate >= 1.0f || m_lpObjControl->m_SkillAttackValue ) 
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

				//particle
				CX3DEffect* pEffect = new CX3DEffect;
				CSceneManager::AddEffect( pEffect );
				if( fAttackRate >= 1.0f || m_lpObjControl->m_SkillAttackValue )
				{
					pEffect->Load( EFFECTPATH, "attack2_particle.eff" );
					
					if( m_lpObjControl->m_SkillAttackValue > 0 )
						m_lpObjControl->m_SkillAttackValue --;
				}
				else
				{
					pEffect->Load( EFFECTPATH, "attack1_particle.eff" );
				}
				pEffect->SetLoop( FALSE );				
				pEffect->SetAxis( 0.0f,-( ( 180.0f * fAngle ) / 3.14159f ) + 90.0f + fAttackAngle, 0.0f );
				pEffect->SetCenter(vecEffectPos.x,vecEffectPos.y,vecEffectPos.z );				
                dwDefenser[ nAttackCount++ ] = pTempCreature->GetCharID();
				if( nAttackCount >= 10 ) 
					break;
			}
		}
		*/

        dwType = m_lpObjControl->GetClientIdentity( pTempCreature->GetCharID() );

		if( dwType == Creature::CT_MONSTER )
		{
			dwType = m_lpObjControl->GetClientSummon( pTempCreature->GetCharID(), TRUE );

			if( dwType != Creature::CT_MONSTER ) 
				continue;
		}

		dwNationType = m_lpObjControl->GetClientEnemyType( pTempCreature->GetCharID() );

		if( dwType == Creature::CT_NPC || dwNationType != EnemyCheck::EC_ENEMY ) 
			continue;
		
		if( bGod && dwType == Creature::CT_PC && pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Invincible ) )
			continue;
		
		vecLens    = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );	
		vecLens.y  = 0.0f;
		fInterLens = vecLens.GetLens();

		vector3 vecMin  = vector3( -35.0f, 0.0f, -55.0f );
		vector3 vecMax  = vector3(  35.0f, 0.0f,  65.0f );
		vector3 vecMin2 = vector3( -35.0f, 0.0f,  65.0f );
		vector3 vecMax2 = vector3(  35.0f, 0.0f, -55.0f );

		if( dwType == Creature::CT_MONSTER )
		{
			if( m_lpObjControl->GetMonsterMinBox ) 
			{
				vecMax = m_lpObjControl->GetMonsterMinBox( pTempCreature->GetCharID() );
				vecMax *= -1.0f;

			}
			if( m_lpObjControl->GetMonsterMaxBox ) 
			{
				vecMin = m_lpObjControl->GetMonsterMaxBox( pTempCreature->GetCharID() );
				vecMin *= -1.0f;

			}
			if( m_lpObjControl->GetMonsterMaxBox || m_lpObjControl->GetMonsterMinBox )
			{
				vecMin2 = vector3( vecMin.x, 0.0f, vecMax.z );
				vecMax2 = vector3( vecMax.x, 0.0f, vecMin.z );

			}
		}

		float fEnemyAngle = pTempModel->GetDirection() - FLOAT_PHI;

		matrix	matPos, matInv, matResult;
		matPos.YRotation( fEnemyAngle );
		
		matPos._41 = pTempCreature->GetPosition()->x;
		matPos._43 = pTempCreature->GetPosition()->z;

		matInv.Inverse( matPos );			
		matResult = matSelfChrPos * matInv;

		vector3 vecBoxRad1	= matResult.GetLoc() - vecMax;
		float	fBoxRad1	= vecBoxRad1.GetLens();

		vector3 vecBoxRad2	= matResult.GetLoc() - vecMin;
		float	fBoxRad2	= vecBoxRad2.GetLens();
			
		vector3 vecBoxRad3  = matResult.GetLoc() - vecMax2;
		float	fBoxRad3	= vecBoxRad3.GetLens();

		vector3 vecBoxRad4  = matResult.GetLoc() - vecMin2;
		float	fBoxRad4	= vecBoxRad4.GetLens();
			
		vector3 vecResult	= matResult.GetLoc();

		if( ( fBoxRad1 <= 210.0f ) || ( fBoxRad2 <= 210.0f ) || ( fBoxRad3 <= 210.0f ) || ( fBoxRad4 <= 210.0f ) ||
			( ( vecMin.x <= vecResult.x ) && ( vecMin.z <= vecResult.z ) && ( vecMax.x >= vecResult.x ) && ( vecMax.z >= vecResult.z ) ) )
		{
			vector3 vecDir;
			vecDir.x = cosf( fEnemyAngle );
			vecDir.z = sinf( fEnemyAngle );
			vecDir.y = 0.0f;

			vecDir.x = cosf( fAngle );
			vecDir.z = sinf( fAngle );
			vecDir.y = 0.0f;

			matrix matDirChr;
			matDirChr	   = matSelfChrPos;
			matDirChr._41 += vecDir.x;
			matDirChr._43 += vecDir.z;			

			matDirChr = matDirChr * matInv;

			vecDir = matDirChr.GetLoc() - matResult.GetLoc();		

			if( !bAngle || ( bAngle && CylinderOBBIntersection( matResult.GetLoc(), vecDir, FLOAT_PHI / 3.0f, vecMax, vecMin, 1210.0f ) ) )
			{	
				//fEnemyDirection=(-m_lstCharData[cChr]->m_lpChrModel->GetDirection()) - FLOAT_PHI / 2.0f;				
				if( nAttackCount == 0 ) 	
				{
					pTempModel->GetPosition( vecFirstAttackPos );					
				}
				vecEnemyDir		  = vecLens.Normalized();						
				float fInterAngle = vecEnemyDir * vecSelfDir;
				fInterAngle		  = acosf( fInterAngle );

				if( bAngle && fInterAngle > FLOAT_PHI / 3.0f )
					continue;

				float fAttackActionRate = rand() % 6;
				fAttackActionRate += 0.3f;
				fAttackActionRate *= 0.1f;

				if( fAttackActionRate > 0.5f) 
					fAttackActionRate = 0.5f;

				if( pTempModel->IsAddonActionFinished( 1 ) )
					pTempModel->AddMotion( "ATTACKED", 3, fAttackActionRate );

                if( pTempData->m_nChrPattern != 8 )		// 석상 체크
				{					
					CSceneManager::CharacterRandomPos( pTempModel, 10, vecSelfDir * 50.0f );				
				}

				vector3 vecTargetLightPos = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
				vecTargetLightPos.Normalize();
				vecTargetLightPos = *( pSelfCreature->GetPosition() ) + vecTargetLightPos * 60.0f;				
				CSceneManager::AddChrLight( pTempModel, vecTargetLightPos + vector3( 0.0f, 100.0f, 0.0f ), 0xffff2200, 400.0f, 6 );

				vector3 vecEffectPos = vecTargetLightPos+vector3( 0.0f, 120.0f, 0.0f );

                CEffScript *test_script = new CEffScript;
				CSceneManager::AddEffectScript( test_script );

				if( fAttackRate >= 1.0f || m_lpObjControl->m_SkillAttackValue ) 
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

				//particle
				CX3DEffect *pEffect = new CX3DEffect;
				CSceneManager::AddEffect( pEffect );
				if( fAttackRate >= 1.0f || m_lpObjControl->m_SkillAttackValue )
				{
					pEffect->Load( EFFECTPATH, "attack2_particle.eff" );
					
					if( m_lpObjControl->m_SkillAttackValue > 0 )
						m_lpObjControl->m_SkillAttackValue--;
				}
				else
				{
					pEffect->Load( EFFECTPATH, "attack1_particle.eff" );
				}

				pEffect->SetLoop( FALSE );				
				pEffect->SetAxis( 0.0f,-( ( 180.0f * fAngle ) / 3.14159f ) + 90.0f + fAttackAngle, 0.0f );
				pEffect->SetCenter(vecEffectPos.x,vecEffectPos.y,vecEffectPos.z );				
				dwDefenser[ nAttackCount++ ] = pTempCreature->GetCharID();

				if( nAttackCount >= 10 ) 
					break;
			}
		}
	}
	
	if( nAttackCount > 0 || wSkill & 0x8000 )
	{
		m_lpObjControl->PlaySound( wSkill, vecFirstAttackPos );
		m_lpObjControl->WrapSendCharAttack( *( pSelfCreature->GetPosition() ),
							                pSelfCreature->GetDirection() - FLOAT_PHI / 2.0f,
							                wSkill, false, cSkillLock, cSkillLevel, cAtCount, nAttackCount, dwDefenser );
		return true;
	} 	

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLDropshipControl::MakeSkillAttackInfo()
bool		CRYLDropshipControl::MakeSkillAttackInfo( unsigned long dwTargetID, unsigned short wSkill, 
												    unsigned char cSkillLock, unsigned char 
													cSkillLevel, 
													unsigned char cAtCount, BOOL bGod, BOOL bDead )
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return false;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if( !pSelfModel )
        return false;

    if( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) )
        return false;

	unsigned long   dwDefenser[ 10 ] ;
	int				nAttackCount = 0 ;
	unsigned long	dwType;
	ZeroMemory( dwDefenser, sizeof( unsigned long ) * 4 ) ;

    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
    
	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for(; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
		if( NULL == pTempCreature )
			continue;

		CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
        if( NULL == pTempModel )
            continue;

        if( !bDead && pTempCreature->GetIsDead() )
            continue;

		dwType = m_lpObjControl->GetClientIdentity( pTempCreature->GetCharID() );
//		dwNationType = GetClientEnemyType( pTempCreature->m_dwChrID ) ;

        if ( bGod && dwType == Creature::CT_PC && pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Invincible ) ) 
			continue ;

        if ( pTempCreature->GetCharID() == dwTargetID )
		{
			if ( wSkill == 3 || wSkill == 4 )		// 활
			{
				vector3 vecChrPos ;

				float fAttackActionRate = rand() % 6 ;
				fAttackActionRate *= 0.1f ;
				fAttackActionRate += 0.3f ;

				if ( fAttackActionRate > 0.5f) 
					fAttackActionRate = 0.5f ;
				if ( pTempModel->IsAddonActionFinished( 1 ) )
					pTempModel->AddMotion( "ATTACKED", 3, fAttackActionRate ) ;

				pTempModel->GetPosition( vecChrPos ) ;
				CSceneManager::AddChrLight( pTempModel, vecChrPos + vector3( 0.0f, 200.0f, 0.0f ), 0xffff2200, 400.0f, 6 ) ;

				vector3		vecEffectPos ;
				vecEffectPos	= vecChrPos ;
				vecEffectPos.y += 120.0f ;

				CEffScript *test_script ;				
//				_ASSERTE(_CrtCheckMemory());
				test_script = new CEffScript ;
				CSceneManager::AddEffectScript( test_script ) ;
				test_script->GetScriptBinData( "attack1.esf" ) ;
				test_script->SetStartPos( pSelfCreature->GetPosition()->x,
										  pSelfCreature->GetPosition()->y,
										  pSelfCreature->GetPosition()->z ) ;
				test_script->SetEndPos( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z ) ;
				test_script->SetChr( pSelfModel, pTempModel );

				// particle
				CX3DEffect *pEffect ;
				pEffect=new CX3DEffect ;
				CSceneManager::AddEffect( pEffect ) ;
				pEffect->Load( EFFECTPATH,"attack1_particle.eff" ) ;
				pEffect->SetLoop( FALSE ) ;				
				pEffect->SetCenter( vecEffectPos.x, vecEffectPos.y, vecEffectPos.z ) ;
				m_lpObjControl->PlaySound( wSkill, vecEffectPos ) ;
			}
            dwDefenser[ nAttackCount ++ ] = pTempCreature->GetCharID();
		}
		if ( nAttackCount >= 10 )
			break ;
	}

	if ( nAttackCount > 0 || wSkill & 0x8000 )
	{
        m_lpObjControl->WrapSendCharAttack( *( pSelfCreature->GetPosition() ),
                                            pSelfCreature->GetDirection() - FLOAT_PHI / 2.0f,
											0x9A01, false, cSkillLock, cSkillLevel, cAtCount, nAttackCount, dwDefenser ) ;
		return true ;
	}
	return false ;
}
