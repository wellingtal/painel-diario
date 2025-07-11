//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreatureEffect.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: single Creature's effect management class
//
// Date: 2004. 4. 8. ( Thu )
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------

#include <Creature/EnemyCheck.h>
#include <Creature/CreatureStructure.h>
#include <Creature/Monster/MonsterMgr.h>

#include <Skill/Spell/SpellKind.h>

#include "SceneManager.h"
#include "FullSceneEffect.h"
#include "RYLCreatureCommon.h"
#include "RYLGameData.h"
#include "RYLCreature.h"
#include "RYLCreatureEffect.h"
#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// fucntor
//--------------------------------------------------------------------------------------------------------------------------
static void ftDeleteBodyEffectMemory( RYLBodyEffect* pBodyEffect )
{
    delete pBodyEffect;
    pBodyEffect = NULL;
}

static void ftDeleteArrowEffectMemory( RYLArrowEffect* pArrowEffect )
{
    delete pArrowEffect;
    pArrowEffect = NULL;
}

static void ftDeleteWeaponEffectMemory( RYLWeaponEffect* pWeaponEffect )
{
    delete pWeaponEffect;
    pWeaponEffect = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreatureEffect
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
RYLCreatureEffect::RYLCreatureEffect( RYLCreature* pMotherCreature )
{
    m_pMotherCreature   = pMotherCreature;
    m_EnchantInfo.ClearFlag();

    ClearEffect();
}

RYLCreatureEffect::~RYLCreatureEffect()
{   
    ClearEffect();
}

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::SetEffectVisibility()
void RYLCreatureEffect::SetEffectVisibility( bool bVisible )
{
    for( list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.begin() ; iter != m_lstBodyEff.end() ; ++iter )
    {
        if( (*iter) && (*iter)->m_pBodyEffScript )
        {
            (*iter)->m_pBodyEffScript->SetVisibility( bVisible );
        }
    }

    for( list<RYLArrowEffect*>::iterator iter = m_lstArrowEff.begin() ; iter != m_lstArrowEff.end() ; ++iter )
    {
        if( (*iter) && (*iter)->m_pArrowEffScript )
        {
            (*iter)->m_pArrowEffScript->SetVisibility( bVisible );
        }
    }

    for( list<RYLWeaponEffect*>::iterator iter = m_lstWeaponEff.begin() ; iter != m_lstWeaponEff.end() ; ++iter )
    {
        if( (*iter) && (*iter)->m_pWeaponEffScript )
        {
            (*iter)->m_pWeaponEffScript->SetVisibility( bVisible );
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::AddBodyEffect()
// 캐릭 pos 에 effect 붙일경우
void RYLCreatureEffect::AddBodyEffect( string strScriptFile, unsigned long ulEffID, bool bMine, bool bVisibility  )
{
    if( !m_pMotherCreature )
        return;

    if( !m_pMotherCreature->GetCharModel() )
		return;

    if( GetChantStatus( Skill::SpellID::Hide ) )              // Hide 된 캐릭터는 이펙트 출력안함
        return;	
	
    if( strScriptFile.empty() )
		return;

	if( CheckInBodyEffect( strScriptFile ) )       // 기존에 같은 종류의 effect 가 있다면 이번것 무시. 
		return;

	RYLBodyEffect*  pTempEffect     = new RYLBodyEffect;
	pTempEffect->m_bPivotRot        = false;
    pTempEffect->m_ulEffID          = ulEffID;

    //-- Effect Script 생성 --//
    CEffScript*     pTempScript     = new CEffScript;
    pTempScript->GetScriptBinData( const_cast<char*>( strScriptFile.c_str() ), bVisibility );
    pTempScript->SetMine( bMine );
    pTempScript->SetPlayer( m_pMotherCreature, m_pMotherCreature->m_ulCharID );
    pTempScript->SetChant( true );

	// 스킬 이펙트 사이즈 설정 
	if ( g_CharacterData.GetClientIdentity( m_pMotherCreature->GetCharID() ) == Creature::CT_MONSTER )
	{
		const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( m_pMotherCreature->GetCharID() & 0x0000FFFF );
		if (NULL != pMonster)
		{
			pTempScript->SetScale( m_pMotherCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
		}
	}

	// 캐릭 방향 생성
	float fCharDir  = -m_pMotherCreature->m_fDirection - ( FLOAT_PHI / 2.0f );
    vector3 vecCharToward( cosf( fCharDir ), 0.0f, sinf( fCharDir ) );
	vecCharToward.Normalize();

    CZ3DGeneralChrModel* pMotherModel = m_pMotherCreature->GetCharModel();

    pTempScript->SetChr( pMotherModel, pMotherModel );
    pTempScript->SetChrFwd( -vecCharToward.x, -vecCharToward.y, -vecCharToward.z );

    // set effect script to effect
    pTempEffect->m_pBodyEffScript   = pTempScript;
    // add effect script to effect script manager        
    CSceneManager::m_EffectManager.AddEffScript( pTempScript );
    // add body effect to vector
    m_lstBodyEff.push_back( pTempEffect );
   // Update Effect
    list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.end();
    if( --iter != m_lstBodyEff.end() )
    {
        BodyEffectUpdate( iter );
    }

    // Set BodyEffect State
    m_bBodyEffExist = true;
}

// pivot 하나에 effect 붙임 ( pivot name으로.. )
void RYLCreatureEffect::AddBodyEffect( string strScriptFile, string strPivot, bool bRot, unsigned long ulEffID  )
{
    if( !m_pMotherCreature )
        return;

    if( !m_pMotherCreature->GetCharModel() )
		return;

    if( GetChantStatus( Skill::SpellID::Hide ) )       // Hide 된 캐릭터는 이펙트 출력안함
        return;	

    if( strScriptFile.empty() || strPivot.empty() )
		return;

	RYLBodyEffect* pTempEffect  = new RYLBodyEffect;
    pTempEffect->m_ulEffID      = ulEffID;
    pTempEffect->m_bPivotRot    = bRot;
    pTempEffect->m_strPivotName = strPivot;
	
    if( !strScriptFile.compare( strScriptFile.length() - 4, 4, ".eff" ) ) // snap eff use
    { 
		pTempEffect->m_bIsUseEff    = true;		
		pTempEffect->m_strEffName   = strScriptFile;
	}
	else //'snap' esf use
    { 
        pTempEffect->m_bIsUseEff    = false;

        CEffScript* pTempScript     = new CEffScript;
        pTempScript->GetScriptBinData( const_cast<char*>( strScriptFile.c_str() ) );
		if( bRot )
        {
            D3DXQUATERNION TempQuat( 0.0f, 0.0f, 0.0f, 1.0f );
			pTempScript->SetExt4SelfInfo( TempQuat );
        }

        // set effect script to effect
        pTempEffect->m_pBodyEffScript   = pTempScript;
        // add effect script to effect script amanger
		CSceneManager::m_EffectManager.AddEffScript( pTempScript );
	}

    // add body effect to vector
    m_lstBodyEff.push_back( pTempEffect );
	// Update Effect
    list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.end();
    if( --iter != m_lstBodyEff.end() )
    {
        BodyEffectUpdate( iter );
    }

    // Set BodyEffect State
    m_bBodyEffExist = true;
}

// pivot 하나에 effect 붙임 ( pivot id로.. )
void RYLCreatureEffect::AddBodyEffect( string strScriptFile, int nPivotID, bool bRot, unsigned long ulEffID  )
{
    if( !m_pMotherCreature )
        return;

    if( !m_pMotherCreature->GetCharModel() )
		return;

    if( GetChantStatus( Skill::SpellID::Hide ) )      // Hide 된 캐릭터는 이펙트 출력안함
        return;

    if( strScriptFile.empty() )
		return;

    CZ3DGeneralChrModel* pMotherModel = m_pMotherCreature->GetCharModel();
    long lPivotNum = 0;
	pMotherModel->GetSkeletonSet( &lPivotNum );

    if( nPivotID < 0 || nPivotID >= lPivotNum ) 
		return;

	RYLBodyEffect* pTempEffect  = new RYLBodyEffect;
    pTempEffect->m_ulEffID      = ulEffID;
    pTempEffect->m_bPivotRot    = bRot;
    pTempEffect->m_nPivotIndex  = nPivotID;
	
	if( !strScriptFile.compare( strScriptFile.length() - 4, 4, ".eff" ) ) // snap eff use
    { 
        pTempEffect->m_bIsUseEff    = true;
		pTempEffect->m_strEffName   = strScriptFile;
	}
	else //'snap' esf use
    { 
		pTempEffect->m_bIsUseEff    = false;
        
        CEffScript* pTempScript     = new CEffScript;
		pTempScript->GetScriptBinData( const_cast<char*>( strScriptFile.c_str() ) );
		if( bRot )
        {
            D3DXQUATERNION TempQuat( 0.0f, 0.0f, 0.0f, 1.0f );
			pTempEffect->m_pBodyEffScript->SetExt4SelfInfo( TempQuat );
        }

        // set effect script to effect
        pTempEffect->m_pBodyEffScript   = pTempScript;
        // add effect script to effect script amanger
		CSceneManager::m_EffectManager.AddEffScript( pTempScript );
	}

    // add body effect to vector
    m_lstBodyEff.push_back( pTempEffect );
	// Update Effect
    list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.end();
    if( --iter != m_lstBodyEff.end() )
    {
        BodyEffectUpdate( iter );
    }

    // Set BodyEffect State
    m_bBodyEffExist = true;
}

//--------------------------------------------------------------------------------------------------------------------------
// GetChantID()
unsigned long GetChantID2( string strScriptFile )
{
	unsigned long  ulChantID = 0;

	if ( !strScriptFile.compare( "Chant_BattleSong.esf" ) )
	{
		ulChantID = Skill::SpellID::BattleSong;
	}
	else if ( !strScriptFile.compare( "Chant_MaintenanceChant.esf" ) )
	{
		ulChantID = Skill::SpellID::MaintenanceChant;
	}
	else if ( !strScriptFile.compare( "Chant_AccelerationChant.esf" ) )
	{
		ulChantID = Skill::SpellID::AccelerationChant;
	}
	else if ( !strScriptFile.compare( "Chant_LifeAura.esf" ) )
	{
		ulChantID = Skill::SpellID::LifeAura;
	}
	else if ( !strScriptFile.compare( "Chant_Blaze.esf" ) )
	{
		ulChantID = Skill::SpellID::Blaze;
	}
	else if ( !strScriptFile.compare( "Chant_Charging.esf" ) )
	{
		ulChantID = Skill::SpellID::Charging;
	}
	else if ( !strScriptFile.compare( "Chant_ManaShell2.esf" ) )
	{
		ulChantID = Skill::SpellID::ManaShell;
	}
	else if ( !strScriptFile.compare( "Chant_Encourage.esf" ) )
	{
		ulChantID = Skill::SpellID::Encourage;
	}
	else if ( !strScriptFile.compare( "Chant_EnchantWeapon.esf" ) )
	{
		ulChantID = Skill::SpellID::EnchantWeapon;
	}
	else if ( !strScriptFile.compare( "Chant_BrightArmor.esf" ) )
	{
		ulChantID = Skill::SpellID::BrightArmor;
	}
	else if ( !strScriptFile.compare( "Chant_HardenSkin.esf" ) )
	{
		ulChantID = Skill::SpellID::HardenSkin;
	}
	else if ( !strScriptFile.compare( "Chant_Flexibility.esf" ) )
	{
		ulChantID = Skill::SpellID::Flexibility;
	}
	else if ( !strScriptFile.compare( "Chant_Guard.esf" ) )
	{
		ulChantID = Skill::SpellID::Guard;
	}
	else if ( !strScriptFile.compare( "Chant_Hold.esf" ) )
	{
		ulChantID = Skill::SpellID::Hold;
	}
	else if ( !strScriptFile.compare( "Chant_Stun.esf" ) )
	{
		ulChantID = Skill::SpellID::Stun;
	}
	else if ( !strScriptFile.compare( "Chant_Frozen.esf" ) )
	{
		ulChantID = Skill::SpellID::Frozen;
	}
	else if ( !strScriptFile.compare( "Chant_Poisoned.esf" ) )
	{
		ulChantID = Skill::SpellID::Poisoned;
	}
	else if ( !strScriptFile.compare( "Chant_LowerStrength.esf" ) )
	{
		ulChantID = Skill::SpellID::LowerStrength;
	}
	else if ( !strScriptFile.compare( "Chant_Fired.esf" ) )
	{
		ulChantID = Skill::SpellID::Fired;
	}
	else if ( !strScriptFile.compare( "Chant_CurseOfBlind.esf" ) )
	{
		ulChantID = Skill::SpellID::CurseOfBlind;
	}
	else if ( !strScriptFile.compare( "PC_Level_100.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "PC_Level_101.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff1;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "PC_Level_102.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff2;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "PC_Level_103.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff3;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "PC_Level_103.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff4;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "PC_Level_103.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff5;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "PC_Level_103.esf" ) )
	{
		ulChantID = Skill::SpellID::KillStreakBuff6;//hz killstreakbuff was Skill::SpellID::HundredLevel;
	}
	else if ( !strScriptFile.compare( "Enchant_Regen.esf" ) )
	{
		ulChantID = Skill::SpellID::Regeneration;
	}
	else if ( !strScriptFile.compare( "Chant_Counter.esf" ) )
	{
		ulChantID = Skill::SpellID::CounterAttack;
	}
	else if ( !strScriptFile.compare( "Chant_Envenum.esf" ) )
	{
		ulChantID = Skill::SpellID::Envenom;
	}
	else if ( !strScriptFile.compare( "Chant_BombSet4.esf" ) )
	{
		ulChantID = Skill::SpellID::BombSet;
	}
	else if ( !strScriptFile.compare( "Heart.esf" ) )
	{
		ulChantID = Skill::SpellID::WifeCracker;
	}
	else if ( !strScriptFile.compare( "Chant_StealHand.esf" ) )
	{
		ulChantID = Skill::SpellID::StealHand;
	}
	/*else if ( !strScriptFile.compare( "PC_Level_100.esf" ) )
	{
		ulChantID = Skill::SpellID::Clairvoyance;
	}*/ //hz killstreak buff was not commented out.
	return ulChantID;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatrueEffect::DeleteBodyEffect()
void RYLCreatureEffect::DeleteBodyEffect()
{
	for_each( m_lstBodyEff.begin(), m_lstBodyEff.end(), ftDeleteBodyEffectMemory );
	m_lstBodyEff.clear();

	m_bBodyEffExist = false;
}

void RYLCreatureEffect::DeleteBodyEffect( string strPivot )
{
    if( !m_lstBodyEff.empty() )
    {
        for( list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.begin() ; iter != m_lstBodyEff.end() ; )
        {
            if( !(*iter) )
            {
                ++iter;
                continue;
            }

            if( !strPivot.compare( (*iter)->m_strPivotName ) )
            {       
                delete (*iter);
                (*iter) = NULL;

                iter = m_lstBodyEff.erase( iter );
            }
            else
            {
                ++iter;
            }
        }

        if( m_lstBodyEff.size() == 0 )
	    {
		    m_bBodyEffExist = false;
	    }
    }
}

void RYLCreatureEffect::DeleteBodyEffect( const unsigned long ulEffID )
{
    if( !m_lstBodyEff.empty() )
    {
        for( list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.begin() ; iter != m_lstBodyEff.end() ; )
        {
            if( !(*iter) )
            {
                ++iter;
                continue;
            }

            if( (*iter)->m_ulEffID == ulEffID )
            {       
                delete (*iter);
                (*iter) = NULL;

                iter = m_lstBodyEff.erase( iter );
            }
            else
            {
                ++iter;
            }
        }

        if( m_lstBodyEff.size() == 0 )
	    {
		    m_bBodyEffExist = false;
	    }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatrueEffect::DeleteBodyEffectName()
void RYLCreatureEffect::DeleteBodyEffectName( string strScriptFile )
{
    for( list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.begin() ; iter != m_lstBodyEff.end() ; )
    {
        if( (*iter) )
        {
            if( !CSceneManager::m_EffectManager.CheckNullScript( (*iter)->m_pBodyEffScript ) ) 
            {
                // if effect is registered in effect script list, check its name and delete
                if( !strScriptFile.compare( (*iter)->m_pBodyEffScript->m_FileName ) )
                {
                    delete (*iter);
                    (*iter) = NULL;

                    iter = m_lstBodyEff.erase( iter );
                }
                else
                {
                    ++iter;
                }
            }
            else
            {
                // if effect is not registered in effect script list
                delete (*iter);
                (*iter) = NULL;

                iter = m_lstBodyEff.erase( iter );
            }
        }
        else
        {
            iter = m_lstBodyEff.erase( iter );
        }
    }

	if( m_lstBodyEff.size() == 0 )
	{
		m_bBodyEffExist= false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::BodyEffectUpdate()
void RYLCreatureEffect::BodyEffectUpdate()
{
	if( m_bBodyEffExist )
	{
		for( list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.begin() ; iter != m_lstBodyEff.end() ; ++iter )
		{
			if (NULL != *iter)
			{
				if( true == (*iter)->m_bIsUseEff || false == CSceneManager::m_EffectManager.CheckNullScript( (*iter)->m_pBodyEffScript ) )
				{
					BodyEffectUpdate( iter, true );
				}
				else 
				{
					if( true == CSceneManager::m_EffectManager.CheckNullScript( (*iter)->m_pBodyEffScript ) ) 
					{
						delete (*iter);

						(*iter) = NULL;	
						iter = m_lstBodyEff.erase( iter );
					}
				}
			}
		}
	}
}

void RYLCreatureEffect::BodyEffectUpdate( list<RYLBodyEffect*>::iterator& iterEffect, bool bAllUpdate )
{
    if( !m_pMotherCreature )
        return;

    if( iterEffect!= m_lstBodyEff.end() && (*iterEffect) != NULL ) 
    {
        D3DXVECTOR3 D3DVecBodyPos;	
		D3DXVECTOR3 D3DVecEffectFwd;

		//modified by Ppu // CZ3DObject body;
		CZ3DObject* body;
		CZ3DSkeletonObject *pPivotArray = NULL;
		long nPivotNum = 0;
		
        CZ3DGeneralChrModel* pMotherModel = m_pMotherCreature->GetCharModel();

		RYLBodyEffect* pbEffect = (*iterEffect);

		if( pMotherModel != NULL ) 
        {
			if( (*iterEffect)->m_nPivotIndex != -1 ) 
            {
				pPivotArray = pMotherModel->GetSkeletonSet( &nPivotNum );
			}
            else if( (*iterEffect)->m_strPivotName.length() > 1 )
            {
				CZ3DObject *pTempObject = pMotherModel->GetSkeletonPartObject( (*iterEffect)->m_strPivotName.c_str() );

				if( !pTempObject ) 
                {	
					delete (*iterEffect);
					(*iterEffect) = NULL;	

					iterEffect = m_lstBodyEff.erase( iterEffect );

					return;
				}
				
				body = pTempObject;
			}
			else 
            {
				vector3 charpos = pMotherModel->GetPosition();

				(*iterEffect)->m_pBodyEffScript->SetStartPos( charpos.x, charpos.y, charpos.z );

				float fChrDir = m_pMotherCreature->m_fDirection = pMotherModel->GetDirection();
				fChrDir = -fChrDir;
				fChrDir -= FLOAT_PHI / 2.0f;

				vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
				vecChrToward.Normalize();

				(*iterEffect)->m_pBodyEffScript->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );	

				if( pMotherModel != NULL  ) 
                {
					// 몬스터 이펙트 사이즈 설정 - Vincent
					if ( CRYLGameData::GetClientIdentity( m_pMotherCreature->GetCharID() ) == Creature::CT_MONSTER )
					{
						const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( m_pMotherCreature->GetCharID() & 0x0000FFFF );
						if (NULL != lpMonster)
						{
							float fEffectScale = pMotherModel->GetEffectScale();
							fEffectScale *= lpMonster->m_MonsterInfo.m_fSkillEffectSize ;
							(*iterEffect)->m_pBodyEffScript->SetScale( fEffectScale );
						}
					}
				}

				return ;
			}

			if( 1 ) 
            {
				vector3 body_tm;
				quaternion body_q;
				D3DXQUATERNION tmp;
				if( (*iterEffect)->m_nPivotIndex!=-1 ) 
                {
					pPivotArray[  (*iterEffect)->m_nPivotIndex  ].GetPosition( body_tm );
					pPivotArray[  (*iterEffect)->m_nPivotIndex  ].GetDirection( body_q );
				}
				else 
                {
					body->GetPosition( body_tm );
					body->GetDirection( body_q );
				}

				float body_scale = 0.0f;
				body_scale = pMotherModel->GetScale();

				D3DVecBodyPos.x = body_tm.x;
				D3DVecBodyPos.y = body_tm.y;
				D3DVecBodyPos.z = body_tm.z;
				
				D3DVecBodyPos *= body_scale;

				tmp.x = body_q.x;
				tmp.y = body_q.y;
				tmp.z = body_q.z;
				tmp.w = body_q.w;

				if( (*iterEffect)->m_bIsUseEff ) 
                { //(*iterEffect) use eff file 
                    //snap effect render( 30.0f : effect 간격 )
                    if( bAllUpdate )
                    {
                        RenderSnap( (*iterEffect)->m_D3DVecEffPos.x, 
                                    (*iterEffect)->m_D3DVecEffPos.y, 
                                    (*iterEffect)->m_D3DVecEffPos.z,
								    m_pMotherCreature->GetPosition()->x + D3DVecBodyPos.x, 
                                    m_pMotherCreature->GetPosition()->y + D3DVecBodyPos.y, 
                                    m_pMotherCreature->GetPosition()->z + D3DVecBodyPos.z,
                                    30.0f, (*iterEffect)->m_strEffName.c_str() );
                    }

					(*iterEffect)->m_D3DVecEffPos.x = m_pMotherCreature->GetPosition()->x + D3DVecBodyPos.x;
					(*iterEffect)->m_D3DVecEffPos.y = m_pMotherCreature->GetPosition()->y + D3DVecBodyPos.y;
					(*iterEffect)->m_D3DVecEffPos.z = m_pMotherCreature->GetPosition()->z + D3DVecBodyPos.z;
				}
				else 
                { 
					// (*iterEffect) use esf file
					if( (*iterEffect)->m_bPivotRot )	
						(*iterEffect)->m_pBodyEffScript->SetExt4Rot( tmp );
				
					(*iterEffect)->m_pBodyEffScript->SetStartPos( m_pMotherCreature->GetPosition()->x + D3DVecBodyPos.x, 
                                                            m_pMotherCreature->GetPosition()->y + D3DVecBodyPos.y, 
                                                            m_pMotherCreature->GetPosition()->z + D3DVecBodyPos.z );

					float fChrDir = m_pMotherCreature->m_fDirection = pMotherModel->GetDirection();
					fChrDir = -fChrDir;
					fChrDir -= FLOAT_PHI / 2.0f;

					vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
					vecChrToward.Normalize();

					(*iterEffect)->m_pBodyEffScript->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );	

					// 몬스터 이펙트 사이즈 설정 - Vincent
					if ( CRYLGameData::GetClientIdentity( m_pMotherCreature->GetCharID() ) == Creature::CT_MONSTER )
					{
						const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( m_pMotherCreature->GetCharID() & 0x0000FFFF );
						if (NULL != lpMonster)
						{
							float fEffectScale = pMotherModel->GetEffectScale();
							fEffectScale *= lpMonster->m_MonsterInfo.m_fSkillEffectSize ;
							(*iterEffect)->m_pBodyEffScript->SetScale( fEffectScale );
						}
					}
				}		
			}
			else 
            {	
				delete (*iterEffect);
				(*iterEffect) = NULL;	
				iterEffect = m_lstBodyEff.erase( iterEffect );
			}
		}
		else 
        {	
			delete (*iterEffect);
			(*iterEffect) = NULL;	
			iterEffect = m_lstBodyEff.erase( iterEffect );
        }
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::SetEffect()
void RYLCreatureEffect::SetEffect()
{
    // 나중에 character에 effect 붙일때 쓰는 코드 
    // GCMDS 에서 캐릭에 붙는 effect filename과 pivot 명을 넘겨줌
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::MonsterEffectProcess()
void RYLCreatureEffect::MonsterEffectProcess( const char* pstrFileName, int nEffectValue )
{
    if( !m_pMotherCreature )
        return;

    if( nEffectValue == 0 ) // Attack 관련 Effect
    {
        if( !pstrFileName || strlen( pstrFileName ) <=1 )
            return;

        CZ3DGeneralChrModel* pModel = m_pMotherCreature->GetCharModel();
        CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( m_pMotherCreature->GetCharID() );

        if( !pModel || !pData )
            return;

        unsigned long ulTarget = pData->m_ulTargetCharID;
        
		CEffScript *pInputEffect = NULL;

		if( ulTarget == 0xFFFFFFFF )
		{
			float fChrDir = m_pMotherCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
			vecChrToward.Normalize();

			pInputEffect = new CEffScript;
			
			pInputEffect->GetScriptBinData( const_cast<char*>( pstrFileName ) );//,true);
			pInputEffect->SetStartPos( m_pMotherCreature->GetPosition()->x, m_pMotherCreature->GetPosition()->y, m_pMotherCreature->GetPosition()->z );
			pInputEffect->SetEndPos( (D3DXVECTOR3*)m_pMotherCreature->GetPosition(), 1 );
			pInputEffect->SetPlayer( m_pMotherCreature, m_pMotherCreature->GetCharID() );
			pInputEffect->SetChr( pModel, pModel );
			pInputEffect->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
			pInputEffect->SetMine( true );

		}
		else
		{
			pData->m_ulTargetCharID = 0xFFFFFFFF;

			RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( ulTarget );
			CZ3DGeneralChrModel* pTargetModel = pTargetCreature->GetCharModel();

			if( !pTargetCreature )
				return;

			// 캐릭 방향 생성
			float fChrDir = m_pMotherCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
			vecChrToward.Normalize();

			pInputEffect = new CEffScript;
			
			pInputEffect->GetScriptBinData( const_cast<char*>( pstrFileName ) );//,true);
			pInputEffect->SetStartPos( m_pMotherCreature->GetPosition()->x, m_pMotherCreature->GetPosition()->y, m_pMotherCreature->GetPosition()->z );
			pInputEffect->SetEndPos( (D3DXVECTOR3*)pTargetCreature->GetPosition(), 1 );
			pInputEffect->SetPlayer( m_pMotherCreature, m_pMotherCreature->GetCharID() );
			pInputEffect->SetChr( pModel, pTargetModel );
			pInputEffect->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
			pInputEffect->SetMine( true );


		}
		/*	wizardbug 12.8.2004
        if( ulTarget == 0xFFFFFFFF )
            return;
        
        pData->m_ulTargetCharID = 0xFFFFFFFF;

        RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( ulTarget );
        CZ3DGeneralChrModel* pTargetModel = pTargetCreature->GetCharModel();

        if( !pTargetCreature )
            return;

        // 캐릭 방향 생성
        float fChrDir = m_pMotherCreature->GetDirection();
		fChrDir = -fChrDir;
		fChrDir -= FLOAT_PHI / 2.0f;

		vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
		vecChrToward.Normalize();

		CEffScript *pInputEffect = new CEffScript;
		
		pInputEffect->GetScriptBinData( const_cast<char*>( pstrFileName ) );//,true);
		pInputEffect->SetStartPos( m_pMotherCreature->GetPosition()->x, m_pMotherCreature->GetPosition()->y, m_pMotherCreature->GetPosition()->z );
        pInputEffect->SetEndPos( (D3DXVECTOR3*)pTargetCreature->GetPosition(), 1 );
		pInputEffect->SetPlayer( m_pMotherCreature, m_pMotherCreature->GetCharID() );
		pInputEffect->SetChr( pModel, pTargetModel );
		pInputEffect->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
		pInputEffect->SetMine( true );
		*/
        CSceneManager::m_EffectManager.AddEffScript( pInputEffect );
	}
	else if( nEffectValue == 1 ) // Attack 이외의 이벤트 시에 나오는 Effect
	{
		if( !pstrFileName || strlen( pstrFileName ) <= 1 )
			return;

		if(strstr(pstrFileName,"MONWORLD_footsound.esf"))
		{
			CEffScript *pInputEffect = NULL;

			CZ3DGeneralChrModel* pModel = m_pMotherCreature->GetCharModel();
		
			float fChrDir = m_pMotherCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
			vecChrToward.Normalize();

			pInputEffect = new CEffScript;
			
			pInputEffect->GetScriptBinData( const_cast<char*>( pstrFileName ) );//,true);
			pInputEffect->SetStartPos( m_pMotherCreature->GetPosition()->x, m_pMotherCreature->GetPosition()->y, m_pMotherCreature->GetPosition()->z );
			pInputEffect->SetEndPos( (D3DXVECTOR3*)m_pMotherCreature->GetPosition(), 1 );
			pInputEffect->SetPlayer( m_pMotherCreature, m_pMotherCreature->GetCharID() );
			pInputEffect->SetChr( pModel, pModel );
			pInputEffect->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
			pInputEffect->SetMine( true );
			CSceneManager::m_EffectManager.AddEffScript( pInputEffect );

			return;

		}
		// by Hades Kang - option dialog < chant effect >
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( !pSelfCreature )
            return;

		int index;
        if( m_pMotherCreature->GetCharID() == pSelfCreature->GetCharID() )
		{
			index = 0;
		}
		else
		{
			switch ( g_CharacterData.GetClientEnemyType(  m_pMotherCreature->GetCharID()  ) )
			{
				case EnemyCheck::EC_ENEMY:		index = 3;	break;
				case EnemyCheck::EC_FRIEND:		index = 1;	break;
				case EnemyCheck::EC_NEUTRAL:	index = 2;	break;
			}
		}
		 
		unsigned long dwChantID = GetChantID2( pstrFileName ) ;
		
		if ( dwChantID )
			SetChantEffect( dwChantID, false, true, g_CharacterData.m_bGoodChantEffectShow[ index ], g_CharacterData.m_bBadChantEffectShow[ index ] );
		else
			AddBodyEffect( pstrFileName, static_cast<unsigned long>( 0 ), true, g_CharacterData.m_bGoodChantEffectShow[ index ] );
    }
}


//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::GetChantStatus()
bool RYLCreatureEffect::GetChantStatus( unsigned long ulChantKind )
{
    return m_EnchantInfo.GetFlag( ulChantKind );
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::SetChantEffect()
void RYLCreatureEffect::SetChantEffect( bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow )
{
    SetChantEffect( m_EnchantInfo, bSelf, bGoodChantEffectShow, bBadChantEffectShow );
}

void RYLCreatureEffect::SetChantEffect(const EnchantInfo& Info, bool bSelf, 
                                       bool bGoodChantEffectShow, bool bBadChantEffectShow )
{
    bool bNewStatus = false;

    for (unsigned long i = 0 ; i < Skill::SpellID::MAX_SPELL_TYPE ; ++i)
	{

        bNewStatus = Info.GetFlag(i);
        
        if (m_EnchantInfo.GetFlag(i) ^ bNewStatus)
		{
            // XOR연산을 통해, 상태가 바뀐 경우만 상태를 바꾸어 준다.
			SetChantEffect(i, bNewStatus, 
                bSelf, bGoodChantEffectShow, bBadChantEffectShow);
		}
	}
				
	m_EnchantInfo = Info;
}

void RYLCreatureEffect::SetChantEffect( unsigned long ulChant, bool bToggle, bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow )
{
    if (0 == m_pMotherCreature || 0 == m_pMotherCreature->m_pCharModel)
    {
        return;
    }

    if( bToggle )
	{
		// 인첸트의 이팩트 처리
		switch( ulChant )
		{
			case Skill::SpellID::BattleSong:
                {
					AddBodyEffect( "Chant_BattleSong.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::MaintenanceChant:
                {
				    AddBodyEffect( "Chant_MaintenanceChant.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::AccelerationChant:
                {
				    AddBodyEffect( "Chant_AccelerationChant.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::LifeAura:
                {
				    AddBodyEffect( "Chant_LifeAura.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::Regeneration: // Regeneration
                {
				    AddBodyEffect( "Enchant_Regen.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::Blaze:
                {
				    AddBodyEffect( "Chant_Blaze.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );	

				    if( bSelf ) 
                    {
					    CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_MOTIONBLUR,true );
					    CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR,true );
				    }
                }
				break;

			case Skill::SpellID::Charging:
                {
				    AddBodyEffect( "Chant_Charging.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );	

				    if( bSelf ) 
                    {
					    CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_MOTIONBLUR,true );
					    CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR,true );
				    }
                }
				break;

			case Skill::SpellID::Stealth:	// Stealth
                {
				    if( bSelf )
					    CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_NEGATIVE,true );
                }
				break;

			case Skill::SpellID::ManaShell:
                {
				    AddBodyEffect( "Chant_ManaShell2.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;
			
			case Skill::SpellID::Encourage:
                {
				    AddBodyEffect( "Chant_Encourage.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::EnchantWeapon:
                {
				    AddBodyEffect( "Chant_EnchantWeapon.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::BrightArmor:
                {
				    AddBodyEffect( "Chant_BrightArmor.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::HardenSkin:
                {
				    AddBodyEffect( "Chant_HardenSkin.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::Flexibility:
                {
				    AddBodyEffect( "Chant_Flexibility.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::Guard:
                {
				    AddBodyEffect( "Chant_Guard.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::Hold:
                {
				    AddBodyEffect( "Chant_Hold.esf", static_cast<unsigned long>( 0 ), bSelf, bBadChantEffectShow );
                }
				break;

			case Skill::SpellID::Stun:
                {
				    AddBodyEffect( "Chant_Stun.esf", static_cast<unsigned long>( 0 ), bSelf, bBadChantEffectShow );
                }
				break;

			case Skill::SpellID::Frozen:
                {
				    AddBodyEffect( "Chant_Frozen.esf", static_cast<unsigned long>( 0 ), bSelf, bBadChantEffectShow );
                }
				break;

			case Skill::SpellID::Poisoned:
                {
				    AddBodyEffect( "Chant_Poisoned.esf", static_cast<unsigned long>( 0 ), bSelf, bBadChantEffectShow );
                }
				break;

			case Skill::SpellID::LowerStrength:
                {
				    AddBodyEffect( "Chant_LowerStrength.esf", static_cast<unsigned long>( 0 ), bSelf, bBadChantEffectShow );
                }
				break;

			case Skill::SpellID::StoneForm: // StoneForm
				{
					m_pMotherCreature->m_pCharModel->SetRenderOverride(Z3DRO_STONE);
					m_pMotherCreature->m_pCharModel->Pause(true);
				}
				break;

			case Skill::SpellID::CounterAttack: // CounterAttack
                {
				    AddBodyEffect( "Chant_Counter.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::Envenom: // Envenom
                {
				    AddBodyEffect( "Chant_Envenum.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

			case Skill::SpellID::BombSet: // BomeSet
                {
				    AddBodyEffect( "Chant_BombSet4.esf", static_cast<unsigned long>( 0 ), bSelf, bBadChantEffectShow );
                }
				break;

			case Skill::SpellID::WifeCracker:
                {
				    AddBodyEffect( "Heart.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;
			case Skill::SpellID::Fired:
				{
					AddBodyEffect( "Chant_Fired.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
				}
				break;
			case Skill::SpellID::CurseOfBlind:
				{
					AddBodyEffect( "Chant_CurseOfBlind.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
				}
				break;
			case Skill::SpellID::KillStreakBuff:
				{
						AddBodyEffect( "PC_Level_100.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
				}
				break;
			case Skill::SpellID::KillStreakBuff1:
				{
						DeleteBodyEffectName( "PC_Level_100.esf" );
						AddBodyEffect( "PC_Level_101.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
						break;
				}
				break;
			case Skill::SpellID::KillStreakBuff2:
				{
						DeleteBodyEffectName( "PC_Level_101.esf" );
						AddBodyEffect( "PC_Level_102.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
						break;
				}
				break;
			case Skill::SpellID::KillStreakBuff3:
				{
						DeleteBodyEffectName( "PC_Level_102.esf" );
						AddBodyEffect( "PC_Level_103.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
						break;
				}
				break;
			case Skill::SpellID::KillStreakBuff4:
				{
						DeleteBodyEffectName( "PC_Level_103.esf" );
						AddBodyEffect( "PC_Level_104.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
						break;
				}
				break;
			case Skill::SpellID::KillStreakBuff5:
				{
						DeleteBodyEffectName( "PC_Level_104.esf" );
						AddBodyEffect( "PC_Level_105.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
						break;
				}
				break;
			case Skill::SpellID::KillStreakBuff6:
				{
						DeleteBodyEffectName( "PC_Level_105.esf" );
						AddBodyEffect( "PC_Level_106.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
						break;
				}
				break;

			case Skill::SpellID::StealHand:
                {
					AddBodyEffect( "Chant_StealHand.esf", static_cast<unsigned long>( 0 ), bSelf, bGoodChantEffectShow );
                }
				break;

		}

        m_EnchantInfo.SetFlag( ulChant );
	} 
    else
	{
		switch(ulChant)
		{
			case Skill::SpellID::BattleSong:
                {
				    DeleteBodyEffectName( "Chant_BattleSong.esf" );
                }
				break;

			case Skill::SpellID::MaintenanceChant:
                {
				    DeleteBodyEffectName( "Chant_MaintenanceChant.esf" );
                }
				break;

			case Skill::SpellID::AccelerationChant:
                {
				    DeleteBodyEffectName( "Chant_AccelerationChant.esf" );
                }
				break;

			case Skill::SpellID::LifeAura:
                {
				    DeleteBodyEffectName( "Chant_LifeAura.esf" );
                }
				break;

			case Skill::SpellID::Regeneration: // Regeneration
                {
				    DeleteBodyEffectName( "Enchant_Regen.esf" );
                }
				break;

			case Skill::SpellID::Blaze:
                {
				    DeleteBodyEffectName( "Chant_Blaze.esf" );	
				    DeleteBodyEffectName( "blaze_ring_foot.esf" );		// 첸트에서 사용하는 snap을 따로 지워준다
				    if( bSelf ) 
                    {
					    CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR,false );
					    CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_MOTIONBLUR,false );					
				    }
                }
				break;

			case Skill::SpellID::Charging:
                {
				    DeleteBodyEffectName( "Chant_Charging.esf" );	
				    DeleteBodyEffectName( "charging_ring_foot.esf" );		// 첸트에서 사용하는 snap을 따로 지워준다
				    if( bSelf ) 
                    {
					    CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR,false );				
					    CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_MOTIONBLUR,false );
				    }
                }
				break;

			case Skill::SpellID::Stealth:	// Stealth
                {
				    if( bSelf )
					    CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_NEGATIVE,false );
                }
				break;

			case Skill::SpellID::ManaShell:
                {
				    DeleteBodyEffectName( "Chant_ManaShell2.esf" );
                }
				break;
			
			case Skill::SpellID::Encourage:
                {
				    DeleteBodyEffectName( "Chant_Encourage.esf" );
                }
				break;

			case Skill::SpellID::EnchantWeapon:
                {
				    DeleteBodyEffectName( "Chant_EnchantWeapon.esf" );
                }
				break;

			case Skill::SpellID::BrightArmor:
                {
				    DeleteBodyEffectName( "Chant_BrightArmor.esf" );
                }
				break;

			case Skill::SpellID::HardenSkin:
                {
    				DeleteBodyEffectName( "Chant_HardenSkin.esf" );
                }
				break;

			case Skill::SpellID::Flexibility:
                {
				    DeleteBodyEffectName( "Chant_Flexibility.esf" );
                }   
				break;

			case Skill::SpellID::Guard:
                {
				    DeleteBodyEffectName( "Chant_Guard.esf" );
                }
				break;

			case Skill::SpellID::Hold:
                {
				    DeleteBodyEffectName( "Chant_Hold.esf" );
                }
				break;

			case Skill::SpellID::Stun:
                {
				    DeleteBodyEffectName( "Chant_Stun.esf" );
                }
				break;

			case Skill::SpellID::Frozen:
                {
				    DeleteBodyEffectName( "Chant_Frozen.esf" );
                }
				break;

			case Skill::SpellID::Poisoned:
                {
				    DeleteBodyEffectName( "Chant_Poisoned.esf" );
                }
				break;

			case Skill::SpellID::LowerStrength:
                {
				    DeleteBodyEffectName( "Chant_LowerStrength.esf" );
                }
				break;

			case Skill::SpellID::StoneForm: // StoneForm
				{
					m_pMotherCreature->m_pCharModel->SetRenderOverride(Z3DRO_NONE);
					m_pMotherCreature->m_pCharModel->Pause(false);
				}
				break;

			case Skill::SpellID::CounterAttack: // CounterAttack
                {
				    DeleteBodyEffectName( "Chant_Counter.esf" );
                }
				break;

			case Skill::SpellID::Envenom: // Envenom
                {
				    DeleteBodyEffectName( "Chant_Envenum.esf" );
                }
				break;

			case Skill::SpellID::BombSet: // BomeSet
                {
				    DeleteBodyEffectName( "Chant_BombSet4.esf" );
                }
				break;


			case Skill::SpellID::WifeCracker:
                {
				    DeleteBodyEffectName( "Heart.esf" );
                }
				break;
			case Skill::SpellID::Fired:
				{
					DeleteBodyEffectName( "Chant_Fired.esf" );
				}
				break;
			case Skill::SpellID::CurseOfBlind:
				{
					DeleteBodyEffectName( "Chant_CurseOfBlind.esf" );
				}
				break;

				// edith 100랩 달성이팩트
			case Skill::SpellID::KillStreakBuff: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_100.esf" );
				}
				break;

			case Skill::SpellID::KillStreakBuff1: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_101.esf" );
				}
				break;
			case Skill::SpellID::KillStreakBuff2: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_102.esf" );
				}
				break;
			case Skill::SpellID::KillStreakBuff3: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_103.esf" );
				}
				break;
			case Skill::SpellID::KillStreakBuff4: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_104.esf" );
				}
				break;
			case Skill::SpellID::KillStreakBuff5: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_105.esf" );
				}
				break;
			case Skill::SpellID::KillStreakBuff6: // HundredLevel
				{
						DeleteBodyEffectName( "PC_Level_106.esf" );
				}
				break;
			case Skill::SpellID::StealHand:
                {
				    DeleteBodyEffectName( "Chant_StealHand.esf" );
                }
				break;

		}

		m_EnchantInfo.ResetFlag( ulChant );
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::CheckInBodyEffect()
bool RYLCreatureEffect::CheckInBodyEffect( string strScriptFile )
{
    if( strScriptFile.empty() )
		return false;

    for( list<RYLBodyEffect*>::iterator iter = m_lstBodyEff.begin() ; iter != m_lstBodyEff.end() ; ++iter )
    {
        if( !(*iter)->m_pBodyEffScript )
            continue;

        if( !strScriptFile.compare( (*iter)->m_pBodyEffScript->m_FileName ) ) 
			return true;
    }

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatrueEffect::RenderSnap()
void RYLCreatureEffect::RenderSnap( float fBeforex, float fBeforey, float fBeforez, float fNowx, float fNowy, float fNowz, float fUnitsize, const char *pstrEffName )
{
    // snap effect 중 eff 를 이용하는 것들.
	//unitsize : 이팩트 사이 간격

	float   vec_length  = 0.0f;
	int     unitcount   = 0;

	D3DXVECTOR3 tmp_vec;
	D3DXVECTOR3 tmp_vec2( 0.0f, 0.0f, 0.0f );

	tmp_vec.x = fNowx - fBeforex;
	tmp_vec.y = fNowy - fBeforey;
	tmp_vec.z = fNowz - fBeforez;
	
	vec_length = D3DXVec3Length( &tmp_vec );
	D3DXVec3Normalize( &tmp_vec, &tmp_vec );

	while(vec_length > D3DXVec3Length( &tmp_vec2 ) ) 
    { 
        D3DXVECTOR3 effectpos( fNowx,fNowy,fNowz );
		
		tmp_vec2 += tmp_vec * fUnitsize;

		//rfoot
		CX3DEffect *pEffect = new CX3DEffect;
		CSceneManager::AddEffect( pEffect );
		pEffect->Load( EFFECTPATH, pstrEffName );
		pEffect->SetLoop( FALSE );				
		pEffect->SetAxis( 0.0f,0.0f,0.0f );

		if( vec_length < 1.0f ) 
        {
			pEffect->SetCenter( effectpos.x, effectpos.y, effectpos.z );
		}
		else 
        { 
			pEffect->SetCenter( effectpos.x - tmp_vec2.x,effectpos.y - tmp_vec2.y, effectpos.z - tmp_vec2.z );
		}

		pEffect->SetFrame( ( float )( unitcount * 2.0f ) );

		unitcount++;

		if( vec_length < 1.0f )
			break;

	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::AddArrow()
void RYLCreatureEffect::AddArrow( const char* pstrPivotName, D3DXQUATERNION Quat, int nDiv )
{
    RYLArrowEffect* pTempEffect = new RYLArrowEffect;
    pTempEffect->m_strPivotName = pstrPivotName;

	if( m_lstArrowEff.size() > 2 ) 
    { 
        // 화살갯수 제한
        list<RYLArrowEffect*>::iterator iter = m_lstArrowEff.begin();

        delete (*iter);
        (*iter) = NULL;

        iter = m_lstArrowEff.erase( iter );
	}

	CEffScript* pTempScript = new CEffScript;
	pTempScript->SetExt4SelfInfo( Quat );
	switch( nDiv ) 
    {
	case 0:
        {
		    if( rand() % 2 )
            {
			    pTempScript->GetScriptBinData( "arrow_body.esf" );
            }
		    else
            {
			    pTempScript->GetScriptBinData( "arrow_body2.esf" );
            }
        }
		break;

	case 1:
        {
		    pTempScript->GetScriptBinData( "dagger_body1.esf" );
        }
		break;

	case 2:
        {
		    pTempScript->GetScriptBinData( "dagger_body2.esf" );
        }
		break;

	case 3:
        {
		    pTempScript->GetScriptBinData( "dagger_body3.esf" );
        }
		break;	

	case 4:
        {
		    pTempScript->GetScriptBinData( "dagger_body4.esf" );
        }
		break;

	case 5:
        {
		    pTempScript->GetScriptBinData( "dagger_body5.esf" );
        }
		break;
	
	}

    // set effect script to effect
    pTempEffect->m_pArrowEffScript = pTempScript;
    // add effect script to effect script manager        
    CSceneManager::AddEffectScript( pTempScript );
    // add arrow effect to list
    m_lstArrowEff.push_back( pTempEffect );
    // Update Effect
	ArrowUpdate( m_lstArrowEff.size() -1 );

    // Set BodyEffect State
    m_bArrowEffExist = true;
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::ArrowUpdate()
void RYLCreatureEffect::ArrowUpdate()
{
    // 몸에 맞은 화살들 업데이트 
	if( m_bArrowEffExist ) 
    {
		unsigned int unCount = 0;

		for( list<RYLArrowEffect*>::iterator iter = m_lstArrowEff.begin() ; iter != m_lstArrowEff.end() ; ++iter )
        {
            if( unCount > m_lstArrowEff.size() )
                break;

            // delete effect
            if( CSceneManager::m_EffectManager.CheckNullScript( (*iter)->m_pArrowEffScript ) )
            {
				DeleteArrow( unCount );
            }

			ArrowUpdate( unCount );

            ++unCount;
		}
	}
}

void RYLCreatureEffect::ArrowUpdate( unsigned int unIndex )
{
    if( !m_pMotherCreature )
        return;

    unsigned int unCount = 0;
    list<RYLArrowEffect*>::iterator iter = m_lstArrowEff.begin();

    while( iter != m_lstArrowEff.end() && unCount < m_lstArrowEff.size() )
    {   
        if( unCount == unIndex )
            break;

        ++iter;
        ++unCount;
    }

    if( (*iter) != NULL ) 
    {
        CZ3DGeneralChrModel* pMotherModel = m_pMotherCreature->GetCharModel();
        
        if( pMotherModel ) 
        {
            CZ3DObject* pBodyObject = pMotherModel->GetSkeletonPartObject( (*iter)->m_strPivotName.c_str() );

            if( pBodyObject )
            {
                D3DXVECTOR3 D3DVecBodyPos;	
                D3DXVECTOR3 D3DVecEffectFwd;

                vector3 vecBody;
                quaternion quaBody;
                D3DXQUATERNION D3DBodyQuat;
                float fBodyScale = m_pMotherCreature->m_pCharModel->GetScale();

                pBodyObject->GetDirection( quaBody );
                pBodyObject->GetPosition( vecBody );

                D3DVecBodyPos.x = vecBody.x;
                D3DVecBodyPos.y = vecBody.y;
                D3DVecBodyPos.z = vecBody.z;

                D3DVecBodyPos *= fBodyScale;

                D3DBodyQuat.x = quaBody.x;
                D3DBodyQuat.y = quaBody.y;
                D3DBodyQuat.z = quaBody.z;
                D3DBodyQuat.w = quaBody.w;

                (*iter)->m_pArrowEffScript->SetExt4Rot( D3DBodyQuat );

                (*iter)->m_pArrowEffScript->SetStartPos( m_pMotherCreature->m_vecPosition.x + D3DVecBodyPos.x, m_pMotherCreature->m_vecPosition.y + D3DVecBodyPos.y, m_pMotherCreature->m_vecPosition.z + D3DVecBodyPos.z );
                float fChrDir = m_pMotherCreature->m_fDirection = m_pMotherCreature->m_pCharModel->GetDirection();
                fChrDir = -fChrDir;
                fChrDir -= FLOAT_PHI / 2.0f;

                vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
                vecChrToward.Normalize();

                (*iter)->m_pArrowEffScript->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
            }
        }
        else 
        {
            delete (*iter);
            (*iter) = NULL;	
            iter = m_lstArrowEff.erase( iter );
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::DeleteArrow()
void RYLCreatureEffect::DeleteArrow( int nIndex )
{
    list<RYLArrowEffect*>::iterator iterArrow = m_lstArrowEff.begin();

    unsigned int unCount = 0;

    while( iterArrow != m_lstArrowEff.end() )
    {
        if( unCount == nIndex )
        {
            delete (*iterArrow);
            *iterArrow = NULL;
            
            iterArrow = m_lstArrowEff.erase( iterArrow );

            break;
        }

        ++unCount;
        ++iterArrow;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatrueEffect::AddWeaponEffect()
void RYLCreatureEffect::AddWeaponEffect( string strEsfName, int nEffectNum, bool bVisible )
{
    if( !m_pMotherCreature )
        return;

    if( GetChantStatus( Skill::SpellID::Hide ) )           // Hide 된 캐릭터의 이펙트는 출력 안함
        return;	

    CZ3DGeneralChrModel* pMotherModel = m_pMotherCreature->GetCharModel();

	if( pMotherModel != NULL ) 
    {
        // 무기의 처음과 끝을 알아낸다.
		vector3 v1, v2;
		vector3 v3;

		// WEAPON 슬롯에 들어있는 무기 class Get
		CZ3DWeapon *pWp = pMotherModel->GetAttachmentObject( "WEAPON" );
		// 처음점, 끝점 얻어낸다. ( v1,v2 어떤것이 위의 점인지는 알수 없당 )
		//상대좌표이다.
		
		if( pWp == NULL )	//무기 존재하지 않을때
			return;

		pWp->GetMarker( v1, v2 );
		
		// 상대좌표이므로 실 좌표는 ( 무기좌표 + 캐릭터 좌표 ) 이다.			

        v1 += *(m_pMotherCreature->GetPosition());
		v2 += *(m_pMotherCreature->GetPosition());
		v3 = v2 - v1;

		for( int i = 0 ; i < nEffectNum ; ++i )						// Add Effect
		{
			float vecLength = ( i + 1 ) / (float)nEffectNum;

			vector3 lopv1 = v1 + ( v3 * vecLength );

			RYLWeaponEffect* pWeaponEffect =  new RYLWeaponEffect;
			
			// pWeaponEffect setting
            pWeaponEffect->m_nWeaponEffectNum   = nEffectNum;
			pWeaponEffect->m_nWeaponPosNum      = i;
			pWeaponEffect->m_strEffectName      = strEsfName;

            CEffScript* pTempScript             = new CEffScript;
            pTempScript->GetScriptBinData( const_cast<char*>( strEsfName.c_str() ), bVisible );
            //lopv1 == 이팩트 위치
            pTempScript->SetStartPos( lopv1.x, lopv1.y, lopv1.z );
            pTempScript->SetEndPos( lopv1.x, lopv1.y, lopv1.z );

            m_pMotherCreature->SetDirection( pMotherModel->GetDirection() );
            float fChrDir = m_pMotherCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

            vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
            vecChrToward.Normalize();

            pTempScript->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );

            // set effect script to effect
            pWeaponEffect->m_pWeaponEffScript   = pTempScript;
            // add effect script to effect script manager
			CSceneManager::m_EffectManager.AddEffScript( pTempScript );
            // add weapon effect to vector
			m_lstWeaponEff.push_back( pWeaponEffect );
        }
    
        // set weapon effect state
        m_bWeaponEffExist = true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::DeleteWeaponEffect()
void RYLCreatureEffect::DeleteWeaponEffect()
{
    for_each( m_lstWeaponEff.begin(), m_lstWeaponEff.end(), ftDeleteWeaponEffectMemory );
    m_lstWeaponEff.clear();

    m_bWeaponEffExist = false;
}

void RYLCreatureEffect::DeleteWeaponEffect( string strEsfName, int nEffNum )
{
    int iEffNum = nEffNum;

    for( list<RYLWeaponEffect*>::iterator iter = m_lstWeaponEff.begin() ; iter != m_lstWeaponEff.end() ; ++iter )
    {
        if( (*iter) )
        {
            if( !CSceneManager::m_EffectManager.CheckNullScript( (*iter)->m_pWeaponEffScript ) )
            {
                // 지우는 갯수 한도를 넘지 않는다 
                if( nEffNum )
                {
                    ftDeleteWeaponEffectMemory( (*iter) );
                    iter = m_lstWeaponEff.erase( iter );
                    --nEffNum;
                }
            }
            else
            {
                ftDeleteWeaponEffectMemory( (*iter) );
            }
        }
        else
        {
            iter = m_lstWeaponEff.erase( iter );
        }
    }

    if( m_lstWeaponEff.size() <= 0 )
    {
        m_bWeaponEffExist = false;
        m_lstWeaponEff.clear();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::WeaponEffectUpdate()
void RYLCreatureEffect::WeaponEffectUpdate()
{
    if( !m_pMotherCreature )
        return;

    if( m_bWeaponEffExist )								//등록된 Weapon Effect 가 있을때
	{
        CZ3DGeneralChrModel* pMotherModel = m_pMotherCreature->GetCharModel();

		if( pMotherModel ) 
        {
			/// 무기의 처음과 끝을 알아낸다.
			vector3 v1, v2;
			vector3 v3;

			// WEAPON 슬롯에 들어있는 무기 class Get
			CZ3DWeapon *pWp = pMotherModel->GetAttachmentObject(  "WEAPON"  );
			
			if( pWp == NULL ) 
            {	//캐릭터가 무기 않들었을시에 weapon effect 전체 삭제
				DeleteWeaponEffect();
				return;
			}

			// 처음점, 끝점 얻어낸다. ( v1,v2 어떤것이 위의 점인지는 알수 없당 )
			//상대좌표이다.
			pWp->GetMarker(  v1, v2  );
			
			///
			
			// 상대좌표이므로 실 좌표는 ( 무기좌표 + 캐릭터 좌표 ) 이다.			
            v1 += *(m_pMotherCreature->GetPosition());
			v2 += *(m_pMotherCreature->GetPosition());
			v3 = v2 - v1;

			if(  m_lstWeaponEff.size() > 0 ) 
            {
				RYLWeaponEffect* pEffect = NULL;

				for( list<RYLWeaponEffect*>::iterator iter = m_lstWeaponEff.begin() ; iter != m_lstWeaponEff.end() ; ++iter )
				{
					pEffect = (*iter);

					if(pEffect != NULL && pEffect->m_pWeaponEffScript != NULL )
					{
						float veclen = ( pEffect->m_nWeaponPosNum + 1 ) / ( float )( pEffect->m_nWeaponEffectNum + 1 );
						
						vector3 lopv1 = v1;
						lopv1 += v3 * veclen;

						//lopv1이 Update 될 effect의 위치
						if( pEffect->m_pWeaponEffScript != NULL && !( CSceneManager::m_EffectManager.CheckNullScript( pEffect->m_pWeaponEffScript ) ) ) 
						{
							pEffect->m_pWeaponEffScript->SetStartPos( lopv1.x, lopv1.y, lopv1.z );
							pEffect->m_pWeaponEffScript->SetEndPos( lopv1.x, lopv1.y, lopv1.z );
							
                            m_pMotherCreature->SetDirection( pMotherModel->GetDirection() );
                            float fChrDir = m_pMotherCreature->GetDirection();
							fChrDir = -fChrDir;
							fChrDir -= FLOAT_PHI / 2.0f;

							vector3 vecChrToward( cosf( fChrDir ),  0.0f,  sinf( fChrDir ) );
							vecChrToward.Normalize();

							pEffect->m_pWeaponEffScript->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
						}
						else 
						{
							delete pEffect;
							pEffect = NULL;	
							iter = m_lstWeaponEff.erase( iter );
						}
					}
					else 
					{	
						iter = m_lstWeaponEff.erase( iter );
					}
				}										//for
			}											//if( m_lstWeaponEff.size() > 0 )
			else 
			{
				m_lstWeaponEff.clear();

                m_bWeaponEffExist = false;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLCreatureEffect::ClearEffect()
void RYLCreatureEffect::ClearEffect()
{   
    for_each( m_lstBodyEff.begin(), m_lstBodyEff.end(), ftDeleteBodyEffectMemory );
    m_lstBodyEff.clear();
    m_bBodyEffExist = false;

    for_each( m_lstArrowEff.begin(), m_lstArrowEff.end(), ftDeleteArrowEffectMemory );
    m_lstArrowEff.clear();
    m_bArrowEffExist = false;

    for_each( m_lstWeaponEff.begin(), m_lstWeaponEff.end(), ftDeleteWeaponEffectMemory );
    m_lstWeaponEff.clear();
    m_bWeaponEffExist = false;
}

