//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreatureEffect.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: single Creature's effect management class
//
// Date: 2004. 4. 8. ( Thu )
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __RYL_CREATURE_EFFECT_H__
#define __RYL_CREATURE_EFFECT_H__
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "SceneManager.h"
#include <Creature/CreatureStructure.h>
#include <string>
#include <vector>
#include <list>
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// struct RYLBodyEffect : Creature 몸에 붙는 Effect
struct RYLBodyEffect
{	
    CEffScript*			m_pBodyEffScript;

    bool				m_bPivotRot;
    int					m_nPivotIndex;
    string				m_strPivotName;
    string              m_strEffName;
    bool				m_bIsUseEff;
    unsigned long		m_ulEffID;

    D3DXVECTOR3			m_D3DVecEffPos;

    RYLBodyEffect() 
    {
        m_pBodyEffScript= NULL;
        m_bPivotRot     = false;
        m_nPivotIndex   = -1;
        m_strPivotName.reserve( 128 );
        m_strEffName.reserve( 128 );
        m_bIsUseEff     = false;
        m_ulEffID       = 0;

        m_D3DVecEffPos.x = m_D3DVecEffPos.y = m_D3DVecEffPos.z = 0.0f;
    }

    ~RYLBodyEffect() 
    {
        m_strPivotName.clear();
        m_strEffName.clear();

        if( m_pBodyEffScript ) 
        {     
            if( !CSceneManager::m_EffectManager.CheckNullScript( m_pBodyEffScript ) ) 
            {
                m_pBodyEffScript->SetAllLoopOff();
                m_pBodyEffScript->SetVisibility( false );
                m_pBodyEffScript = NULL;
            }
        }
    }
};

//--------------------------------------------------------------------------------------------------------------------------
// struct RYLArrowEffect
struct RYLArrowEffect
{
    CEffScript*         m_pArrowEffScript;
	string              m_strPivotName;

	RYLArrowEffect() 
    {
		m_pArrowEffScript = NULL;
        m_strPivotName.clear();
	}

	~RYLArrowEffect() 
    {
        m_strPivotName.clear();

		if( m_pArrowEffScript ) 
        {
			if( !CSceneManager::m_EffectManager.CheckNullScript( m_pArrowEffScript ) )
            {
				m_pArrowEffScript->SetAllLoopOff();
				m_pArrowEffScript->SetVisibility( false );
				m_pArrowEffScript = NULL;
			}
		}
	}
};

//--------------------------------------------------------------------------------------------------------------------------
// struct RYLWeaponEffect : Weapon에 붙는 effect
struct RYLWeaponEffect
{
    CEffScript*			m_pWeaponEffScript;
    string              m_strEffectName;

    int					m_nWeaponEffectNum;		//effect의 갯수 
    int					m_nWeaponPosNum;		//effect의 갯수중에 현 effect 노드의 위치

    RYLWeaponEffect() 
    {
        m_pWeaponEffScript  = NULL;
        m_strEffectName.clear();

        m_nWeaponEffectNum  = 0;		 
        m_nWeaponPosNum     = 0;		
    }

    ~RYLWeaponEffect() 
    {
        if( m_pWeaponEffScript ) 
        {
            if( !CSceneManager::m_EffectManager.CheckNullScript( m_pWeaponEffScript ) )
            {
                m_pWeaponEffScript->SetAllLoopOff();
                m_pWeaponEffScript->SetVisibility( false );
                m_pWeaponEffScript = NULL;
            }
        }
    }
};

//--------------------------------------------------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// pre-defined class
class RYLCreature;

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreatureEffect
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class RYLCreatureEffect
{
private:
            //--------------------------------------------------------------------------------------------------------------------------
            // Effect 객체를 갖고 있는 mother creature 객체의 pointer( 할당/삭제 금지! )
            RYLCreature*            m_pMotherCreature;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with chant effect
            EnchantInfo		        m_EnchantInfo;              // current chant status

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Body Effect
    list<RYLBodyEffect*>            m_lstBodyEff;
            bool                    m_bBodyEffExist;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Arrow Effect
    list<RYLArrowEffect*>           m_lstArrowEff;
            bool                    m_bArrowEffExist;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Weapon Effect
    list<RYLWeaponEffect*>          m_lstWeaponEff;
            bool                    m_bWeaponEffExist;

public:
            //--------------------------------------------------------------------------------------------------------------------------
            // Constructor, Destructor
                                    RYLCreatureEffect( RYLCreature* pMotherCreature );
                                    ~RYLCreatureEffect();

            //--------------------------------------------------------------------------------------------------------------------------
            // set effect visibility
            void                    SetEffectVisibility( bool bVisible );

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Body Effect
            // 캐릭 pos 에 effect 붙일경우
            void                    AddBodyEffect( string strScriptFile, unsigned long ulEffID = 0, bool bMine = true, bool bVisibility = true );
            // pivot 하나에 effect 붙임 ( pivot name으로.. )
            void                    AddBodyEffect( string strScriptFile, string strPivot, bool bRot = false, unsigned long ulEffID = 0 );
            // pivot 하나에 effect 붙임 ( pivot id로.. )
            void                    AddBodyEffect( string strScriptFile, int nPivotID, bool bRot = false, unsigned long ulEffID = 0 );
           
            void                    DeleteBodyEffect();                                                             // 전 effect delete
            void                    DeleteBodyEffect( string strPivot );                                            // 한 pivot에 붙은 effect delete
            void                    DeleteBodyEffect( const unsigned long ulEffID );                                // effect id로 delete
            void                    DeleteBodyEffectName( string strScriptFile );                                   // effect script 이름에 따라 delete

            // all creature update
            void                    BodyEffectUpdate();
            // single creature update
            void                    BodyEffectUpdate( list<RYLBodyEffect*>::iterator& iterEffect, bool bAllUpdate = false );
            
            void                    SetEffect();                                                                    // monster 관련 GCMDS에서 effect 불러 붙이는 코드

            // Monster
            void                    MonsterEffectProcess( const char* pstrFileName, int nEffectValue );

            // chant
    inline  EnchantInfo&            GetChantStatus()                                    { return m_EnchantInfo; }
            bool                    GetChantStatus( unsigned long ulChantKind );
            void                    SetChantEffect( bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow );
            void                    SetChantEffect( const EnchantInfo& Info, bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow );
            void                    SetChantEffect( unsigned long ulChant, bool bToggle, bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow );

            //--------------------------------------------------------------------------------------------------------------------------
            // support body effect 
private:
            bool                    CheckInBodyEffect( string strScriptFile );                                      // 현재 body effect list안에 이 이팩트가 존재 하는지 검사
            void                    RenderSnap( float fBeforex, float fBeforey, float fBeforez,     
                                                float fNowx, float fNowy, float fNowz, 
                                                float fUnitsize, const char *pstrEffName );

public:
    //--------------------------------------------------------------------------------------------------------------------------
    // concerned with Arrow Effect
            // pivot에 arrow 붙이기
		    void                    AddArrow( const char *pstrPivotName, D3DXQUATERNION Quat, int nDiv = 0 );

            // all arrow update
            void                    ArrowUpdate();
            // single arrow update
		    void                    ArrowUpdate( const unsigned int unIndex );

		    void                    DeleteArrow( int nIndex );

    //--------------------------------------------------------------------------------------------------------------------------
    // concerned with Weapon Effect
            void                    AddWeaponEffect( string strEsfName, int nEffectNum, bool bVisible );
		
		    void                    DeleteWeaponEffect();						                                    //전 weapon effect delete
		    void                    DeleteWeaponEffect(string strEsfName, int nEffNum );                            //esfname을 가지는 effect delete

		    void                    WeaponEffectUpdate();

    //--------------------------------------------------------------------------------------------------------------------------
    // clear self object
            void                    ClearEffect();
};

#endif //__RYL_CREATURE_EFFECT_H__