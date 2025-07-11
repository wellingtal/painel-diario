#include <Creature/CreatureStructure.h>
#include "RYLGMCharacterControl.h"
#include "RYLObjectControl.h"
#include "WinInput.h"
#include "FrameTimer.h"
#include "BaseGraphicsLayer.h"
#include "item/ItemConstants.h"

#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "RYLNetworkData.h"
#include "RYLGameData.h"

#include "RYLStringTable.h"
#include "RYLChattingDlg.h"

CRYLGMCharacterControl::CRYLGMCharacterControl( CCharacterControl* lpObjControl ) 
{
	m_lpObjControl = lpObjControl ;
	m_lpCreature   = NULL ;

	m_bLeftMousePress		= FALSE;
	m_bRightMousePress		= FALSE;
	m_bClickPlayerAndNpc	= FALSE;
	m_bClickSkill			= FALSE;

	m_dwNormalTargetID		= 0xFFFFFFFF;
	m_dwSpecialTargetID		= 0xFFFFFFFF;

	m_bLeftClickDown		= FALSE;
	m_bKeyPress				= false;

}
	
CRYLGMCharacterControl::~CRYLGMCharacterControl() 
{
}

HRESULT CRYLGMCharacterControl::UpdateControl( BOOL bKeyAble, BOOL bEdge, int nMouseX,int nMouseY ) 
{
	if ( CRYLNetworkData::Instance()->GetNetworkFlag() != NF_NOWAITING )
		return E_FAIL ;		

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return E_FAIL;

	if ( m_lpObjControl->m_MouseClickMode == 0 )
	{
		MouseClickModeSelfCharacterUpdate( bKeyAble, bEdge, nMouseX, nMouseY );
		return E_FAIL;
	}

	CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

	if ( !pSelfModel || !pSelfData )
		return E_FAIL;

	CCharacterLightShadowManager::CCharacterDataNode CheckNode;
	CSceneManager::CharacterCollisionDetectType( pSelfModel, CDT_FULL );

	float fAngle = pSelfModel->GetDirection();
	fAngle -= pSelfCreature->GetFixAngle();
	pSelfCreature->SetFixAngle( 0.0f );
	pSelfModel->SetDirection( fAngle );

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

	if ( bKeyAble && g_DeviceInput.GetIsLeftMouseDown() )
	{
		m_bLeftClickDown = TRUE;
	} 
	else if ( g_DeviceInput.GetIsLeftMouseUp() )
	{
		m_bLeftClickDown = FALSE;
	}

	m_lpObjControl->CheckAttackedTiming();
	m_lpObjControl->CheckCameraShakeTiming();

	CollisionType   CT = CSceneManager::GetCharacterCollisionType( pSelfModel );	

	if ( true == pSelfCreature->Landing( m_lpObjControl ) )
	{
		pSelfCreature->InitCharAction();
		if ( m_lpObjControl->ChangeWeapon ) 
			m_lpObjControl->ChangeWeapon();
	}

	m_bKeyPress = false;

	float	fRunFactor  = CFrameTimer::GetUpdateTimer( m_lpObjControl->m_dwRunFactorTimer );

	if ( bKeyAble && !pSelfCreature->GetIsDead() )
	{                                                   
		CollisionType   CT      = CSceneManager::GetCharacterCollisionType( pSelfModel );
		bool            bSpace  = false;

		CheckNode = CSceneManager::GetCharacterDataNode( pSelfModel );		
		
		if ( !m_bKeyPress )
		{
			// 움직일수 있는지 체크
			if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold )
				&& !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) 
				&& CRYLGameData::Instance()->GetEnableMove() )
			{                    
				if ( KeyPressed( 'W' ) && KeyPressed( 'A' ) )
				{
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );

					vecChrToward = vecChrToward + vecChrLeft;
					vecChrToward.Normalize();

					pSelfCreature->SetFixAngle( -FLOAT_PHI / 4.0f );
					fAngle += -FLOAT_PHI / 4.0f;

					pSelfModel->SetDirection( fAngle );
				} 
				else if ( KeyPressed( 'W' ) && KeyPressed( 'D' ) )
				{
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );

					vecChrToward = vecChrToward - vecChrLeft;  
					vecChrToward.Normalize();

					pSelfCreature->SetFixAngle( FLOAT_PHI / 4.0f );
					fAngle += FLOAT_PHI / 4.0f;

					pSelfModel->SetDirection( fAngle );
				} 
				else if ( KeyPressed( 'W' ) )
				{
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
				}
				else if ( KeyPressed( 'S' ) && KeyPressed( 'A' ) )
				{
					m_bKeyPress		= true;

					vecChrToward	= vecChrToward - vecChrLeft;
					vecChrToward.Normalize();

					pSelfCreature->SetFixAngle( FLOAT_PHI / 4.0f );
					fAngle += FLOAT_PHI / 4.0f;

					pSelfModel->SetDirection( fAngle );

					pSelfCreature->Walk( RYLCreature::MOVE_BACK, 0.0f );
				}
				else if ( KeyPressed( 'S' ) && KeyPressed( 'D' ) )
				{
					m_bKeyPress		= true;

					vecChrToward	= vecChrToward + vecChrLeft;
					vecChrToward.Normalize();

					pSelfCreature->SetFixAngle( -FLOAT_PHI / 4.0f );
					fAngle += -FLOAT_PHI / 4.0f;

					pSelfModel->SetDirection( fAngle );			
					pSelfCreature->Walk( RYLCreature::MOVE_BACK, 0.0f );
				}
				else if ( KeyPressed( 'S' ) )
				{
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_BACK, 0.0f );
				}
				else if ( KeyPressed( 'A' ) )
				{
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_LEFT, 0.0f );
				}
				else if ( KeyPressed( 'D' ) )
				{
					m_bKeyPress = true;
					pSelfCreature->Walk( RYLCreature::MOVE_RIGHT, 0.0f );
				}
			}
		}
	}

	// 움직일수 없는 상황인지 체크
	if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) ||
		pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) ||
		!CRYLGameData::Instance()->GetEnableMove() )
	{
		if ( m_lpObjControl->SetAutoRun ) 
			m_lpObjControl->SetAutoRun( FALSE );

		m_lpObjControl->m_bAutoFollowMode	= FALSE;
		m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
	}

	if ( !m_bKeyPress )
	{
		m_lpObjControl->PlayerProcessOperation( bKeyAble );

		if ( m_lpObjControl->GetAutoRun && m_lpObjControl->GetAutoRun() )
		{
			m_bKeyPress = true;
			pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, fRunFactor );
			m_lpObjControl->SetKillCounter( 0 );
			m_lpObjControl->SetClientExitCounter( 0 );
			m_lpObjControl->SetLogoutCounter( 0 );
			m_lpObjControl->SetPortalCounter( 0 );
			m_lpObjControl->SetSkillUseCounter( 0 );
		}

		if ( m_lpObjControl->m_bAutoFollowMode )
		{
			RYLCreature* pFollowCreature = RYLCreatureManager::Instance()->GetCreature( m_lpObjControl->m_dwAutoFollowChrID );

			if ( pFollowCreature )
			{
				vector3 vecLen = *( pFollowCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
				vecLen.y	   = 0.0f;

				if ( vecLen.GetLens() > 700.0f )
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
		m_lpObjControl->SetLogoutCounter( 0 );
		m_lpObjControl->SetPortalCounter( 0 );
		m_lpObjControl->SetSkillUseCounter( 0 );

		if ( m_lpObjControl->SetAutoRun ) 
			m_lpObjControl->SetAutoRun( FALSE );

		m_lpObjControl->m_bAutoFollowMode	= FALSE;
		m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
	}
	
	if ( !m_bKeyPress )
	{
		if ( !pSelfCreature->GetIsDead() )
		{
			pSelfCreature->Wait();
		}
	}

	if ( !m_lpObjControl->GetAttacking() )
	{
		if ( m_lpObjControl->SetChangeWeapon )
			m_lpObjControl->SetChangeWeapon( FALSE );
	}

	switch ( pSelfCreature->GetLowerActionID() )
	{
	case CA_WALK:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * ( m_lpObjControl->m_fWalkSpeed / 35.0f ) );
		}
		break;

	case CA_FRONTDASH:
	case CA_RUN:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * ( m_lpObjControl->m_fRunSpeed / 35.0f ) );
		}
		break;

	case CA_WALKBACK:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * -( m_lpObjControl->m_fWalkSpeed / 35.0f ) );
		}
		break;

	case CA_WALKLEFT:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * ( m_lpObjControl->m_fWalkSpeed / 35.0f ) );
		}
		break;

	case CA_WALKRIGHT:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * -( m_lpObjControl->m_fWalkSpeed / 35.0f ) );
		}
		break;
	
	case CA_LEFTDASH:
		{
			string strMotionSheet = pSelfModel->GetCurrentMotionSheetName();

			if ( strMotionSheet.size() 
				&& ( !strMotionSheet.compare( "CLASS_ROGUE" ) || !strMotionSheet.compare( "WEAPON_SHADOW_OFF" )
				|| !strMotionSheet.compare( "WEAPON_2CLOW" ) || !strMotionSheet.compare( "WEAPON_2DA" ) ) )
			{
				CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * ( 1365.0f / 35.0f ) , -1.1f );
			} 
			else
			{
				CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * ( 1050.0f / 35.0f ) , -1.1f );
			}
		}
		break;

	case CA_RIGHTDASH:
		{
			string strMotionSheet = pSelfModel->GetCurrentMotionSheetName();

			if ( strMotionSheet.size() 
				&& ( !strMotionSheet.compare( "CLASS_ROGUE" ) || !strMotionSheet.compare( "WEAPON_SHADOW_OFF" )
				|| !strMotionSheet.compare( "WEAPON_2CLOW" ) || !strMotionSheet.compare( "WEAPON_2DA" ) ) )
			{
				CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * -( 1365.0f / 35.0f ) ,-1.1f );
			} 
			else
			{
				CSceneManager::CharacterMovementVector( pSelfModel, vecChrLeft * -( 1050.0f / 35.0f ) ,-1.1f );
			}
		}
		break;

	case CA_BACKDASH:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * -( 1365.0f / 35.0f ) ,-1.1f);
		}
		break;

	default:
		{
			CSceneManager::CharacterMovementVector( pSelfModel, vector3( 0.0f, 0.0f, 0.0f ) );
		}
		break;
	}

	if ( CFrameTimer::GetUpdateTimer( m_lpObjControl->m_nCharMovingPacketTimerID ) >= 1.0f ) // && pSelfCreature->IsCancelAction(2))
	{
		if ( pSelfCreature->GetLowerActionID() == CA_WALK
			|| pSelfCreature->GetLowerActionID() == CA_RUN
			|| pSelfCreature->GetLowerActionID() == CA_WALKBACK
			|| pSelfCreature->GetLowerActionID() == CA_WALKLEFT
			|| pSelfCreature->GetLowerActionID() == CA_WALKRIGHT			
			|| pSelfCreature->GetLowerActionID() == CA_WAIT						
			|| pSelfCreature->GetLowerActionID() == CA_FRONTDASH			
			|| pSelfCreature->GetLowerActionID() == CA_FALLDOWN )
		{
			vecChrToward.Normalize();
			vecChrLeft.Normalize();

			vector3 vecMove;

			switch( pSelfCreature->GetLowerActionID() )
			{
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

			default:
				{
					vecMove = vector3( 0.0f, 0.0f, 0.0f );
				}
				break;
			}

			vecMove += *( pSelfCreature->GetPosition() );

			CollisionType CT = CSceneManager::GetCharacterCollisionType( pSelfModel );

			if ( CT == CT_WATER )
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
// CRYLGMCharacterControl::MouseClickModeSelfCharacterUpdate()
static BOOL bEdit = TRUE;
void CRYLGMCharacterControl::MouseClickModeSelfCharacterUpdate( BOOL bKeyAble, BOOL bEdge, int nMouseX, int nMouseY )
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return;

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if ( !pSelfData )
        return;

	if ( bEdit )
	{
        if ( !RYLCreatureManager::Instance()->GetCreatureListHead() )
            return;

		if ( m_lpObjControl->m_bClientUsed )
		{
			nMouseX = g_DeviceInput.GetMousePosition()->x;
			nMouseY = g_DeviceInput.GetMousePosition()->y;
		}

		CCharacterLightShadowManager::CCharacterDataNode CheckNode;
		CSceneManager::CharacterCollisionDetectType( pSelfModel, CDT_FULL );

        pSelfCreature->SetDirection( pSelfModel->GetDirection() );
        float fAngle = pSelfModel->GetDirection();

        pSelfCreature->SetPosition( pSelfModel->GetPosition() );
		vector3 vecChrPos	 = pSelfModel->GetPosition();
		vector3 vecChrToward = vector3( 0.0f, 0.0f, 0.0f );
		vector3 vecChrLeft	 = vector3( 0.0f, 0.0f, 0.0f );

		m_lpObjControl->CheckAttackedTiming();
		m_lpObjControl->CheckCameraShakeTiming();

		m_bKeyPress = false;

		float fRunFactor = CFrameTimer::GetUpdateTimer( m_lpObjControl->m_dwRunFactorTimer );

		BOOL bAvailableTarget = FALSE;

		if ( bKeyAble && g_DeviceInput.GetIsLeftMouseDown() )
		{
			m_bLeftClickDown = TRUE;
		} 
		else if ( g_DeviceInput.GetIsLeftMouseUp() )
		{
			m_bLeftClickDown = FALSE;
		}

		unsigned long dwFunction;
		if ( pSelfCreature->GetStillCasting() && !m_lpObjControl->CheckStillCasting( dwFunction ) )
		{
			m_lpObjControl->m_bSkillExecuted = TRUE;
		}

		if ( !bEdge && bKeyAble && m_lpObjControl->m_lAutoRunCounter && g_DeviceInput.GetIsLeftMousePress() )
		{
			m_lpObjControl->m_lAutoRunCounter -= fRunFactor * 5;
			
			if ( m_lpObjControl->m_lAutoRunCounter < 0 )
			{
				m_lpObjControl->m_lAutoRunCounter   = 0;
				m_lpObjControl->SetAutoRun( TRUE );
				m_lpObjControl->m_bAutoFollowMode   = FALSE;
				m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
			}
		}

		if ( true == pSelfCreature->Landing( m_lpObjControl ) )
		{
			pSelfCreature->InitCharAction();

			if ( m_lpObjControl->ChangeWeapon ) 
            {
				m_lpObjControl->ChangeWeapon();
            }
		}
		
        RYLCreature* pTempCreature = NULL;
        RYLCreature* pSpecialCreature = NULL;

		if ( !bEdge && bKeyAble && !pSelfCreature->GetIsDead() )
		{
			CollisionType CT = CSceneManager::GetCharacterCollisionType( pSelfModel );
			CheckNode = CSceneManager::GetCharacterDataNode( pSelfModel );

			// 타겟 처리
			CZ3DGeneralChrModel *pSelectChr = NULL;
			pSelectChr = CSceneManager::PickingCharacter( nMouseX, nMouseY );

			if ( pSelectChr ) 
                pTempCreature = RYLCreatureManager::Instance()->GetCreature( pSelectChr );

			if ( m_lpObjControl->CheckSkillStart && m_lpObjControl->CheckSkillStart( TRUE ) )
			{
				unsigned long dwFunction;
				m_lpObjControl->CheckStillCasting( dwFunction );
				
				if ( dwFunction != 0xFFFFFFFF )
				{
                    pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwFunction );
				}
			}
			
			if ( pTempCreature )
			{
				bAvailableTarget = TRUE;

				if ( m_dwSpecialTargetID == 0xFFFFFFFF )
				{
					m_lpObjControl->m_dwSpecialTargetID = pTempCreature->GetCharID();
					
					if ( m_lpObjControl->CheckTargetforSkill )
					{
						m_lpObjControl->CheckTargetforSkill( m_lpObjControl->m_dwSpecialTargetID, TRUE );
					}
				}

				if ( m_bLeftMousePress && g_DeviceInput.GetIsLeftMousePress() )	// 왼쪽버튼 마우스 커서 스냅
				{
					if ( pTempCreature->GetCharID() != 0xFFFFFFFF && pSelfCreature->GetCharID() != pTempCreature->GetCharID() )
					{
						vector3 vecResult, vecTargetMonsterPos;
						float fW;

						vecTargetMonsterPos = *( pTempCreature->GetPosition() );
						vecTargetMonsterPos = vecTargetMonsterPos + vector3( 0.0f, 100.0f, 0.0f );
						BaseGraphicsLayer::TransformVector( vecTargetMonsterPos, vecResult, fW );
						g_DeviceInput.SetCursorPos( ( int )vecResult.x, ( int )vecResult.y );
					}
				}

				if ( m_bRightMousePress && g_DeviceInput.GetIsRightMousePress() )	// 오른쪽버튼 마우스 커서 스냅
				{
					if ( m_dwSpecialTargetID != 0xFFFFFFFF )
					{
						vector3 vecResult, vecTargetMonsterPos;
						float fW;

						pSpecialCreature = RYLCreatureManager::Instance()->GetCreature( m_dwSpecialTargetID );

						if ( pSpecialCreature )
						{
							vecTargetMonsterPos = *( pSpecialCreature->GetPosition() );
							vecTargetMonsterPos = vecTargetMonsterPos + vector3( 0.0f, 100.0f, 0.0f );
							BaseGraphicsLayer::TransformVector( vecTargetMonsterPos, vecResult, fW );
							g_DeviceInput.SetCursorPos( ( int )vecResult.x, ( int )vecResult.y );
						}
					}
				}
			}
			else
			{
				if ( m_dwSpecialTargetID == 0xFFFFFFFF )
				{
					m_lpObjControl->m_dwSpecialTargetID = 0xFFFFFFFF;
				}
			}

			// 마우스 오른쪽 버튼
			RYLCreature* pNewSpecialTarget = MouseClickModeSelectSpecialTarget( pTempCreature, nMouseX, nMouseY ) ;
			if (NULL != pNewSpecialTarget)
			{
				pSpecialCreature = pNewSpecialTarget;
			}

			// 마우스 왼쪽 버튼
			if ( g_DeviceInput.GetIsLeftMouseDown() )
			{
				m_bLeftMousePress	= TRUE;
				m_bKeyPress			= true;
				m_lpObjControl->m_bAutoFollowMode	= FALSE;
				m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
				m_lpObjControl->SetAutoRun( FALSE );

				if ( pTempCreature )
				{
					if ( m_lpObjControl->m_bSummonCmd )
					{
						m_lpObjControl->m_bAutoTargetAttack = false;
						m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;
					}
					else
					{
                        if ( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) &&
							pSelfCreature->GetCharID() != pTempCreature->GetCharID() )
						{
							m_lpObjControl->m_bAutoTargetAttack = true;
							m_lpObjControl->m_dwAutoTargetID	= pTempCreature->GetCharID();
						}
						else
						{
							m_lpObjControl->m_bAutoTargetAttack = false;
							m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;
						}
					}

					m_dwNormalTargetID	= pTempCreature->GetCharID();
					m_dwSpecialTargetID	= 0xFFFFFFFF;
				} 
				else
				{
					m_lpObjControl->m_nSelectItem		= CSceneManager::MouseOnCheckInstanceObject( nMouseX, nMouseY );
					m_dwNormalTargetID	= 0xFFFFFFFF;
					m_dwSpecialTargetID	= 0xFFFFFFFF;
					m_lpObjControl->m_bAutoTargetAttack = false;
					m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;

					if ( m_lpObjControl->m_nSelectItem == 0xFFFFFFFF )
					{
						m_lpObjControl->m_lAutoRunCounter = 3000;

						CEffScript *pMousePointEff = new CEffScript;
						CSceneManager::AddEffectScript( pMousePointEff );

						vector3 vecMovePos = CSceneManager::PickMousePos( nMouseX, nMouseY );
						pMousePointEff->GetScriptBinData( "point.esf" );
						pMousePointEff->SetStartPos( vecMovePos.x, vecMovePos.y + 30.0f, vecMovePos.z );			
						pMousePointEff->SetEndPos( vecMovePos.x, vecMovePos.y + 30.0f, vecMovePos.z );
					}
				}
			}
			else if ( g_DeviceInput.GetIsLeftMouseUp() )
			{
				m_bLeftMousePress = FALSE;
			}
			else if ( g_DeviceInput.GetIsLeftMousePress() )
			{
				if ( m_dwSpecialTargetID != 0xFFFFFFFF )
				{
					m_dwNormalTargetID = m_dwSpecialTargetID;
				}
			}

			if ( !pTempCreature && m_dwNormalTargetID != 0xFFFFFFFF )
			{
				pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwNormalTargetID );
				
				if ( pTempCreature )
				{
					bAvailableTarget = TRUE;
				}
				else
				{
					m_dwNormalTargetID = 0xFFFFFFFF;
				}
			}

			if ( !pSpecialCreature && m_dwSpecialTargetID != 0xFFFFFFFF )
			{
				pSpecialCreature = RYLCreatureManager::Instance()->GetCreature( m_dwSpecialTargetID );
				
                if ( !pSpecialCreature )
				{
					m_dwSpecialTargetID = 0xFFFFFFFF;
				}
			}

			if ( pTempCreature )
			{
				if ( pTempCreature->GetIsDead() || pTempCreature->GetCharID() == pSelfCreature->GetCharID() )
				{
					m_dwNormalTargetID	= 0xFFFFFFFF;
					pTempCreature		= NULL;
					bAvailableTarget	= FALSE;
				}
			}

			if ( m_lpObjControl->m_bAutoTargetAttack )
			{
				RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( m_lpObjControl->m_dwAutoTargetID );
				
                if ( pTargetCreature )
				{
					if ( pTargetCreature->GetIsDead() )
					{
						m_lpObjControl->m_bAutoTargetAttack = false;
						m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;
					}
				}
				else
				{
					m_lpObjControl->m_bAutoTargetAttack = false;
					m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;
				}

				if ( m_lpObjControl->m_dwAutoTargetID != 0xFFFFFFFF )
					m_dwNormalTargetID = m_lpObjControl->m_dwAutoTargetID;
			}

			// 움직임 체크
			unsigned long dwFunction;
			if ( ( m_bLeftClickDown && g_DeviceInput.GetIsLeftMousePress() && m_bLeftMousePress ) || 
				( g_DeviceInput.GetIsRightMousePress() && m_bRightMousePress ) || 
				( m_lpObjControl->CheckStillCasting && m_lpObjControl->CheckStillCasting( dwFunction ) ) )
			{
				if ( ( m_bLeftClickDown && g_DeviceInput.GetIsLeftMousePress() && m_bLeftMousePress ) || 
					( g_DeviceInput.GetIsRightMousePress() && m_bRightMousePress ) )
				{
					CSceneManager::PickMakeRay( nMouseX, nMouseY, BaseGraphicsLayer::m_lScreenSx, BaseGraphicsLayer::m_lScreenSy );
					vector3 vecStart = CSceneManager::m_vecPickRayStart;
					vector3 vecEnd	 = CSceneManager::m_vecPickRayStart + 1000000.0f * CSceneManager::m_vecPickRayDir;

					vector3 vecPoly[ 4 ];
					float fSize = 1000000.0f;
					vecPoly[ 0 ] = vecChrPos + vector3( -fSize, 0.0f, -fSize );
					vecPoly[ 1 ] = vecChrPos + vector3( fSize, 0.0f, -fSize );
					vecPoly[ 2 ] = vecChrPos + vector3( -fSize, 0.0f, fSize );
					vecPoly[ 3 ] = vecChrPos + vector3( fSize, 0.0f, fSize );

					float fInter;
					if ( CIntersection::PolygonRay( vecStart, vecEnd, vecPoly, fInter ) )
					{
						m_lpObjControl->m_vecClickPos = vecStart + ( fInter * CSceneManager::m_vecPickRayDir );
					}
					else if ( CIntersection::PolygonRay( vecStart, vecEnd, &vecPoly[ 1 ], fInter ) )
					{
						m_lpObjControl->m_vecClickPos = vecStart + ( fInter * CSceneManager::m_vecPickRayDir );
					} 
					else
					{
						m_lpObjControl->m_vecClickPos = CSceneManager::PickMousePos( nMouseX, nMouseY );
					}
				}

				if ( pTempCreature )
				{
                    CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

					if ( pTempModel )
                    {
						m_lpObjControl->m_vecClickPos = pTempModel->GetPosition();
                    }
					else
                    {
                        m_lpObjControl->m_vecClickPos = *( pTempCreature->GetPosition() );
                    }

					vector3 vecLen = m_lpObjControl->m_vecClickPos - *( pSelfCreature->GetPosition() ); 
					vecLen.y = 0.0f;

					float fTargetLength = vecLen.GetLens();

					if ( g_DeviceInput.GetIsLeftMouseDown() && m_lpObjControl->m_bSummonCmd )
					{
						m_lpObjControl->m_bClickMove		= false;
						m_lpObjControl->m_bClickAttack		= false;
						m_bClickPlayerAndNpc	= FALSE;
						m_bLeftClickDown		= FALSE;

						if ( m_lpObjControl->GetClientEnemyType( pTempCreature->GetCharID() ) == Creature::ET_ENEMY )
						{
							m_lpObjControl->SendSummonAttack( pTempCreature->GetCharID() );
						}

						return;
					}

					if ( g_DeviceInput.GetIsLeftMousePress() )
					{
						switch( m_lpObjControl->GetClientIdentity( pTempCreature->GetCharID() ) )
						{
							case Creature::CT_PC:
                            case Creature::CT_STRUCT:
							case Creature::CT_SUMMON:
							case Creature::CT_SIEGE_OBJECT:
                            {
								m_lpObjControl->m_bClickMove	= false ;								
								m_bClickPlayerAndNpc			= FALSE ;								
								break;
                            }

							case Creature::CT_NPC:
                            {
								// 이동 후 대화
								if ( m_dwNormalTargetID != 0xFFFFFFFF )
								{
									m_lpObjControl->m_bClickMove		= false;
									m_lpObjControl->m_bClickAttack		= false;
									m_bClickPlayerAndNpc	= TRUE;
								}
								else
								{
									m_lpObjControl->m_bClickMove		= true;
									m_lpObjControl->m_bClickAttack		= false;
									m_bClickPlayerAndNpc	= FALSE;
								}
								break;
                            }

							case Creature::CT_MONSTER:
                            {
								m_lpObjControl->m_bClickMove		= true;
								m_lpObjControl->m_bClickAttack		= false;
								m_bClickPlayerAndNpc	= FALSE;
								
								break;
                            }
						}
					}
				}

				/*if ( pSpecialCreature )
				{
					//==============================================================
					// 스킬 사용
					unsigned long dwFunction;

                    if ( m_lpObjControl->CheckStillCasting( dwFunction ) )
					{
						// 타겟 유무 체크
                        if ( m_lpObjControl->GetClientIdentity( pSpecialCreature->GetCharID() ) != Creature::CT_NPC )
						{
							// 이동 후 스킬
							if ( m_dwSpecialTargetID != 0xFFFFFFFF )
							{
								m_lpObjControl->m_bClickMove		= false;
								m_lpObjControl->m_bClickAttack		= false;
								m_bClickPlayerAndNpc	= FALSE;
								m_bClickSkill			= TRUE;
							} 
							else
							{
								m_lpObjControl->m_bClickMove		= true;
								m_lpObjControl->m_bClickAttack		= false;
								m_bClickPlayerAndNpc	= FALSE;
								m_bClickSkill			= FALSE;
							}
						}
					}
				}*/

				if ( ( g_DeviceInput.GetIsLeftMousePress() && m_bLeftMousePress ) || ( g_DeviceInput.GetIsRightMousePress() && m_bRightMousePress ) )
				{
					if ( !pTempCreature && !pSpecialCreature )
					{
						vector3 vecLen = m_lpObjControl->m_vecClickPos - *( pSelfCreature->GetPosition() );
						vecLen.y = 0.0f;

						if ( vecLen.GetLens() <= TARGETNEAR_RANGE )
						{
							m_lpObjControl->m_bClickMove		= false;
							m_lpObjControl->m_bClickAttack		= false;
							m_bClickPlayerAndNpc	= FALSE;
						} 
						else
						{
							// 이동
							m_lpObjControl->m_bClickMove		= true;
							m_lpObjControl->m_bClickAttack		= false;
							m_bClickPlayerAndNpc	= FALSE;
						}
					}
				}
			}
		}

		if ( m_dwNormalTargetID != 0xFFFFFFFF || m_dwSpecialTargetID != 0xFFFFFFFF )
		{
			m_lpObjControl->SetAutoRun( FALSE );
			m_lpObjControl->m_bAutoFollowMode	= FALSE;
			m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
		}

		if ( !m_bKeyPress )
		{
			if ( m_lpObjControl->GetAutoRun && m_lpObjControl->GetAutoRun() )
			{
				m_lpObjControl->SetKillCounter( 0 );
				m_lpObjControl->SetClientExitCounter( 0 );
				m_lpObjControl->SetLogoutCounter( 0 );
				m_lpObjControl->SetPortalCounter( 0 );
				m_lpObjControl->SetSkillUseCounter( 0 );

				if ( bKeyAble && !bEdge && !g_DeviceInput.GetIsMiddleMousePress() )
				{
					CSceneManager::PickMakeRay( nMouseX, nMouseY, BaseGraphicsLayer::m_lScreenSx, BaseGraphicsLayer::m_lScreenSy );
					vector3 vecStart = CSceneManager::m_vecPickRayStart;
					vector3 vecEnd	 = CSceneManager::m_vecPickRayStart + 1000000.0f * CSceneManager::m_vecPickRayDir;

					vector3 vecPoly[ 4 ];
					float fSize = 1000000.0f;
					vecPoly[ 0 ] = vecChrPos + vector3( -fSize, 0.0f, -fSize );
					vecPoly[ 1 ] = vecChrPos + vector3( fSize, 0.0f, -fSize );
					vecPoly[ 2 ] = vecChrPos + vector3( -fSize, 0.0f, fSize );
					vecPoly[ 3 ] = vecChrPos + vector3( fSize, 0.0f, fSize );

					float fInter;
					if ( CIntersection::PolygonRay( vecStart, vecEnd, vecPoly, fInter ) )
					{
						m_lpObjControl->m_vecClickPos = vecStart + ( fInter * CSceneManager::m_vecPickRayDir );
					}
					else if ( CIntersection::PolygonRay( vecStart, vecEnd, &vecPoly[ 1 ], fInter ) )
					{
						m_lpObjControl->m_vecClickPos = vecStart + ( fInter * CSceneManager::m_vecPickRayDir );
					}
					else
					{
						m_lpObjControl->m_vecClickPos = CSceneManager::PickMousePos( nMouseX, nMouseY );
					}
				}
				else
				{
					float fChrDir = pSelfCreature->GetDirection();
					fChrDir = -fChrDir;
					fChrDir -= FLOAT_PHI / 2.0f;

					vector3 vecToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
					vecToward.Normalize();
					
                    vecToward *= 100.0f;
					m_lpObjControl->m_vecClickPos = *( pSelfCreature->GetPosition() ) + vecToward;
				}

				m_lpObjControl->m_bClickMove		= true;
				m_lpObjControl->m_bClickAttack		= false;
				m_bClickPlayerAndNpc	= FALSE;
			}

			if ( m_lpObjControl->m_bAutoFollowMode )
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_lpObjControl->m_dwAutoFollowChrID );
				
                if ( pTempCreature )
				{
					vector3 vecLen = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
					vecLen.y = 0.0f;

					if ( vecLen.GetLens() > 700.0f )
					{
						vecLen.Normalize();
						vecLen *= 490.0f;
						m_lpObjControl->m_vecClickPos = *( pSelfCreature->GetPosition() ) + vecLen;
					}

					m_lpObjControl->m_bClickMove		= true;
					m_lpObjControl->m_bClickAttack		= false;
					m_bClickPlayerAndNpc	= FALSE;
				} 
				else
				{
					m_lpObjControl->m_bAutoFollowMode	= FALSE;
					m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;

					m_lpObjControl->m_bClickMove		= false;
					m_lpObjControl->m_bClickAttack		= false;
					m_bClickPlayerAndNpc	= FALSE;
				}
			}
		} 
		else
		{
			m_lpObjControl->SetAutoRun( FALSE );
			m_lpObjControl->m_bAutoFollowMode = FALSE;
			m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
		}

		if ( !pSelfCreature->GetIsDead() )
		{
			//====================================================================
			// 클릭해서 이동
			//====================================================================
			if ( m_lpObjControl->m_bClickMove )
			{
				m_lpObjControl->SetKillCounter( 0 );
				m_lpObjControl->SetClientExitCounter( 0 );
				m_lpObjControl->SetLogoutCounter( 0 );
				m_lpObjControl->SetPortalCounter( 0 );
				m_lpObjControl->SetSkillUseCounter( 0 );

                vecChrToward	= m_lpObjControl->m_vecClickPos - *( pSelfCreature->GetPosition() );
				vecChrToward.y	= 0.0f;

				if ( m_lpObjControl->m_nSelectItem != 0xFFFFFFFF )
				{
					if ( vecChrToward.GetLens() <= 300.0f )
					{
						m_lpObjControl->m_bClickMove		= false;
						m_lpObjControl->m_bClickAttack		= false;
						m_bClickPlayerAndNpc	= FALSE;

						if ( m_lpObjControl->PickItem )
                        {
							m_lpObjControl->PickItem( m_lpObjControl->m_nSelectItem );
                        }

						m_lpObjControl->m_nSelectItem = 0xFFFFFFFF;
					} 
					else
					{
						// 움직일수 있는지 체크
                        if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) && 
							!pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) &&
							CRYLGameData::Instance()->GetEnableMove() )
						{   
							m_lpObjControl->SetDirectionforMouse( vecChrToward );
                            pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );
						}
					}
				} 
				else
				{
					if ( m_lpObjControl->GetAutoRun && !m_lpObjControl->GetAutoRun() && vecChrToward.GetLens() <= TARGETNEAR_RANGE )
					{
						m_lpObjControl->m_bClickMove		= false;
						m_lpObjControl->m_bClickAttack		= false;
						m_bClickPlayerAndNpc	= FALSE;
					} 
					else
					{
						// 움직일수 있는지 체크
						if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) && 
							!pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) &&
							CRYLGameData::Instance()->GetEnableMove() )
						{
							m_lpObjControl->SetDirectionforMouse( vecChrToward );
                            pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );
						}
					}
				}
			}

			//====================================================================
			// 클릭해서 공격 (몬스터 위치로 최대한 가까이 간다)
			//====================================================================
			if ( m_lpObjControl->m_bClickAttack )
			{				
				m_lpObjControl->SetKillCounter( 0 );
				m_lpObjControl->SetClientExitCounter( 0 );
				m_lpObjControl->SetLogoutCounter( 0 );
				m_lpObjControl->SetPortalCounter( 0 );
				m_lpObjControl->SetSkillUseCounter( 0 );

				RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( m_dwNormalTargetID );

				if ( pTargetCreature )
				{												
					// 움직일수 있는지 체크
					if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) && 
						!pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) &&
						CRYLGameData::Instance()->GetEnableMove() )
					{
						m_lpObjControl->SetDirectionforMouse( vecChrToward );
						pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );
					}
				}
				else
				{
					m_dwNormalTargetID	= 0xFFFFFFFF;
					m_lpObjControl->m_bClickMove		= false;
					m_lpObjControl->m_bClickAttack		= false;
					m_bClickPlayerAndNpc	= FALSE;
				}
			}


			//====================================================================
			// 클릭해서 대화 / 대기
			//====================================================================
			if ( m_bClickPlayerAndNpc )
			{
				m_lpObjControl->SetKillCounter( 0 );
				m_lpObjControl->SetClientExitCounter( 0 );
				m_lpObjControl->SetLogoutCounter( 0 );
				m_lpObjControl->SetPortalCounter( 0 );
				m_lpObjControl->SetSkillUseCounter( 0 );

				RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( m_dwNormalTargetID );

				if ( pTargetCreature )
				{
					vecChrToward	= *( pTargetCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
					vecChrToward.y	= 0.0f;

					if ( vecChrToward.GetLens() <= PLAYERTARGETNEAR_RANGE )
					{
						switch( m_lpObjControl->GetClientIdentity( pTargetCreature->GetCharID() ) )
						{
							case Creature::CT_PC:
                                {
								    if ( g_DeviceInput.KeyHold( VK_SHIFT ) )
								    {
                                        if ( m_lpObjControl->GetClientEnemyType( pTargetCreature->GetCharID() ) == Creature::ET_ENEMY )
										    break;
									    m_lpObjControl->m_bCharacterTargetNear	= TRUE;
									    m_lpObjControl->m_dwNormalTargetID		= m_dwNormalTargetID;
								    }
                                }
								break;

							case Creature::CT_NPC:
							case Creature::CT_STRUCT:
								{
                                    if ( m_lpObjControl->GetClientEnemyType( pTargetCreature->GetCharID() ) == Creature::ET_ENEMY )
										break;
									m_lpObjControl->m_bCharacterTargetNear	= TRUE;
									m_lpObjControl->m_dwNormalTargetID	 	= m_dwNormalTargetID;
								}
								break;
						}

						m_lpObjControl->m_bClickMove		= false;
						m_lpObjControl->m_bClickAttack		= false;
						m_bClickPlayerAndNpc	= FALSE;
						m_dwNormalTargetID	= 0xFFFFFFFF;
						m_lpObjControl->SetDirectionforMouse( vecChrToward );
					} 
					else
					{
						// 움직일수 있는지 체크
						if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) &&
							!pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) &&
							CRYLGameData::Instance()->GetEnableMove() )
						{
							m_lpObjControl->SetDirectionforMouse( vecChrToward );
							pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );
						}
					}
				} 
				else
				{
					m_dwNormalTargetID	= 0xFFFFFFFF;
					m_lpObjControl->m_bClickMove		= false;
					m_lpObjControl->m_bClickAttack		= false;
					m_bClickPlayerAndNpc	= FALSE;
				}
			}

			//====================================================================
			// 클릭해서 스킬
			//====================================================================
			if ( m_bClickSkill )
			{
				m_lpObjControl->SetKillCounter( 0 );
				m_lpObjControl->SetClientExitCounter( 0 );
				m_lpObjControl->SetLogoutCounter( 0 );
				m_lpObjControl->SetPortalCounter( 0 );
				m_lpObjControl->SetSkillUseCounter( 0 );

				RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( m_dwSpecialTargetID );

				if ( pTargetCreature )
				{
					vecChrToward   = *( pTargetCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
					vecChrToward.y = 0.0f;
					
					// 움직일수 있는지 체크
					if ( !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ) &&
						!pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) &&
						CRYLGameData::Instance()->GetEnableMove() )
					{
						m_lpObjControl->SetDirectionforMouse( vecChrToward );
						pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );
					}
				}
				else
				{
					m_dwSpecialTargetID	= 0xFFFFFFFF;
					m_lpObjControl->m_bClickMove		= false;
					m_lpObjControl->m_bClickAttack		= false;
					m_bClickPlayerAndNpc	= FALSE;
					m_bClickSkill			= FALSE;
				}
			}

			if ( !m_lpObjControl->m_bClickMove && !m_bClickPlayerAndNpc)
			{
				pSelfCreature->Wait();
			}
		}		

		if ( !m_lpObjControl->GetAttacking() )
		{
			if ( m_lpObjControl->SetChangeWeapon ) 
				m_lpObjControl->SetChangeWeapon( FALSE );
		}

		vecChrToward.Normalize();
		switch ( pSelfCreature->GetLowerActionID() )
		{
			case CA_RUN:
				CSceneManager::CharacterMovementVector( pSelfModel, 
														vecChrToward * ( m_lpObjControl->m_fRunSpeed / 35.0f ) );
				break;

			case CA_WALK:
				CSceneManager::CharacterMovementVector( pSelfModel, 
														vecChrToward * ( m_lpObjControl->m_fWalkSpeed / 35.0f ) );
				break;

			default:
				CSceneManager::CharacterMovementVector( pSelfModel, 
													    vector3( 0.0f, 0.0f, 0.0f ) );
				break;
		}

        // 자신 이외의 타겟은 빛이 번쩍
		if ( bAvailableTarget && pTempCreature )	
		{
            CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

            if ( pTempModel )
            {
			    vector3 vecLightChrPos = *pTempCreature->GetPosition();
			    CSceneManager::AddChrLight( pTempModel, vecLightChrPos + vector3( 0.0f, 200.0f, 0.0f ), 0xff0022ff, 400.0f, 6 );
            }
		}

		m_lpObjControl->PlayerProcessOperation( bKeyAble );

		if ( CFrameTimer::GetUpdateTimer( m_lpObjControl->m_nCharMovingPacketTimerID ) >= 1.0f )// && pSelfCreature->IsCancelAction(2))
		{
			if ( pSelfCreature->GetLowerActionID() == CA_WALK
			 || pSelfCreature->GetLowerActionID() == CA_RUN
			 || pSelfCreature->GetLowerActionID() == CA_WALKBACK
			 || pSelfCreature->GetLowerActionID() == CA_WALKLEFT
			 || pSelfCreature->GetLowerActionID() == CA_WALKRIGHT
			 || pSelfCreature->GetLowerActionID() == CA_REST
			 || pSelfCreature->GetLowerActionID() == CA_WAIT			 
			 || pSelfCreature->GetLowerActionID() == CA_FRONTDASH			 
			 || pSelfCreature->GetLowerActionID() == CA_FALLDOWN )
			{
				vecChrToward.Normalize();
				vecChrLeft.Normalize();

				vector3 vecMove;
				switch ( pSelfCreature->GetLowerActionID() )
				{
					case CA_FRONTDASH:
					case CA_RUN:
					    vecMove = vecChrToward * ( m_lpObjControl->m_fRunSpeed / 4.0f );
						break;

					case CA_WALKBACK:	vecMove = vecChrToward * -( m_lpObjControl->m_fWalkSpeed / 4.0f );	break;
					case CA_WALKLEFT:	vecMove = vecChrLeft * ( m_lpObjControl->m_fWalkSpeed / 4.0f );		break;
					case CA_WALKRIGHT:	vecMove = vecChrLeft * -( m_lpObjControl->m_fWalkSpeed / 4.0f );	break;
					case CA_WALK:	    vecMove = vecChrToward * ( m_lpObjControl->m_fWalkSpeed / 4.0f );	break;
					
					default:		    vecMove = vector3( 0.0f, 0.0f, 0.0f );								break;
				}

				vecMove += *( pSelfCreature->GetPosition() );

				CollisionType CT = CSceneManager::GetCharacterCollisionType( pSelfModel );

				if ( CT == CT_WATER )
				{
                    m_lpObjControl->WrapSendMovingPacket( CA_SWIM, CA_SWIM, pSelfCreature->GetDirection(), vecMove );
				} 
				else
				{
					m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
										                  pSelfCreature->GetLowerActionID(),
                                                          pSelfCreature->GetDirection(), 
                                                          vecMove );
				}
			}
		}
	} 
	else
	{
		if ( !RYLCreatureManager::Instance()->GetCreatureListHead() )
            return;

		CSceneManager::CharacterCollisionDetectType( pSelfModel, CDT_FULL );

		float	fAngle    = pSelfModel->GetDirection();
		vector3 vecChrPos = pSelfModel->GetPosition();

		//=====================================================================
		// 마우스 왼쪽 버튼 상태
		static bool bMouseLeftButtonStateChange  = false;
		static bool m_bMouseLeftButtonStatePress = false;
		static bool bMouseRightButtonStateChange = false;
		static bool m_bMouseRightButtonStatePress= false;
		
		//0 타격기 1 원거리 2 셀프
		static int  m_nSkillType = 0;

		if ( KeyPressed( VK_LBUTTON ) )
		{
			if ( m_bMouseLeftButtonStatePress )
            {
				bMouseLeftButtonStateChange = false;
            }
			else
            {
				bMouseLeftButtonStateChange = false;
            }
			
			m_bMouseLeftButtonStatePress = true;
		}
		else
		{
			if ( m_bMouseLeftButtonStatePress )
            {
				bMouseLeftButtonStateChange = true;
            }
			else
            {
				bMouseLeftButtonStateChange = false;
            }
			
			m_bMouseLeftButtonStatePress = false;
		}

		if ( KeyPressed( VK_RBUTTON ) )
		{
			if ( m_bMouseRightButtonStatePress )
            {
				bMouseRightButtonStateChange = true;
            }
			else
            {
				bMouseRightButtonStateChange = false;
            }

			m_bMouseRightButtonStatePress = false;
		}
		//=====================================================================

		if ( m_lpObjControl->m_bClientUsed )
		{
			nMouseX = g_DeviceInput.GetMousePosition()->x;
			nMouseY = g_DeviceInput.GetMousePosition()->y;
		}

		unsigned long dwFunction;
		if ( pSelfCreature->GetStillCasting() && !m_lpObjControl->CheckStillCasting( dwFunction ) )
		{
			m_lpObjControl->m_bSkillExecuted = TRUE;		
		}

		pSelfCreature->InitCharAction();

		m_lpObjControl->CheckAttackedTiming();
		m_lpObjControl->CheckCameraShakeTiming();

		vector3 vecChrToward = vector3( 0.0f, 0.0f, 0.0f );
		
		if ( bKeyAble )
		{
			bool m_bKeyPress = false;
			CollisionType CT = CSceneManager::GetCharacterCollisionType( pSelfModel );

			if ( !m_bKeyPress )
			{
				//===================================================================
				// 마우스에 픽킹된 캐릭 찾기
				CZ3DGeneralChrModel *pSelectChr = NULL;
				pSelectChr = CSceneManager::PickingCharacter( nMouseX, nMouseY );

				if ( pSelectChr )
				{
					RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pSelectChr );

					if ( pTempCreature )
					{
						if ( pTempCreature->GetIsDead() == TRUE )
						{
							pTempCreature = NULL;
						}
					}

					if ( pTempCreature )
					{
                        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

						if ( pTempModel != pSelfModel )
						{
							vector3 vecLightChrPos;
							pTempModel->GetPosition( vecLightChrPos );
							CSceneManager::AddChrLight( pTempModel, vecLightChrPos + vector3( 0.0f, 200.0f, 0.0f ), 0xff0022ff, 400.0f, 6 );
							
                            if ( ( m_lpObjControl->GetClientEnemyType( pTempCreature->GetCharID() ) == Creature::ET_ENEMY
                               || m_lpObjControl->GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_NPC )
							 && m_lpObjControl->m_bClientUsed 
                             && ( m_bMouseLeftButtonStatePress || KeyPressed( VK_RBUTTON ) ) )
							{	
								vector3		vecResult,vecTargetMonsterPos;
								float		fW;

								pTempModel->GetPosition( vecTargetMonsterPos );						
								vecTargetMonsterPos = vecTargetMonsterPos + vector3( 0.0f, 100.0f, 0.0f );
								BaseGraphicsLayer::TransformVector( vecTargetMonsterPos, vecResult,fW );
								g_DeviceInput.SetCursorPos( ( int )vecResult.x, ( int )vecResult.y );
							}
						}
					}
				}

				//============================================================
				// 클릭해서 이동
				//============================================================
				if ( m_lpObjControl->m_bClickMove )
				{
					vecChrToward   = m_lpObjControl->m_vecClickPos - vecChrPos; 	
					vecChrToward.y = 0.0f;

					if ( vecChrToward.GetLens() <= TARGETNEAR_RANGE )
					{
						m_lpObjControl->m_bClickMove = false;

						if ( m_lpObjControl->m_nSelectItem != 0xffffffff )
						{
							if ( m_lpObjControl->PickItem )
								m_lpObjControl->PickItem( m_lpObjControl->m_nSelectItem );

							m_lpObjControl->m_nSelectItem = 0xffffffff;				 	
						}
						return;
					}

					m_lpObjControl->SetDirectionforMouse( vecChrToward );
                    pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );

					if ( pSelfCreature->GetLowerActionID() == CA_RUN )
						CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * ( 14.0f ) );
				} 
				else if ( m_lpObjControl->m_bClickAttack )
				{					
					m_lpObjControl->SetDirectionforMouse( vecChrToward );
					pSelfCreature->Walk( RYLCreature::MOVE_FOWARD, 0.0f );
					if ( pSelfCreature->GetLowerActionID() == CA_RUN )
                    {
						CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward * ( 14.0f ) );
                    }
				}
				else
				{
					if ( !pSelfCreature->GetIsDead() )
					{
						pSelfCreature->Wait();
					}
					CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward );
				}

				///////////////////////////////////////////////////
				///////////////
				///////////////////////////////////////////////////
				if ( !pSelfCreature->GetIsDead() && ( bMouseLeftButtonStateChange || KeyPressed( VK_LBUTTON ) || KeyPressed( VK_RBUTTON ) ) ) 
				{
					vector3		vecMovePos;
					if ( bMouseLeftButtonStateChange )
					{	
						// 마우스 왼쪽 버튼이 Click됐을 때
						vecMovePos = CSceneManager::PickMousePos( nMouseX, nMouseY );
						if ( vecMovePos.GetLens() <= 10.0f )
							return;
						
						CEffScript* pMousePointEff = new CEffScript;
						CSceneManager::AddEffectScript( pMousePointEff );

						pMousePointEff->GetScriptBinData( "point.esf" );
						pMousePointEff->SetStartPos( vecMovePos.x, vecMovePos.y + 30.0f, vecMovePos.z );			
						pMousePointEff->SetEndPos( vecMovePos.x, vecMovePos.y + 30.0f,vecMovePos.z );

						POINT *ptMousePos = g_DeviceInput.GetMousePosition();
						m_lpObjControl->m_nSelectItem = CSceneManager::MouseOnCheckInstanceObject( ptMousePos->x, ptMousePos->y );				
					}
					else if ( KeyPressed( VK_LBUTTON ) )
					{	// 마우스 왼쪽 버튼이 눌리고 있을때
						CSceneManager::PickMakeRay( nMouseX, nMouseY, BaseGraphicsLayer::m_lScreenSx, BaseGraphicsLayer::m_lScreenSy );
						vector3 vecStart = CSceneManager::m_vecPickRayStart;
						vector3 vecEnd   = CSceneManager::m_vecPickRayStart + 10000.0f * CSceneManager::m_vecPickRayDir;

						vector3 vecPoly[ 4 ];
						float fSize  = 10000.0f;
						vecPoly[ 0 ] = vecChrPos + vector3( -fSize, 0.0f, -fSize );
						vecPoly[ 1 ] = vecChrPos + vector3(  fSize, 0.0f, -fSize );
						vecPoly[ 2 ] = vecChrPos + vector3( -fSize, 0.0f,  fSize );
						vecPoly[ 3 ] = vecChrPos + vector3(  fSize, 0.0f,  fSize );

						float fInter;
						if ( CIntersection::PolygonRay( vecStart, vecEnd, vecPoly, fInter ) )
						{
							vecMovePos = vecStart + ( fInter * CSceneManager::m_vecPickRayDir );
						}
						else if ( CIntersection::PolygonRay( vecStart, vecEnd, &vecPoly[ 1 ], fInter ) )
						{
							vecMovePos = vecStart + ( fInter * CSceneManager::m_vecPickRayDir );
						}
					}

					float	fMaxLens = 200.0f;
					m_lpObjControl->m_dwClickModeAttackTargetID = 0xFFFFFFFF;
					m_lpObjControl->m_vecClickPos = vecMovePos;		
					
					if ( pSelectChr )
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pSelectChr );				
                        
						// 나를 제외하고 픽킹이 되었을 때
						if ( pTempCreature && pTempCreature->GetIsDead() == FALSE ) 
						{			
                            CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

                            if ( pTempModel != pSelfModel )
                            {
							    vector3		vecTargetChrPos, vecLens;
							    pSelectChr->GetPosition( vecTargetChrPos );
							    vecLens = vecTargetChrPos - vecChrPos;
    							
							    if ( vecLens.GetLens() < ATTACKABLE_RANGE )
							    {
								    vector3		vecMoveDirection = vecTargetChrPos - vecChrPos;
								    m_lpObjControl->SetDirectionforMouse( vecMoveDirection );

								    if ( KeyPressed( VK_LBUTTON ) )						
								    {
									    if ( pSelfCreature->Attack() ) 
									    {
										    m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
															                      pSelfCreature->GetLowerActionID(),
                                                                                  pSelfCreature->GetDirection(),
															                      *( pSelfCreature->GetPosition() ) );
									    }
								    }
								    else if ( KeyPressed(VK_RBUTTON) && !pSelfCreature->GetStillCasting())
								    {							
									    if ( m_nSkillType == 0 || m_nSkillType == 1 )
									    {
										    pSelfCreature->SetStillCasting( true );
										    m_lpObjControl->m_dwStartSkillTick = timeGetTime();
										    m_lpObjControl->m_dwCastingSkillTick = 0;
										    m_lpObjControl->m_dwCastingSkillGrade = 0;
									    }
								    }

								    m_lpObjControl->m_bClickAttack = false;
								    m_lpObjControl->m_bClickMove   = false;
							    }
							    else
							    {
								    if ( KeyPressed( VK_RBUTTON ) && m_nSkillType == 1 && !pSelfCreature->GetStillCasting() )
								    {
									    pSelfCreature->SetStillCasting( true );
									    m_lpObjControl->m_dwStartSkillTick		= timeGetTime();
									    m_lpObjControl->m_dwCastingSkillTick	= 0;
									    m_lpObjControl->m_dwCastingSkillGrade	= 0;
								    }
								    else
								    {
									    m_lpObjControl->m_dwClickModeAttackTargetID = pTempCreature->GetCharID();
									    m_lpObjControl->m_bCharacterTargetNear = FALSE;
    									
									    if ( m_lpObjControl->CheckAttackable() )
									    {
										    m_lpObjControl->m_bClickAttack = true;
									    }
									    else
									    {
										    m_lpObjControl->m_bClickAttack = false;
									    }

									    m_lpObjControl->m_bClickMove = false;
								    } 
							    }

							    // 오토 타게팅
							    if ( KeyPressed( VK_CONTROL ) )		
							    {
								    m_lpObjControl->m_dwClickModeAttackTargetID = pTempCreature->GetCharID();
								    m_lpObjControl->m_bCharacterTargetNear		= FALSE;
								    m_lpObjControl->m_bAutoTargetAttack			= true;
								    m_lpObjControl->m_dwAutoTargetID			= m_lpObjControl->m_dwClickModeAttackTargetID;
							    }
							    else
							    {
								    m_lpObjControl->m_bAutoTargetAttack = false;
								    m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;
							    }
                            }
						}
						else
						{
							m_lpObjControl->m_dwClickModeAttackTargetID = 0xFFFFFFFF;
							m_lpObjControl->m_bClickAttack				= false;
							m_lpObjControl->m_bClickMove				= true;
						}
					}
					else
					{
						m_lpObjControl->m_dwClickModeAttackTargetID = 0xFFFFFFFF;
						m_lpObjControl->m_bClickAttack				= false;

						if ( KeyPressed( VK_RBUTTON ) )
                        {
							m_lpObjControl->m_bClickMove = false;
                        }
						else
                        {
							m_lpObjControl->m_bClickMove = true;
                        }
					}
				}
				else
				{			
					if ( m_lpObjControl->m_bAutoTargetAttack == false )
					{
	//					m_pEnemyAttackEff->SetStartPos(0.0f,0.0f,0.0f);
	//					m_pEnemyAttackEff->SetEndPos(0.0f,0.0f,0.0f);
					}
				}
			}
		} 
		else
		{
			//===============================================================
			// 키입력이 먹지 않을때
			if ( !pSelfCreature->GetIsDead() )
			{
				pSelfCreature->Wait();
			}
			CSceneManager::CharacterMovementVector( pSelfModel, vecChrToward );

			m_lpObjControl->m_dwClickModeAttackTargetID = 0xFFFFFFFF;
			m_lpObjControl->m_bClickAttack				= false;
			m_lpObjControl->m_bClickMove				= false;
		}

		m_lpObjControl->PlayerProcessOperation( bKeyAble );

		if ( CFrameTimer::GetUpdateTimer( m_lpObjControl->m_nCharMovingPacketTimerID ) >= 1.0f && pSelfCreature->IsCancelAction( 2 ) )
		{
			vector3 vecMove;
			switch( pSelfCreature->GetLowerActionID() )
			{
				case CA_WALK:
                    {
					    vecMove = vecChrToward * ( m_lpObjControl->m_fWalkSpeed / 4.0f );
                    }
					break;

				case CA_RUN:
                    {
					    vecMove = vecChrToward * ( m_lpObjControl->m_fRunSpeed / 4.0f );
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

			if ( CT == CT_WATER )
			{
				m_lpObjControl->WrapSendMovingPacket( CA_SWIM, CA_SWIM, pSelfCreature->GetDirection(), vecMove );
			}
			else
			{
				m_lpObjControl->WrapSendMovingPacket( pSelfCreature->GetUpperActionID(),
									  pSelfCreature->GetLowerActionID(),
                                      pSelfCreature->GetDirection(), 
                                      vecMove );
			}
		}
	}	
}

RYLCreature*	CRYLGMCharacterControl::MouseClickModeSelectSpecialTarget( RYLCreature* pCreature, int nMouseX, int nMouseY )
{
	RYLCreature* pSpecialCreature = NULL;

	if (m_lpObjControl->CheckSkillStart)
	{
		if (TRUE == m_lpObjControl->CheckSkillStart(TRUE))
		{
			m_bRightMousePress = TRUE;
			m_lpObjControl->SetAutoRun(FALSE);
			m_lpObjControl->m_bAutoFollowMode = FALSE;
			m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
			m_bKeyPress = true;

			if (NULL != pCreature)
			{
				m_dwNormalTargetID	= 0xFFFFFFFF;
				m_dwSpecialTargetID	= pCreature->GetCharID();

				if (m_dwSpecialTargetID != m_lpObjControl->m_dwAutoTargetID)
				{
					m_lpObjControl->m_bAutoTargetAttack = false;
					m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;
				}

				if (m_lpObjControl->CheckTargetforSkill)
				{
					m_lpObjControl->CheckTargetforSkill(m_dwSpecialTargetID, FALSE);
				}

				if (m_dwSpecialTargetID != 0xFFFFFFFF)
				{
					pSpecialCreature = RYLCreatureManager::Instance()->GetCreature(m_dwSpecialTargetID);

					if (!pSpecialCreature)
					{
						m_dwSpecialTargetID = 0xFFFFFFFF;
					}
				}
			} 
			else
			{
				m_dwNormalTargetID	= 0xFFFFFFFF;
				m_dwSpecialTargetID	= 0xFFFFFFFF;
				m_lpObjControl->m_bAutoTargetAttack = false;
				m_lpObjControl->m_dwAutoTargetID	= 0xFFFFFFFF;

				CEffScript *pMousePointEff = new CEffScript;
				CSceneManager::AddEffectScript(pMousePointEff);

				vector3 vecMovePos = CSceneManager::PickMousePos(nMouseX, nMouseY);
				pMousePointEff->GetScriptBinData("point.esf");
				pMousePointEff->SetStartPos(vecMovePos.x, vecMovePos.y + 30.0f, vecMovePos.z);			
				pMousePointEff->SetEndPos(vecMovePos.x, vecMovePos.y + 30.0f, vecMovePos.z);
			}

			return pSpecialCreature;
		}

		if (TRUE == m_lpObjControl->CheckSkillStart(FALSE))
		{
			m_bRightMousePress	= FALSE;
			m_lpObjControl->SetAutoRun(FALSE);
			m_lpObjControl->m_bAutoFollowMode	= FALSE;
			m_lpObjControl->m_dwAutoFollowChrID = 0xFFFFFFFF;
			m_bKeyPress = true;

			if (m_lpObjControl->m_bAutoTargetAttack)
			{
				if (m_dwSpecialTargetID != 0xFFFFFFFF)
				{
					m_dwNormalTargetID = m_dwSpecialTargetID;
				}

				m_lpObjControl->m_bClickMove = false;
				if (m_lpObjControl->CheckAttackable())
				{
					m_lpObjControl->m_bClickAttack = true;
				}
				else
				{
					m_lpObjControl->m_bClickAttack = false;
				}
				m_bClickPlayerAndNpc = FALSE;
			}

			m_dwSpecialTargetID = 0xFFFFFFFF;
			return pSpecialCreature;
		}
	}

	return pSpecialCreature;
}

VOID		CRYLGMCharacterControl::RestoreCamera() 
{
	SetCamera(g_DeviceInput.m_ptMovePos.x, g_DeviceInput.m_ptMovePos.y, -1);
}

VOID		CRYLGMCharacterControl::SetCamera( int dx, int dy, int dz )
{	
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return;

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if ( !pSelfData )
        return;

	// 카메라 각도
	if ( m_lpObjControl->m_nCameraAnimation != 0 )
	{
		// 카메라 애니메이션이 있을 때 (없을땐 그냥 캐릭터 조종모드로 전환)
		return ;
	}

	if ( m_lpObjControl->m_bFreelook)  
	{
		D3DXMATRIX *matPosition = ( D3DXMATRIX* )CSceneManager::GetCamera()->GetMatPosition() ;
		vector3 vecBeforePos    = vector3( matPosition->_41, matPosition->_42, matPosition->_43 ) ;
		float	fDeltaY = dx / 300.0f ;
		float	fDeltaX = dy / 300.0f ;
		
		D3DXVECTOR3 vecT( 0.0f, 0.0f, 0.0f ) ;
		D3DXVECTOR3 vecR( 0.0f, 0.0f, 0.0f ) ;

		D3DXMATRIX		matT, matR ;
		D3DXQUATERNION	qR ;	 	

		if ( KeyPressed( VK_MENU ) )
			return ;
		
		if ( KeyPressed( VK_SHIFT ) )
		{
			if ( KeyPressed( 'A' ) || KeyPressed( VK_LEFT ) )	vecT.x -= m_lpObjControl->m_fCameraSpeed ; // Slide Left			
			if ( KeyPressed( 'D' ) || KeyPressed( VK_RIGHT ) )	vecT.x += m_lpObjControl->m_fCameraSpeed ; // Slide Right
			if ( g_DeviceInput.GetIsRightMousePress() )			vecT.y += m_lpObjControl->m_fCameraSpeed ; // Slide Down
			if ( g_DeviceInput.GetIsLeftMousePress() )			vecT.y -= m_lpObjControl->m_fCameraSpeed ; // Slide Up
			if ( KeyPressed( 'W' ) )							vecT.z += m_lpObjControl->m_fCameraSpeed ; // Move Forward
			if ( KeyPressed( 'S' ) )							vecT.z -= m_lpObjControl->m_fCameraSpeed ; // Move Backward	
			if ( KeyPressed( VK_NUMPAD4 ) )						fDeltaY += 0.008f ;
			if ( KeyPressed( VK_NUMPAD6 ) )						fDeltaY -= 0.008f ;
			if ( KeyPressed( VK_NUMPAD8 ) )						fDeltaX -= 0.008f ;
			if ( KeyPressed( VK_NUMPAD2 ) )						fDeltaX += 0.008f ;
		}
		else
		{
			if ( KeyPressed( 'A' ) || KeyPressed( VK_LEFT ) )	vecT.x -= 10.0f ;			// Slide Left
			if ( KeyPressed( 'D' ) || KeyPressed( VK_RIGHT ) )	vecT.x += 10.0f ;			// Slide Right
			if ( g_DeviceInput.GetIsRightMousePress() )			vecT.y += 10.0f ;			// Slide Down
			if ( g_DeviceInput.GetIsLeftMousePress() )			vecT.y -= 10.0f ;			// Slide Up
			if ( KeyPressed( 'W' ) )
				vecT.z += 10.0f ; // Move Forward
			if ( KeyPressed( 'S' ) )
				vecT.z -= 10.0f ; // Move Backward	

			if ( KeyPressed( VK_NUMPAD4 ) )						fDeltaY += 0.008f ; 
			if ( KeyPressed( VK_NUMPAD6 ) )						fDeltaY -= 0.008f ;
			if ( KeyPressed( VK_NUMPAD8 ) )						fDeltaX -= 0.008f ;
			if ( KeyPressed( VK_NUMPAD2 ) )						fDeltaX += 0.008f ;
		}	
		
		
		float	fRotationX = CSceneManager::GetCamera()->GetRotationX() ;
		float	fRotationY = CSceneManager::GetCamera()->GetRotationY() ;
		
		D3DXVECTOR3		vecPos ;

		vecPos.x = matPosition->_41 ;
		vecPos.y = matPosition->_42 ;
		vecPos.z = matPosition->_43 ;
		
		D3DXMatrixIdentity( matPosition ) ;

		fRotationX -= fDeltaX ;
		fRotationY -= fDeltaY ;

		D3DXQuaternionRotationYawPitchRoll( &qR,fRotationY, fRotationX, 0.0f ) ;
		D3DXMatrixRotationQuaternion( &matR, &qR ) ;
		D3DXMatrixMultiply( matPosition, &matR, matPosition ) ;	
		
		matPosition->_41 = vecPos.x ;
		matPosition->_42 = vecPos.y ;
		matPosition->_43 = vecPos.z ;

		D3DXMatrixTranslation( &matT, vecT.x, vecT.y, vecT.z ) ;
		D3DXMatrixMultiply( matPosition, &matT, matPosition ) ;		

		CSceneManager::GetCamera()->SetRotaitionX( fRotationX ) ;
		CSceneManager::GetCamera()->SetRotaitionY( fRotationY ) ;

		vector3 vecCharPos ;	
		CollisionType CT ;
		vector3 vecOldPos	= vector3( matPosition->_41, matPosition->_42, matPosition->_43 ) ;
		vecCharPos			= vecOldPos ;
		vector3 vecDir		= vecOldPos - vecCharPos ;

		vecCharPos = CSceneManager::m_CollisionDetection.GetPosition( vecOldPos, vecDir, vecBeforePos, CT, true ) ;			
			
		matPosition->_41 = vecCharPos.x ;
		matPosition->_42 = vecCharPos.y ;
		matPosition->_43 = vecCharPos.z ;

        vector3 vecTempPos( CSceneManager::GetCamera()->GetMatPosition()->_41,
							CSceneManager::GetCamera()->GetMatPosition()->_42,
							CSceneManager::GetCamera()->GetMatPosition()->_43 );

        pSelfCreature->SetPosition( vecTempPos );

		CSceneManager::GetCamera()->SetVecPosition( vector3( CSceneManager::GetCamera()->GetMatPosition()->_41,
															 CSceneManager::GetCamera()->GetMatPosition()->_42,
															 CSceneManager::GetCamera()->GetMatPosition()->_43 ) ) ;		

		matrix *matPos=CSceneManager::GetCamera()->GetMatPosition() ;
		matrix matInv ;
		matInv.Inverse( *matPos ) ;
		
		CSceneManager::GetCamera()->SetMatView( matInv ) ;		
		CSceneManager::GetCamera()->MoveFrustum() ;
		return ;
	}

	// m_MouseClickMode가 1일때는 전투모드
	if ( m_lpObjControl->m_bClientUsed )
	{
		dx =- dx ;
		dy =- dy ;

		if ( dx )
		{
			if ( m_lpObjControl->m_MouseClickMode == 1 )
			{
				if ( KeyPressed( VK_MBUTTON ) )
				{
					m_lpObjControl->m_Camera.m_fCameraRotX += ( -dx ) / 300.0f ;
				} 
				else
				{
					float	fMoveDir = dx / 300.0f ;
                    float   fChrDir  = pSelfCreature->GetDirection();

					m_lpObjControl->m_Camera.m_fCameraRotX -= fMoveDir ;					
					fChrDir += fMoveDir ;
					
                    if ( !pSelfCreature->GetIsDead() )
                    {
                        pSelfModel->SetDirection( fChrDir );
                    }
				}
			} 
			else
			{
				m_lpObjControl->m_Camera.m_fCameraRotX += ( -dx ) / 300.0f ;
			}
		}

		if ( dy )
		{
			if ( m_lpObjControl->m_MouseClickMode == 1 )
			{
				m_lpObjControl->m_Camera.m_fBattleCameraRotY += dy / 300.0f ;
			}
			else
			{
				m_lpObjControl->m_Camera.m_fCameraRotY += dy / 300.0f ;
			}
		}

		if ( dz )
		{
			if ( m_lpObjControl->m_MouseClickMode == 1 )
			{
				m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera += dz * 20.0f ;
				
				if ( m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera < 100.0f )  
					m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera = 100.0f ;

				if ( m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera > 1000.0f )
					m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera = 1000.0f ;
			}
			else
			{
				m_lpObjControl->m_Camera.m_fMouseClickInterCharacterCamera += dz * 20.0f ;
				
				if ( m_lpObjControl->m_Camera.m_fMouseClickInterCharacterCamera < 100.0f ) 
					m_lpObjControl->m_Camera.m_fMouseClickInterCharacterCamera = 100.0f ;

				if ( m_lpObjControl->m_Camera.m_fMouseClickInterCharacterCamera > 1000.0f ) 
					m_lpObjControl->m_Camera.m_fMouseClickInterCharacterCamera = 1000.0f ;
			}
		}
	}
	else
	{
		if ( dx )
		{
			if ( m_lpObjControl->m_MouseClickMode == 1 )
			{
				if ( KeyPressed( VK_MBUTTON ) )
				{
					m_lpObjControl->m_Camera.m_fCameraRotX += ( -dx ) / 300.0f ;
				} 
				else
				{
					float	fMoveDir = dx / 300.0f ;
                    float   fChrDir  = pSelfCreature->GetDirection();

					m_lpObjControl->m_Camera.m_fCameraRotX -= fMoveDir ;					
					fChrDir += fMoveDir ;

                    if ( !pSelfCreature->GetIsDead() )
                    {
                        pSelfModel->SetDirection( fChrDir );
                    }
				}
			}
			else
			{
				m_lpObjControl->m_Camera.m_fCameraRotX += ( -dx ) / 300.0f ;
			}
		}
		
		if ( dy )  
		{
			if ( m_lpObjControl->m_MouseClickMode == 1 )
			{
				m_lpObjControl->m_Camera.m_fBattleCameraRotY += ( -dy ) / 300.0f ;
			}
			else
			{
				m_lpObjControl->m_Camera.m_fCameraRotY += (-dy) / 300.0f;
			}			
		}

		if (m_lpObjControl->m_Camera.m_nCameraMode==0)
			m_lpObjControl->m_Camera.m_fBattleInterCharacterCamera += g_DeviceInput.m_lMMoveZ * 10.0f;
		else
			m_lpObjControl->m_Camera.m_fMouseClickInterCharacterCamera += g_DeviceInput.m_lMMoveZ * 10.0f;

		if (dx)
		{
			float fMoveDir = dx/ 300.0f;
            float   fChrDir  = pSelfCreature->GetDirection();

			fChrDir += fMoveDir;
			if (m_lpObjControl->m_MouseClickMode==1)
			{
                if ( !pSelfCreature->GetIsDead() )
                {
                    pSelfModel->SetDirection( fChrDir );
                }
			}
		}
	}	

    vector3 vecChrPos;
	pSelfModel->GetPosition( vecChrPos );

	float fChrDir = pSelfModel->GetDirection();
    fChrDir -= pSelfCreature->GetFixAngle();

	float fInitY = 150.0f;

	if ( pSelfCreature->GetLowerActionID() == CA_SITDOWN || pSelfCreature->GetLowerActionID() == CA_STANDUP )
	{
		if ( pSelfModel )
		{
			CZ3DObject *lpObject = pSelfModel->GetSkeletonPartObject( "CHEST" );

			if ( lpObject != NULL )
			{
				matrix *lpMatrix = lpObject->GetTM();

				if ( lpMatrix != NULL )
				{
					fInitY = lpMatrix->_42; //무조건 ch
				}
			}

            switch ( pSelfData->m_cRace )
			{
				case 0:	// 인간
                    {
					    switch ( pSelfData->m_cSex )
					    {
						    case 1:
                                {
                                    fInitY = fInitY + 34.0f;
                                }
							    break;

						    case 2:
                                {
							        fInitY = fInitY + 30.0f;
                                }
							    break;
					    }
                    }
					break;

				case 1:	// 아칸
                    {
					    switch ( pSelfData->m_cSex )
					    {
						    case 1:
                                {
							        fInitY = fInitY + 6.0f;
                                }
							    break;

						    case 2:
                                {
							        fInitY = fInitY + 19.0f;
                                }
							    break;
					    }
                    }
					break;
			}
		}
	}
	else if ( pSelfCreature->GetLowerActionID() == CA_REST )
	{
        switch ( pSelfData->m_cRace )
		{
			case 0:	// 인간
                {
				    switch ( pSelfData->m_cSex )
				    {
					    case 1:
                            {
						        fInitY = 54.0f;
                            }
						    break;

					    case 2:
                            {
						        fInitY = 62.0f;
                            }
						    break;
				    }
                }
				break;

			case 1:	// 아칸
                {
				    switch ( pSelfData->m_cSex )
				    {
					    case 1:
                            {
						        fInitY = 89.0f;
                            }
						    break;

					    case 2:
                            {
						        fInitY = 176.0f;
                            }
						    break;
				    }
                }
				break;
		}
	}

	if  (m_lpObjControl->m_MouseClickMode == 1 )
	{
		if ( KeyPressed( VK_MBUTTON ) )
		{
			fChrDir = -m_lpObjControl->m_Camera.m_fCameraRotX ;
			m_lpObjControl->m_Camera.UpdateBattleCharacter( vecChrPos, fChrDir, fInitY ) ;
		} 
		else
		{
			fChrDir -= FLOAT_PHI / 2.0f ;
			m_lpObjControl->m_Camera.UpdateBattleCharacter( vecChrPos, fChrDir, fInitY ) ;
		}
	}
	else
		m_lpObjControl->m_Camera.UpdateClickMode( vecChrPos, fInitY ) ;
}